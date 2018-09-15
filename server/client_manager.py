# tsuserver3, an Attorney Online server
#
# Copyright (C) 2016 argoneus <argoneuscze@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from server import fantacrypt
from server import logger
from server.exceptions import ClientError, AreaError
from enum import Enum
from server.constants import TargetType
from heapq import heappop, heappush

import time
import re



class ClientManager:
    class Client:
        def __init__(self, server, transport, user_id, ipid):
            self.is_checked = False
            self.transport = transport
            self.hdid = ''
            self.pm_mute = False
            self.id = user_id
            self.char_id = -1
            self.area = server.area_manager.default_area()
            self.server = server
            self.name = ''
            self.fake_name = ''
            self.is_mod = False
            self.is_dj = True
            self.can_wtce = True
            self.pos = ''
            self.is_cm = False
            self.evi_list = []
            self.disemvowel = False
            self.shaken = False
            self.charcurse = []
            self.muted_global = False
            self.muted_adverts = False
            self.is_muted = False
            self.is_ooc_muted = False
            self.pm_mute = False
            self.mod_call_time = 0
            self.in_rp = False
            self.ipid = ipid
            self.websocket = None

            # Pairing stuff
            self.charid_pair = -1
            self.offset_pair = 0
            self.last_sprite = ''
            self.flip = 0
            self.claimed_folder = ''
            
            #flood-guard stuff
            self.mus_counter = 0
            self.mus_mute_time = 0
            self.mus_change_time = [x * self.server.config['music_change_floodguard']['interval_length'] for x in range(self.server.config['music_change_floodguard']['times_per_interval'])]
            self.wtce_counter = 0
            self.wtce_mute_time = 0
            self.wtce_time = [x * self.server.config['wtce_floodguard']['interval_length'] for x in range(self.server.config['wtce_floodguard']['times_per_interval'])]

        def send_raw_message(self, msg):
            if self.websocket:
                self.websocket.send_text(msg.encode('utf-8'))
            else:
                self.transport.write(msg.encode('utf-8'))

        def send_command(self, command, *args):
            if args:
                if command == 'MS':
                    for evi_num in range(len(self.evi_list)):
                        if self.evi_list[evi_num] == args[11]:
                            lst = list(args)
                            lst[11] = evi_num
                            args = tuple(lst)
                            break
                self.send_raw_message('{}#{}#%'.format(command, '#'.join([str(x) for x in args])))
            else:
                self.send_raw_message('{}#%'.format(command))

        def send_host_message(self, msg):
            self.send_command('CT', self.server.config['hostname'], msg)

        def send_motd(self):
            self.send_host_message('=== MOTD ===\r\n{}\r\n============='.format(self.server.config['motd']))

        def send_player_count(self):
            self.send_host_message('{}/{} players online.'.format(
                self.server.get_player_count(),
                self.server.config['playerlimit']))

        def is_valid_name(self, name):
            name_ws = name.replace(' ', '')
            if not name_ws or name_ws.isdigit():
                return False
            for client in self.server.client_manager.clients:
                print(client.name == name)
                if client.name == name:
                    return False
            return True
            
        def disconnect(self):
            self.transport.close()

        def change_character(self, char_id, force=False):
            if not self.server.is_valid_char_id(char_id):
                raise ClientError('Invalid Character ID.')
            if len(self.charcurse) > 0:
                if not char_id in self.charcurse:
                    raise ClientError('Character not available.')
                force = True
            if not self.area.is_char_available(char_id):
                if force:
                    for client in self.area.clients:
                        if client.char_id == char_id:
                            client.char_select()
                else:
                    raise ClientError('Character not available.')
            old_char = self.get_char_name()
            self.char_id = char_id
            self.pos = ''
            self.send_command('PV', self.id, 'CID', self.char_id)
            self.area.send_command('CharsCheck', *self.get_available_char_list())
            logger.log_server('[{}]Changed character from {} to {}.'
                              .format(self.area.abbreviation, old_char, self.get_char_name()), self)

        def change_music_cd(self):
            if self.is_mod or self.is_cm:
                return 0
            if self.mus_mute_time:
                if time.time() - self.mus_mute_time < self.server.config['music_change_floodguard']['mute_length']:
                    return self.server.config['music_change_floodguard']['mute_length'] - (time.time() - self.mus_mute_time)
                else:
                    self.mus_mute_time = 0
            times_per_interval = self.server.config['music_change_floodguard']['times_per_interval']
            interval_length = self.server.config['music_change_floodguard']['interval_length']
            if time.time() - self.mus_change_time[(self.mus_counter - times_per_interval + 1) % times_per_interval] < interval_length:
                self.mus_mute_time = time.time()
                return self.server.config['music_change_floodguard']['mute_length']
            self.mus_counter = (self.mus_counter + 1) % times_per_interval
            self.mus_change_time[self.mus_counter] = time.time()
            return 0

        def wtce_mute(self):
            if self.is_mod or self.is_cm:
                return 0
            if self.wtce_mute_time:
                if time.time() - self.wtce_mute_time < self.server.config['wtce_floodguard']['mute_length']:
                    return self.server.config['wtce_floodguard']['mute_length'] - (time.time() - self.wtce_mute_time)
                else:
                    self.wtce_mute_time = 0
            times_per_interval = self.server.config['wtce_floodguard']['times_per_interval']
            interval_length = self.server.config['wtce_floodguard']['interval_length']
            if time.time() - self.wtce_time[(self.wtce_counter - times_per_interval + 1) % times_per_interval] < interval_length:
                self.wtce_mute_time = time.time()
                return self.server.config['music_change_floodguard']['mute_length']
            self.wtce_counter = (self.wtce_counter + 1) % times_per_interval
            self.wtce_time[self.wtce_counter] = time.time()
            return 0

        def reload_character(self):
            try:
                self.change_character(self.char_id, True)
            except ClientError:
                raise

        def change_area(self, area):
            if self.area == area:
                raise ClientError('User already in specified area.')
            if area.is_locked == area.Locked.LOCKED and not self.is_mod and not self.id in area.invite_list:
                raise ClientError("That area is locked!")
            if area.is_locked == area.Locked.SPECTATABLE and not self.is_mod and not self.id in area.invite_list:
                self.send_host_message('This area is spectatable, but not free - you will be unable to send messages ICly unless invited.')

            if self.area.jukebox:
                self.area.remove_jukebox_vote(self, True)

            old_area = self.area
            if not area.is_char_available(self.char_id):
                try:
                    new_char_id = area.get_rand_avail_char_id()
                except AreaError:
                    raise ClientError('No available characters in that area.')

                self.change_character(new_char_id)
                self.send_host_message('Character taken, switched to {}.'.format(self.get_char_name()))

            self.area.remove_client(self)
            self.area = area
            area.new_client(self)

            self.send_host_message('Changed area to {}.[{}]'.format(area.name, self.area.status))
            logger.log_server(
                '[{}]Changed area from {} ({}) to {} ({}).'.format(self.get_char_name(), old_area.name, old_area.id,
                                                                   self.area.name, self.area.id), self)
            self.area.send_command('CharsCheck', *self.get_available_char_list())
            self.send_command('HP', 1, self.area.hp_def)
            self.send_command('HP', 2, self.area.hp_pro)
            self.send_command('BN', self.area.background)
            self.send_command('LE', *self.area.get_evidence_list(self))

        def send_area_list(self):
            msg = '=== Areas ==='
            for i, area in enumerate(self.server.area_manager.areas):
                owner = 'FREE'
                if area.owned:
                    for client in [x for x in area.clients if x.is_cm]:
                        owner = 'CM: {}'.format(client.get_char_name())
                        break
                lock = {area.Locked.FREE: '', area.Locked.SPECTATABLE: '[SPECTATABLE]', area.Locked.LOCKED: '[LOCKED]'}
                msg += '\r\nArea {}: {} (users: {}) [{}][{}]{}'.format(area.abbreviation, area.name, len(area.clients), area.status, owner, lock[area.is_locked])
                if self.area == area:
                    msg += ' [*]'
            self.send_host_message(msg)

        def get_area_info(self, area_id, mods):
            info = '\r\n'
            try:
                area = self.server.area_manager.get_area_by_id(area_id)
            except AreaError:
                raise
            info += '=== {} ==='.format(area.name)
            info += '\r\n'

            lock = {area.Locked.FREE: '', area.Locked.SPECTATABLE: '[SPECTATABLE]', area.Locked.LOCKED: '[LOCKED]'}
            info += '[{}]: [{} users][{}]{}'.format(area.abbreviation, len(area.clients), area.status, lock[area.is_locked])
            
            sorted_clients = []
            for client in area.clients:
                if (not mods) or client.is_mod:
                    sorted_clients.append(client)
            if not sorted_clients:
                return ''
            sorted_clients = sorted(sorted_clients, key=lambda x: x.get_char_name())
            for c in sorted_clients:
                info += '\r\n'
                if c.is_cm:
                    info +='[CM]'
                info += '[{}] {}'.format(c.id, c.get_char_name())
                if self.is_mod:
                    info += ' ({})'.format(c.ipid)
                    info += ': {}'.format(c.name)

            return info

        def send_area_info(self, area_id, mods): 
            #if area_id is -1 then return all areas. If mods is True then return only mods
            info = ''
            if area_id == -1:
                # all areas info
                cnt = 0
                info = '\n== Area List =='
                for i in range(len(self.server.area_manager.areas)):
                    if len(self.server.area_manager.areas[i].clients) > 0:
                        cnt += len(self.server.area_manager.areas[i].clients)
                        info += '{}'.format(self.get_area_info(i, mods))
                info = 'Current online: {}'.format(cnt) + info
            else:
                try:
                    info = 'People in this area: {}\n'.format(len(self.server.area_manager.areas[area_id].clients)) + self.get_area_info(area_id, mods)
                except AreaError:
                    raise
            self.send_host_message(info)

        def send_area_hdid(self, area_id):
            try:
                info = self.get_area_hdid(area_id)
            except AreaError:
                raise
            self.send_host_message(info)				

        def send_all_area_hdid(self):
            info = '== HDID List =='
            for i in range (len(self.server.area_manager.areas)):
                 if len(self.server.area_manager.areas[i].clients) > 0:
                    info += '\r\n{}'.format(self.get_area_hdid(i))
            self.send_host_message(info)			

        def send_all_area_ip(self):
            info = '== IP List =='
            for i in range (len(self.server.area_manager.areas)):
                 if len(self.server.area_manager.areas[i].clients) > 0:
                    info += '\r\n{}'.format(self.get_area_ip(i))
            self.send_host_message(info)
			
        def send_done(self):
            self.send_command('CharsCheck', *self.get_available_char_list())
            self.send_command('HP', 1, self.area.hp_def)
            self.send_command('HP', 2, self.area.hp_pro)
            self.send_command('BN', self.area.background)
            self.send_command('LE', *self.area.get_evidence_list(self))
            self.send_command('MM', 1)
            
            self.server.area_manager.send_arup_players()
            self.server.area_manager.send_arup_status()
            self.server.area_manager.send_arup_cms()
            self.server.area_manager.send_arup_lock()

            self.send_command('DONE')

        def char_select(self):
            self.char_id = -1
            self.send_done()

        def get_available_char_list(self):
            if len(self.charcurse) > 0:
                avail_char_ids = set(range(len(self.server.char_list))) and set(self.charcurse)
            else:
                avail_char_ids = set(range(len(self.server.char_list))) - set([x.char_id for x in self.area.clients])
            char_list = [-1] * len(self.server.char_list)
            for x in avail_char_ids:
                char_list[x] = 0
            return char_list

        def auth_mod(self, password):
            if self.is_mod:
                raise ClientError('Already logged in.')
            if password == self.server.config['modpass']:
                self.is_mod = True
            else:
                raise ClientError('Invalid password.')

        def get_ip(self):
            return self.ipid


		
        def get_char_name(self):
            if self.char_id == -1:
                return 'CHAR_SELECT'
            return self.server.char_list[self.char_id]

        def change_position(self, pos=''):
            if pos not in ('', 'def', 'pro', 'hld', 'hlp', 'jud', 'wit', 'jur', 'sea'):
                raise ClientError('Invalid position. Possible values: def, pro, hld, hlp, jud, wit, jur, sea.')
            self.pos = pos

        def set_mod_call_delay(self):
            self.mod_call_time = round(time.time() * 1000.0 + 30000)

        def can_call_mod(self):
            return (time.time() * 1000.0 - self.mod_call_time) > 0

        def disemvowel_message(self, message):
            message = re.sub("[aeiou]", "", message, flags=re.IGNORECASE)
            return re.sub(r"\s+", " ", message)
        
        def shake_message(self, message):
            import random
            parts = message.split()
            random.shuffle(parts)
            return ' '.join(parts)
            

    def __init__(self, server):
        self.clients = set()
        self.server = server
        self.cur_id = [i for i in range(self.server.config['playerlimit'])]
        self.clients_list = []

    def new_client(self, transport):
        c = self.Client(self.server, transport, heappop(self.cur_id), self.server.get_ipid(transport.get_extra_info('peername')[0]))
        self.clients.add(c)
        return c

            
    def remove_client(self, client):
        if client.area.jukebox:
            client.area.remove_jukebox_vote(client, True)
        heappush(self.cur_id, client.id)
        self.clients.remove(client)
		
    def get_targets(self, client, key, value, local = False):
        #possible keys: ip, OOC, id, cname, ipid, hdid
        areas = None
        if local:
            areas = [client.area]
        else:
            areas = client.server.area_manager.areas
        targets = []
        if key == TargetType.ALL:
            for nkey in range(6):
                targets += self.get_targets(client, nkey, value, local)
        for area in areas:
            for client in area.clients:
                if key == TargetType.IP:
                    if value.lower().startswith(client.get_ip().lower()):
                        targets.append(client)
                elif key == TargetType.OOC_NAME:
                    if value.lower().startswith(client.name.lower()) and client.name:
                        targets.append(client)
                elif key == TargetType.CHAR_NAME:
                    if value.lower().startswith(client.get_char_name().lower()):
                        targets.append(client)
                elif key == TargetType.ID:
                    if client.id == value:
                        targets.append(client)
                elif key == TargetType.IPID:
                    if client.ipid == value:
                        targets.append(client)
        return targets
            
        
    def get_muted_clients(self):
        clients = []
        for client in self.clients:
            if client.is_muted:
                clients.append(client)
        return clients

    def get_ooc_muted_clients(self):
        clients = []
        for client in self.clients:
            if client.is_ooc_muted:
                clients.append(client)
        return clients
