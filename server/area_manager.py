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
import asyncio
import random

import time
import yaml

from server.exceptions import AreaError
from server.evidence import EvidenceList


class AreaManager:
    class Area:
        def __init__(self, area_id, server, name, background, bg_lock, evidence_mod = 'FFA', locking_allowed = False, iniswap_allowed = True, showname_changes_allowed = False, shouts_allowed = True, jukebox = False, abbreviation = ''):
            self.iniswap_allowed = iniswap_allowed
            self.clients = set()
            self.invite_list = {}
            self.id = area_id
            self.name = name
            self.background = background
            self.bg_lock = bg_lock
            self.server = server
            self.music_looper = None
            self.next_message_time = 0
            self.hp_def = 10
            self.hp_pro = 10
            self.doc = 'No document.'
            self.status = 'IDLE'
            self.judgelog = []
            self.current_music = ''
            self.current_music_player = ''
            self.current_music_player_ipid = -1
            self.evi_list = EvidenceList()
            self.is_recording = False
            self.recorded_messages = []
            self.evidence_mod = evidence_mod
            self.locking_allowed = locking_allowed
            self.showname_changes_allowed = showname_changes_allowed
            self.shouts_allowed = shouts_allowed
            self.abbreviation = abbreviation
            self.owned = False
            self.cards = dict()

            """
            #debug
            self.evidence_list.append(Evidence("WOW", "desc", "1.png"))
            self.evidence_list.append(Evidence("wewz", "desc2", "2.png"))
            self.evidence_list.append(Evidence("weeeeeew", "desc3", "3.png"))
            """
            
            self.is_locked = False

            self.jukebox = jukebox
            self.jukebox_votes = []

        def new_client(self, client):
            self.clients.add(client)

        def remove_client(self, client):
            self.clients.remove(client)
            if client.is_cm:
                client.is_cm = False
                self.owned = False
                if self.is_locked:
                    self.unlock()
        
        def unlock(self):
            self.is_locked = False
            self.invite_list = {}
            self.send_host_message('This area is open now.')
        
        def is_char_available(self, char_id):
            return char_id not in [x.char_id for x in self.clients]

        def get_rand_avail_char_id(self):
            avail_set = set(range(len(self.server.char_list))) - set([x.char_id for x in self.clients])
            if len(avail_set) == 0:
                raise AreaError('No available characters.')
            return random.choice(tuple(avail_set))

        def send_command(self, cmd, *args):
            for c in self.clients:
                c.send_command(cmd, *args)

        def send_host_message(self, msg):
            self.send_command('CT', self.server.config['hostname'], msg)

        def set_next_msg_delay(self, msg_length):
            delay = min(3000, 100 + 60 * msg_length)
            self.next_message_time = round(time.time() * 1000.0 + delay)
        
        def is_iniswap(self, client, anim1, anim2, char):
            if self.iniswap_allowed:
                return False
            if '..' in anim1 or '..' in anim2:
                return True
            for char_link in self.server.allowed_iniswaps:
                if client.get_char_name() in char_link and char in char_link:
                    return False
            return True

        def add_jukebox_vote(self, client, music_name, length=-1, showname=''):
            if length <= 0:
                self.remove_jukebox_vote(client, False)
            else:
                self.remove_jukebox_vote(client, True)
                self.jukebox_votes.append(self.JukeboxVote(client, music_name, length, showname))
                client.send_host_message('Your song was added to the jukebox.')
                if len(self.jukebox_votes) == 1:
                    self.start_jukebox()
        
        def remove_jukebox_vote(self, client, silent):
            for current_vote in self.jukebox_votes:
                if current_vote.client.id == client.id:
                    self.jukebox_votes.remove(current_vote)
            if not silent:        
                client.send_host_message('You removed your song from the jukebox.')
        
        def get_jukebox_picked(self):
            if len(self.jukebox_votes) == 0:
                return None
            elif len(self.jukebox_votes) == 1:
                return self.jukebox_votes[0]
            else:
                weighted_votes = []
                for current_vote in self.jukebox_votes:
                    i = 0
                    while i < current_vote.chance:
                        weighted_votes.append(current_vote)
                        i += 1
                return random.choice(weighted_votes)

        def start_jukebox(self):
            # There is a probability that the jukebox feature has been turned off since then,
            # we should check that.
            # We also do a check if we were the last to play a song, just in case.
            if not self.jukebox:
                if self.current_music_player == 'The Jukebox' and self.current_music_player_ipid == 'has no IPID':
                    self.current_music = ''
                return

            vote_picked = self.get_jukebox_picked()

            if vote_picked is None:
                self.current_music = ''
                return

            if vote_picked.showname == '':
                self.send_command('MC', vote_picked.name, vote_picked.client.char_id)
            else:
                self.send_command('MC', vote_picked.name, vote_picked.client.char_id, vote_picked.showname)

            self.current_music_player = 'The Jukebox'
            self.current_music_player_ipid = 'has no IPID'
            self.current_music = vote_picked.name
            
            for current_vote in self.jukebox_votes:
                # Choosing the same song will get your votes down to 0, too.
                # Don't want the same song twice in a row!
                if current_vote.name == vote_picked.name:
                    current_vote.chance = 0
                else:
                    current_vote.chance += 1

            if self.music_looper:
                self.music_looper.cancel()
            self.music_looper = asyncio.get_event_loop().call_later(vote_picked.length, lambda: self.start_jukebox())
        
        def play_music(self, name, cid, length=-1):
            self.send_command('MC', name, cid)
            if self.music_looper:
                self.music_looper.cancel()
            if length > 0:
                self.music_looper = asyncio.get_event_loop().call_later(length,
                                                                        lambda: self.play_music(name, -1, length))
        
        def play_music_shownamed(self, name, cid, showname, length=-1):
            self.send_command('MC', name, cid, showname)
            if self.music_looper:
                self.music_looper.cancel()
            if length > 0:
                self.music_looper = asyncio.get_event_loop().call_later(length,
                                                                        lambda: self.play_music(name, -1, length))


        def can_send_message(self, client):
            if self.is_locked and not client.is_mod and not client.ipid in self.invite_list:
                client.send_host_message('This is a locked area - ask the CM to speak.')
                return False
            return (time.time() * 1000.0 - self.next_message_time) > 0

        def change_hp(self, side, val):
            if not 0 <= val <= 10:
                raise AreaError('Invalid penalty value.')
            if not 1 <= side <= 2:
                raise AreaError('Invalid penalty side.')
            if side == 1:
                self.hp_def = val
            elif side == 2:
                self.hp_pro = val
            self.send_command('HP', side, val)

        def change_background(self, bg):
            if bg.lower() not in (name.lower() for name in self.server.backgrounds):
                raise AreaError('Invalid background name.')
            self.background = bg
            self.send_command('BN', self.background)

        def change_status(self, value):
            allowed_values = ('idle', 'rp', 'casing', 'looking-for-players', 'lfp', 'recess', 'gaming')
            if value.lower() not in allowed_values:
                raise AreaError('Invalid status. Possible values: {}'.format(', '.join(allowed_values)))
            if value.lower() == 'lfp':
                value = 'looking-for-players'
            self.status = value.upper()

        def change_doc(self, doc='No document.'):
            self.doc = doc

        def add_to_judgelog(self, client, msg):
            if len(self.judgelog) >= 10:
                self.judgelog = self.judgelog[1:]
            self.judgelog.append('{} ({}) {}.'.format(client.get_char_name(), client.get_ip(), msg))

        def add_music_playing(self, client, name):
            self.current_music_player = client.get_char_name()
            self.current_music_player_ipid = client.ipid
            self.current_music = name

        def add_music_playing_shownamed(self, client, showname, name):
            self.current_music_player = showname + " (" + client.get_char_name() + ")"
            self.current_music_player_ipid = client.ipid
            self.current_music = name

        def get_evidence_list(self, client):
            client.evi_list, evi_list = self.evi_list.create_evi_list(client)
            return evi_list

        def broadcast_evidence_list(self):
            """
                LE#<name>&<desc>&<img>#<name>
                
            """
            for client in self.clients:
                client.send_command('LE', *self.get_evidence_list(client))
        
        class JukeboxVote:
            def __init__(self, client, name, length, showname):
                self.client = client
                self.name = name
                self.length = length
                self.chance = 1
                self.showname = showname

    def __init__(self, server):
        self.server = server
        self.cur_id = 0
        self.areas = []
        self.load_areas()

    def load_areas(self):
        with open('config/areas.yaml', 'r') as chars:
            areas = yaml.load(chars)
        for item in areas:
            if 'evidence_mod' not in item:
                item['evidence_mod'] = 'FFA'
            if 'locking_allowed' not in item:
                item['locking_allowed'] = False
            if 'iniswap_allowed' not in item:
                item['iniswap_allowed'] = True
            if 'showname_changes_allowed' not in item:
                item['showname_changes_allowed'] = False
            if 'shouts_allowed' not in item:
                item['shouts_allowed'] = True
            if 'jukebox' not in item:
                item['jukebox'] = False
            if 'abbreviation' not in item:
                item['abbreviation'] = self.get_generated_abbreviation(item['area'])
            self.areas.append(
                self.Area(self.cur_id, self.server, item['area'], item['background'], item['bglock'], item['evidence_mod'], item['locking_allowed'], item['iniswap_allowed'], item['showname_changes_allowed'], item['shouts_allowed'], item['jukebox'], item['abbreviation']))
            self.cur_id += 1

    def default_area(self):
        return self.areas[0]

    def get_area_by_name(self, name):
        for area in self.areas:
            if area.name == name:
                return area
        raise AreaError('Area not found.')

    def get_area_by_id(self, num):
        for area in self.areas:
            if area.id == num:
                return area
        raise AreaError('Area not found.')

    def get_generated_abbreviation(self, name):
        if name.lower().startswith("courtroom"):
            return "CR" + name.split()[-1]
        elif name.lower().startswith("area"):
            return "A" + name.split()[-1]
        elif len(name.split()) > 1:
            return "".join(item[0].upper() for item in name.split())
        elif len(name) > 3:
            return name[:3].upper()
        else:
            return name.upper()
