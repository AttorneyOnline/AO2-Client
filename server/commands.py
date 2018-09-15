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
#possible keys: ip, OOC, id, cname, ipid, hdid
import random
import hashlib
import string
from server.constants import TargetType

from server import logger
from server.exceptions import ClientError, ServerError, ArgumentError, AreaError


def ooc_cmd_switch(client, arg):
    if len(arg) == 0:
        raise ArgumentError('You must specify a character name.')
    try:
        cid = client.server.get_char_id_by_name(arg)
    except ServerError:
        raise
    try:
        client.change_character(cid, client.is_mod)
    except ClientError:
        raise
    client.send_host_message('Character changed.')
    
def ooc_cmd_bg(client, arg):
    if len(arg) == 0:
        raise ArgumentError('You must specify a name. Use /bg <background>.')
    if not client.is_mod and client.area.bg_lock == "true":
        raise AreaError("This area's background is locked")
    try:
        client.area.change_background(arg)
    except AreaError:
        raise
    client.area.send_host_message('{} changed the background to {}.'.format(client.get_char_name(), arg))
    logger.log_server('[{}][{}]Changed background to {}'.format(client.area.abbreviation, client.get_char_name(), arg), client)

def ooc_cmd_bglock(client,arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if len(arg) != 0:
        raise ArgumentError('This command has no arguments.')
    if client.area.bg_lock  == "true":
        client.area.bg_lock = "false"
    else:
        client.area.bg_lock = "true"
    client.area.send_host_message('{} [{}] has set the background lock to {}.'.format(client.get_char_name(), client.id, client.area.bg_lock))
    logger.log_server('[{}][{}]Changed bglock to {}'.format(client.area.abbreviation, client.get_char_name(), client.area.bg_lock), client)
    
def ooc_cmd_evidence_mod(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if not arg:
        client.send_host_message('current evidence mod: {}'.format(client.area.evidence_mod))
        return
    if arg in ['FFA', 'Mods', 'CM', 'HiddenCM']:
        if arg == client.area.evidence_mod:
            client.send_host_message('current evidence mod: {}'.format(client.area.evidence_mod))
            return
        if client.area.evidence_mod == 'HiddenCM':
            for i in range(len(client.area.evi_list.evidences)):
                client.area.evi_list.evidences[i].pos = 'all'
        client.area.evidence_mod = arg
        client.send_host_message('current evidence mod: {}'.format(client.area.evidence_mod))
        return
    else:
        raise ArgumentError('Wrong Argument. Use /evidence_mod <MOD>. Possible values: FFA, CM, Mods, HiddenCM')
        return
        
def ooc_cmd_allow_iniswap(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    client.area.iniswap_allowed = not client.area.iniswap_allowed
    answer = {True: 'allowed', False: 'forbidden'}
    client.send_host_message('iniswap is {}.'.format(answer[client.area.iniswap_allowed]))
    return
    
def ooc_cmd_allow_blankposting(client, arg):
    if not client.is_mod and not client.is_cm:
        raise ClientError('You must be authorized to do that.')
    client.area.blankposting_allowed = not client.area.blankposting_allowed
    answer = {True: 'allowed', False: 'forbidden'}
    client.area.send_host_message('{} [{}] has set blankposting in the area to {}.'.format(client.get_char_name(), client.id, answer[client.area.blankposting_allowed]))
    return
    
def ooc_cmd_force_nonint_pres(client, arg):
    if not client.is_mod and not client.is_cm:
        raise ClientError('You must be authorized to do that.')
    client.area.non_int_pres_only = not client.area.non_int_pres_only
    answer = {True: 'non-interrupting only', False: 'non-interrupting or interrupting as you choose'}
    client.area.send_host_message('{} [{}] has set pres in the area to be {}.'.format(client.get_char_name(), client.id, answer[client.area.non_int_pres_only]))
    return
    
def ooc_cmd_roll(client, arg):
    roll_max = 11037
    if len(arg) != 0:
        try:
            val = list(map(int, arg.split(' ')))
            if not 1 <= val[0] <= roll_max:
                raise ArgumentError('Roll value must be between 1 and {}.'.format(roll_max))
        except ValueError:
            raise ArgumentError('Wrong argument. Use /roll [<max>] [<num of rolls>]')
    else:
        val = [6]
    if len(val) == 1:
        val.append(1)
    if len(val) > 2:
        raise ArgumentError('Too many arguments. Use /roll [<max>] [<num of rolls>]')
    if val[1] > 20 or val[1] < 1:
        raise ArgumentError('Num of rolls must be between 1 and 20')
    roll = ''
    for i in range(val[1]):
        roll += str(random.randint(1, val[0])) + ', '
    roll = roll[:-2]
    if val[1] > 1:
        roll = '(' + roll + ')'
    client.area.send_host_message('{} rolled {} out of {}.'.format(client.get_char_name(), roll, val[0]))
    logger.log_server(
        '[{}][{}]Used /roll and got {} out of {}.'.format(client.area.abbreviation, client.get_char_name(), roll, val[0]), client)
        
def ooc_cmd_rollp(client, arg):
    roll_max = 11037
    if len(arg) != 0:
        try:
            val = list(map(int, arg.split(' ')))
            if not 1 <= val[0] <= roll_max:
                raise ArgumentError('Roll value must be between 1 and {}.'.format(roll_max))
        except ValueError:
            raise ArgumentError('Wrong argument. Use /roll [<max>] [<num of rolls>]')
    else:
        val = [6]
    if len(val) == 1:
        val.append(1)
    if len(val) > 2:
        raise ArgumentError('Too many arguments. Use /roll [<max>] [<num of rolls>]')
    if val[1] > 20 or val[1] < 1:
        raise ArgumentError('Num of rolls must be between 1 and 20')
    roll = ''
    for i in range(val[1]):
        roll += str(random.randint(1, val[0])) + ', '
    roll = roll[:-2]
    if val[1] > 1:
        roll = '(' + roll + ')'
    client.send_host_message('{} rolled {} out of {}.'.format(client.get_char_name(), roll, val[0]))
    client.area.send_host_message('{} rolled.'.format(client.get_char_name(), roll, val[0]))
    SALT = ''.join(random.choices(string.ascii_uppercase + string.digits, k=16))
    logger.log_server(
        '[{}][{}]Used /roll and got {} out of {}.'.format(client.area.abbreviation, client.get_char_name(), hashlib.sha1((str(roll) + SALT).encode('utf-8')).hexdigest() + '|' + SALT, val[0]), client)

def ooc_cmd_currentmusic(client, arg):
    if len(arg) != 0:
        raise ArgumentError('This command has no arguments.')
    if client.area.current_music == '':
        raise ClientError('There is no music currently playing.')
    if client.is_mod:
        client.send_host_message('The current music is {} and was played by {} ({}).'.format(client.area.current_music,
                                                                                    client.area.current_music_player, client.area.current_music_player_ipid))
    else:
        client.send_host_message('The current music is {} and was played by {}.'.format(client.area.current_music,
                                                                                    client.area.current_music_player))

def ooc_cmd_jukebox_toggle(client, arg):
    if not client.is_mod and not client.is_cm:
        raise ClientError('You must be authorized to do that.')
    if len(arg) != 0:
        raise ArgumentError('This command has no arguments.')
    client.area.jukebox = not client.area.jukebox
    client.area.jukebox_votes = []
    client.area.send_host_message('{} [{}] has set the jukebox to {}.'.format(client.get_char_name(), client.id, client.area.jukebox))

def ooc_cmd_jukebox_skip(client, arg):
    if not client.is_mod and not client.is_cm:
        raise ClientError('You must be authorized to do that.')
    if len(arg) != 0:
        raise ArgumentError('This command has no arguments.')
    if not client.area.jukebox:
        raise ClientError('This area does not have a jukebox.')
    if len(client.area.jukebox_votes) == 0:
        raise ClientError('There is no song playing right now, skipping is pointless.')
    client.area.start_jukebox()
    if len(client.area.jukebox_votes) == 1:
        client.area.send_host_message('{} [{}] has forced a skip, restarting the only jukebox song.'.format(client.get_char_name(), client.id))
    else:
        client.area.send_host_message('{} [{}] has forced a skip to the next jukebox song.'.format(client.get_char_name(), client.id))
    logger.log_server('[{}][{}]Skipped the current jukebox song.'.format(client.area.abbreviation, client.get_char_name()), client)

def ooc_cmd_jukebox(client, arg):
    if len(arg) != 0:
        raise ArgumentError('This command has no arguments.')
    if not client.area.jukebox:
        raise ClientError('This area does not have a jukebox.')
    if len(client.area.jukebox_votes) == 0:
        client.send_host_message('The jukebox has no songs in it.')
    else:
        total = 0
        songs = []
        voters = dict()
        chance = dict()
        message = ''

        for current_vote in client.area.jukebox_votes:
            if songs.count(current_vote.name) == 0:
                songs.append(current_vote.name)
                voters[current_vote.name] = [current_vote.client]
                chance[current_vote.name] = current_vote.chance
            else:
                voters[current_vote.name].append(current_vote.client)
                chance[current_vote.name] += current_vote.chance
            total += current_vote.chance
        
        for song in songs:
            message += '\n- ' + song + '\n'
            message += '-- VOTERS: '
            
            first = True
            for voter in voters[song]:
                if first:
                    first = False
                else:
                    message += ', '
                message += voter.get_char_name() + ' [' + str(voter.id) + ']'
                if client.is_mod:
                    message += '(' + str(voter.ipid) + ')'
            message += '\n'

            if total == 0:
                message += '-- CHANCE: 100'
            else:
                message += '-- CHANCE: ' + str(round(chance[song] / total * 100))

        client.send_host_message('The jukebox has the following songs in it:{}'.format(message))

def ooc_cmd_coinflip(client, arg):
    if len(arg) != 0:
        raise ArgumentError('This command has no arguments.')
    coin = ['heads', 'tails']
    flip = random.choice(coin)
    client.area.send_host_message('{} flipped a coin and got {}.'.format(client.get_char_name(), flip))
    logger.log_server(
        '[{}][{}]Used /coinflip and got {}.'.format(client.area.abbreviation, client.get_char_name(), flip), client)
    
def ooc_cmd_motd(client, arg):
    if len(arg) != 0:
        raise ArgumentError("This command doesn't take any arguments")
    client.send_motd()
    
def ooc_cmd_pos(client, arg):
    if len(arg) == 0:
        client.change_position()
        client.send_host_message('Position reset.')
    else:
        try:
            client.change_position(arg)
        except ClientError:
            raise
        client.area.broadcast_evidence_list()
        client.send_host_message('Position changed.')   

def ooc_cmd_forcepos(client, arg):
    if not client.is_cm and not client.is_mod:
        raise ClientError('You must be authorized to do that.')

    args = arg.split()

    if len(args) < 1:
        raise ArgumentError(
            'Not enough arguments. Use /forcepos <pos> <target>. Target should be ID, OOC-name or char-name. Use /getarea for getting info like "[ID] char-name".')

    targets = []

    pos = args[0]
    if len(args) > 1:
        targets = client.server.client_manager.get_targets(
            client, TargetType.CHAR_NAME, " ".join(args[1:]), True)
        if len(targets) == 0 and args[1].isdigit():
            targets = client.server.client_manager.get_targets(
                client, TargetType.ID, int(arg[1]), True)
        if len(targets) == 0:
            targets = client.server.client_manager.get_targets(
                client, TargetType.OOC_NAME, " ".join(args[1:]), True)
        if len(targets) == 0:
            raise ArgumentError('No targets found.')
    else:
        for c in client.area.clients:
            targets.append(c)



    for t in targets:
        try:
            t.change_position(pos)
            t.area.broadcast_evidence_list()
            t.send_host_message('Forced into /pos {}.'.format(pos))
        except ClientError:
            raise

    client.area.send_host_message(
        '{} forced {} client(s) into /pos {}.'.format(client.get_char_name(), len(targets), pos))
    logger.log_server(
        '[{}][{}]Used /forcepos {} for {} client(s).'.format(client.area.abbreviation, client.get_char_name(), pos, len(targets)), client)

def ooc_cmd_help(client, arg):
    if len(arg) != 0:
        raise ArgumentError('This command has no arguments.')
    help_url = 'http://casecafe.byethost14.com/commandlist'
    help_msg = 'The commands available on this server can be found here: {}'.format(help_url)
    client.send_host_message(help_msg)
        
def ooc_cmd_kick(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if len(arg) == 0:
        raise ArgumentError('You must specify a target. Use /kick <ipid> <ipid> ...')
    args = list(arg.split(' '))
    client.send_host_message('Attempting to kick {} IPIDs.'.format(len(args)))
    for raw_ipid in args:
        try:
            ipid = int(raw_ipid)
        except:
            raise ClientError('{} does not look like a valid IPID.'.format(raw_ipid))
        targets = client.server.client_manager.get_targets(client, TargetType.IPID, ipid, False)
        if targets:
            for c in targets:
                logger.log_server('Kicked {} [{}]({}).'.format(c.get_char_name(), c.id, c.ipid), client)
                logger.log_mod('Kicked {} [{}]({}).'.format(c.get_char_name(), c.id, c.ipid), client)
                client.send_host_message("{} was kicked.".format(c.get_char_name()))
                c.disconnect()
        else:
            client.send_host_message("No targets with the IPID {} were found.".format(ipid))

def ooc_cmd_ban(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if len(arg) == 0:
        raise ArgumentError('You must specify a target. Use /ban <ipid> <ipid> ...')
    args = list(arg.split(' '))
    client.send_host_message('Attempting to ban {} IPIDs.'.format(len(args)))
    for raw_ipid in args:
        try:
            ipid = int(raw_ipid)
        except:
            raise ClientError('{} does not look like a valid IPID.'.format(raw_ipid))
        try:
            client.server.ban_manager.add_ban(ipid)
        except ServerError:
            raise
        if ipid != None:
            targets = client.server.client_manager.get_targets(client, TargetType.IPID, ipid, False)
            if targets:
                for c in targets:
                    c.disconnect()
                client.send_host_message('{} clients was kicked.'.format(len(targets)))
            client.send_host_message('{} was banned.'.format(ipid))
            logger.log_server('Banned {}.'.format(ipid), client)
            logger.log_mod('Banned {}.'.format(ipid), client)
        
def ooc_cmd_unban(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if len(arg) == 0:
        raise ArgumentError('You must specify a target. Use /unban <ipid> <ipid> ...')
    args = list(arg.split(' '))
    client.send_host_message('Attempting to unban {} IPIDs.'.format(len(args)))
    for raw_ipid in args:
        try:
            client.server.ban_manager.remove_ban(int(raw_ipid))
        except:
            raise ClientError('{} does not look like a valid IPID.'.format(raw_ipid))
        logger.log_server('Unbanned {}.'.format(raw_ipid), client)
        logger.log_mod('Unbanned {}.'.format(raw_ipid), client)
        client.send_host_message('Unbanned {}'.format(raw_ipid))
    
def ooc_cmd_play(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if len(arg) == 0:
        raise ArgumentError('You must specify a song.')
    client.area.play_music(arg, client.char_id, -1)
    client.area.add_music_playing(client, arg)
    logger.log_server('[{}][{}]Changed music to {}.'.format(client.area.abbreviation, client.get_char_name(), arg), client)
    
def ooc_cmd_mute(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if len(arg) == 0:
        raise ArgumentError('You must specify a target. Use /mute <ipid>.')
    args = list(arg.split(' '))
    client.send_host_message('Attempting to mute {} IPIDs.'.format(len(args)))
    for raw_ipid in args:
        if raw_ipid.isdigit():
            ipid = int(raw_ipid)
            clients = client.server.client_manager.get_targets(client, TargetType.IPID, ipid, False)
            if (clients):
                msg = 'Muted the IPID ' + str(ipid) + '\'s following clients:'
                for c in clients:
                    c.is_muted = True
                    logger.log_server('Muted {} [{}]({}).'.format(c.get_char_name(), c.id, c.ipid), client)
                    logger.log_mod('Muted {} [{}]({}).'.format(c.get_char_name(), c.id, c.ipid), client)
                    msg += ' ' + c.get_char_name() + ' [' + str(c.id) + '],'
                msg = msg[:-1]
                msg += '.'
                client.send_host_message('{}'.format(msg))
            else:
                client.send_host_message("No targets found. Use /mute <ipid> <ipid> ... for mute.")
        else:
            client.send_host_message('{} does not look like a valid IPID.'.format(raw_ipid))

def ooc_cmd_unmute(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if len(arg) == 0:
        raise ArgumentError('You must specify a target.')
    args = list(arg.split(' '))
    client.send_host_message('Attempting to unmute {} IPIDs.'.format(len(args)))
    for raw_ipid in args:
        if raw_ipid.isdigit():
            ipid = int(raw_ipid)
            clients = client.server.client_manager.get_targets(client, TargetType.IPID, ipid, False)
            if (clients):
                msg = 'Unmuted the IPID ' + str(ipid) + '\'s following clients::'
                for c in clients:
                    c.is_muted = False
                    logger.log_server('Unmuted {} [{}]({}).'.format(c.get_char_name(), c.id, c.ipid), client)
                    logger.log_mod('Unmuted {} [{}]({}).'.format(c.get_char_name(), c.id, c.ipid), client)
                    msg += ' ' + c.get_char_name() + ' [' + str(c.id) + '],'
                msg = msg[:-1]
                msg += '.'
                client.send_host_message('{}'.format(msg))
            else:
                client.send_host_message("No targets found. Use /unmute <ipid> <ipid> ... for unmute.")
        else:
            client.send_host_message('{} does not look like a valid IPID.'.format(raw_ipid))
    
def ooc_cmd_login(client, arg):
    if len(arg) == 0:
        raise ArgumentError('You must specify the password.')
    try:
        client.auth_mod(arg)
    except ClientError:
        raise
    if client.area.evidence_mod == 'HiddenCM':
        client.area.broadcast_evidence_list()
    client.send_host_message('Logged in as a moderator.')
    logger.log_server('Logged in as moderator.', client)
    logger.log_mod('Logged in as moderator.', client)
    
def ooc_cmd_g(client, arg):
    if client.muted_global:
        raise ClientError('Global chat toggled off.')
    if len(arg) == 0:
        raise ArgumentError("You can't send an empty message.")
    client.server.broadcast_global(client, arg)
    logger.log_server('[{}][{}][GLOBAL]{}.'.format(client.area.abbreviation, client.get_char_name(), arg), client)
    
def ooc_cmd_gm(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if client.muted_global:
        raise ClientError('You have the global chat muted.')
    if len(arg) == 0:
        raise ArgumentError("Can't send an empty message.")
    client.server.broadcast_global(client, arg, True)
    logger.log_server('[{}][{}][GLOBAL-MOD]{}.'.format(client.area.abbreviation, client.get_char_name(), arg), client)
    logger.log_mod('[{}][{}][GLOBAL-MOD]{}.'.format(client.area.abbreviation, client.get_char_name(), arg), client)
    
def ooc_cmd_m(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if len(arg) == 0:
        raise ArgumentError("You can't send an empty message.")
    client.server.send_modchat(client, arg)
    logger.log_server('[{}][{}][MODCHAT]{}.'.format(client.area.abbreviation, client.get_char_name(), arg), client)
    logger.log_mod('[{}][{}][MODCHAT]{}.'.format(client.area.abbreviation, client.get_char_name(), arg), client)
    
def ooc_cmd_lm(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if len(arg) == 0:
        raise ArgumentError("Can't send an empty message.")
    client.area.send_command('CT', '{}[MOD][{}]'
                             .format(client.server.config['hostname'], client.get_char_name()), arg)
    logger.log_server('[{}][{}][LOCAL-MOD]{}.'.format(client.area.abbreviation, client.get_char_name(), arg), client)
    logger.log_mod('[{}][{}][LOCAL-MOD]{}.'.format(client.area.abbreviation, client.get_char_name(), arg), client)
    
def ooc_cmd_announce(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if len(arg) == 0:
        raise ArgumentError("Can't send an empty message.")
    client.server.send_all_cmd_pred('CT', '{}'.format(client.server.config['hostname']),
                                    '=== Announcement ===\r\n{}\r\n=================='.format(arg), '1')
    logger.log_server('[{}][{}][ANNOUNCEMENT]{}.'.format(client.area.abbreviation, client.get_char_name(), arg), client)
    logger.log_mod('[{}][{}][ANNOUNCEMENT]{}.'.format(client.area.abbreviation, client.get_char_name(), arg), client)
    
def ooc_cmd_toggleglobal(client, arg):
    if len(arg) != 0:
        raise ArgumentError("This command doesn't take any arguments")
    client.muted_global = not client.muted_global
    glob_stat = 'on'
    if client.muted_global:
        glob_stat = 'off'
    client.send_host_message('Global chat turned {}.'.format(glob_stat))


def ooc_cmd_need(client, arg):
    if client.muted_adverts:
        raise ClientError('You have advertisements muted.')
    if len(arg) == 0:
        raise ArgumentError("You must specify what you need.")
    client.server.broadcast_need(client, arg)
    logger.log_server('[{}][{}][NEED]{}.'.format(client.area.abbreviation, client.get_char_name(), arg), client)
    
def ooc_cmd_toggleadverts(client, arg):
    if len(arg) != 0:
        raise ArgumentError("This command doesn't take any arguments")
    client.muted_adverts = not client.muted_adverts
    adv_stat = 'on'
    if client.muted_adverts:
        adv_stat = 'off'
    client.send_host_message('Advertisements turned {}.'.format(adv_stat))
    
def ooc_cmd_doc(client, arg):
    if len(arg) == 0:
        client.send_host_message('Document: {}'.format(client.area.doc))
        logger.log_server(
            '[{}][{}]Requested document. Link: {}'.format(client.area.abbreviation, client.get_char_name(), client.area.doc), client)
    else:
        client.area.change_doc(arg)
        client.area.send_host_message('{} changed the doc link.'.format(client.get_char_name()))
        logger.log_server('[{}][{}]Changed document to: {}'.format(client.area.abbreviation, client.get_char_name(), arg), client)


def ooc_cmd_cleardoc(client, arg):
    if len(arg) != 0:
        raise ArgumentError('This command has no arguments.')
    client.area.send_host_message('{} cleared the doc link.'.format(client.get_char_name()))
    logger.log_server('[{}][{}]Cleared document. Old link: {}'
                      .format(client.area.abbreviation, client.get_char_name(), client.area.doc), client)
    client.area.change_doc()


def ooc_cmd_status(client, arg):
    if len(arg) == 0:
        client.send_host_message('Current status: {}'.format(client.area.status))
    else:
        try:
            client.area.change_status(arg)
            client.area.send_host_message('{} changed status to {}.'.format(client.get_char_name(), client.area.status))
            logger.log_server(
                '[{}][{}]Changed status to {}'.format(client.area.abbreviation, client.get_char_name(), client.area.status), client)
        except AreaError:
            raise


def ooc_cmd_online(client, _):
    client.send_player_count()


def ooc_cmd_area(client, arg):
    args = arg.split()
    if len(args) == 0:
        client.send_area_list()
    elif len(args) == 1:
        try:
            area = client.server.area_manager.get_area_by_id(int(args[0]))
            client.change_area(area)
        except ValueError:
            raise ArgumentError('Area ID must be a number.')
        except (AreaError, ClientError):
            raise
    else:
        raise ArgumentError('Too many arguments. Use /area <id>.')
        
def ooc_cmd_pm(client, arg):
    args = arg.split()
    key = ''
    msg = None
    if len(args) < 2:
        raise ArgumentError('Not enough arguments. use /pm <target> <message>. Target should be ID, OOC-name or char-name. Use /getarea for getting info like "[ID] char-name".')
    targets = client.server.client_manager.get_targets(client, TargetType.CHAR_NAME, arg, True)
    key = TargetType.CHAR_NAME
    if len(targets) == 0 and args[0].isdigit():
        targets = client.server.client_manager.get_targets(client, TargetType.ID, int(args[0]), False)
        key = TargetType.ID
    if len(targets) == 0:
        targets = client.server.client_manager.get_targets(client, TargetType.OOC_NAME, arg, True)
        key = TargetType.OOC_NAME
    if len(targets) == 0:
        raise ArgumentError('No targets found.')
    try:
        if key == TargetType.ID:
            msg = ' '.join(args[1:])
        else:
            if key == TargetType.CHAR_NAME:
                msg = arg[len(targets[0].get_char_name()) + 1:]
            if key == TargetType.OOC_NAME:
                msg = arg[len(targets[0].name) + 1:]
    except:
        raise ArgumentError('Not enough arguments. Use /pm <target> <message>.')
    c = targets[0]
    if c.pm_mute:
        raise ClientError('This user muted all pm conversation')
    else:
        if c.is_mod:
            c.send_host_message('PM from {} (ID: {}, IPID: {}) in {} ({}): {}'.format(client.name, client.id, client.ipid, client.area.name, client.get_char_name(), msg))
        else:
            c.send_host_message('PM from {} (ID: {}) in {} ({}): {}'.format(client.name, client.id, client.area.name, client.get_char_name(), msg))
        client.send_host_message('PM sent to {}. Message: {}'.format(args[0], msg))
 
def ooc_cmd_mutepm(client, arg):
    if len(arg) != 0:
        raise ArgumentError("This command doesn't take any arguments")
    client.pm_mute = not client.pm_mute
    client.send_host_message({True:'You stopped receiving PMs', False:'You are now receiving PMs'}[client.pm_mute]) 

def ooc_cmd_charselect(client, arg):
    if not arg:
        client.char_select()
    else:
        if client.is_mod:
            try:
                client.server.client_manager.get_targets(client, TargetType.ID, int(arg), False)[0].char_select()
            except:
                raise ArgumentError('Wrong arguments. Use /charselect <target\'s id>')
                
def ooc_cmd_reload(client, arg):
    if len(arg) != 0:
        raise ArgumentError("This command doesn't take any arguments")
    try:
        client.reload_character()
    except ClientError:
        raise
    client.send_host_message('Character reloaded.') 
    
def ooc_cmd_randomchar(client, arg):
    if len(arg) != 0:
        raise ArgumentError('This command has no arguments.')
    if len(client.charcurse) > 0:
        free_id = random.choice(client.charcurse)
    else:
        try:
            free_id = client.area.get_rand_avail_char_id()
        except AreaError:
            raise
    try:
        client.change_character(free_id)
    except ClientError:
        raise
    client.send_host_message('Randomly switched to {}'.format(client.get_char_name()))
    
def ooc_cmd_getarea(client, arg):
    client.send_area_info(client.area.id, False)
    
def ooc_cmd_getareas(client, arg):
    client.send_area_info(-1, False)
    
def ooc_cmd_mods(client, arg):
    client.send_area_info(-1, True)  
    
def ooc_cmd_evi_swap(client, arg):
    args = list(arg.split(' '))
    if len(args) != 2:
        raise ClientError("you must specify 2 numbers")
    try:
        client.area.evi_list.evidence_swap(client, int(args[0]), int(args[1]))
        client.area.broadcast_evidence_list()
    except:
        raise ClientError("you must specify 2 numbers")
     
def ooc_cmd_cm(client, arg):
    if 'CM' not in client.area.evidence_mod:
        raise ClientError('You can\'t become a CM in this area')
    if client.area.owned == False:
        client.area.owned = True
        client.is_cm = True
        if client.area.evidence_mod == 'HiddenCM':
            client.area.broadcast_evidence_list()
        client.server.area_manager.send_arup_cms()
        client.area.send_host_message('{} is CM in this area now.'.format(client.get_char_name()))
     
def ooc_cmd_uncm(client, arg):
    if client.is_cm:
        client.is_cm = False
        client.area.owned = False
        client.area.blankposting_allowed = True
        if client.area.is_locked != client.area.Locked.FREE:
            client.area.unlock()
        client.server.area_manager.send_arup_cms()
        client.area.send_host_message('{} is no longer CM in this area.'.format(client.get_char_name()))
    else:
        raise ClientError('You cannot give up being the CM when you are not one')
    
def ooc_cmd_unmod(client, arg):
    client.is_mod = False
    if client.area.evidence_mod == 'HiddenCM':
        client.area.broadcast_evidence_list()
    client.send_host_message('you\'re not a mod now')
    
def ooc_cmd_area_lock(client, arg):
    if not client.area.locking_allowed:
        client.send_host_message('Area locking is disabled in this area.')
        return
    if client.area.is_locked == client.area.Locked.LOCKED:
        client.send_host_message('Area is already locked.')
    if client.is_cm:
        client.area.lock()
        return
    else:
        raise ClientError('Only CM can lock the area.')
    
def ooc_cmd_area_spectate(client, arg):
    if not client.area.locking_allowed:
        client.send_host_message('Area locking is disabled in this area.')
        return
    if client.area.is_locked == client.area.Locked.SPECTATABLE:
        client.send_host_message('Area is already spectatable.')
    if client.is_cm:
        client.area.spectator()
        return
    else:
        raise ClientError('Only CM can make the area spectatable.')
        
def ooc_cmd_area_unlock(client, arg):
    if client.area.is_locked == client.area.Locked.FREE:
        raise ClientError('Area is already unlocked.')
    if not client.is_cm:
        raise ClientError('Only CM can unlock area.')
    client.area.unlock()
    client.send_host_message('Area is unlocked.')
        
def ooc_cmd_invite(client, arg):
    if not arg:
        raise ClientError('You must specify a target. Use /invite <id>')
    if not client.area.is_locked:
        raise ClientError('Area isn\'t locked.')
    if not client.is_cm and not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    try:
        c = client.server.client_manager.get_targets(client, TargetType.ID, int(arg), False)[0]
        client.area.invite_list[c.id] = None
        client.send_host_message('{} is invited to your area.'.format(c.get_char_name()))
        c.send_host_message('You were invited and given access to {}.'.format(client.area.name))
    except:
        raise ClientError('You must specify a target. Use /invite <id>')

def ooc_cmd_uninvite(client, arg):
    if not client.is_cm and not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if not client.area.is_locked and not client.is_mod:
        raise ClientError('Area isn\'t locked.')
    if not arg:
        raise ClientError('You must specify a target. Use /uninvite <id>')
    arg = arg.split(' ')
    targets = client.server.client_manager.get_targets(client, TargetType.ID, int(arg[0]), True)
    if targets:
        try:
            for c in targets:
                client.send_host_message("You have removed {} from the whitelist.".format(c.get_char_name()))
                c.send_host_message("You were removed from the area whitelist.")
                if client.area.is_locked:
                    client.area.invite_list.pop(c.id)
        except AreaError:
            raise
        except ClientError:
            raise
    else:
        client.send_host_message("No targets found.")

def ooc_cmd_area_kick(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if not client.area.is_locked and not client.is_mod:
        raise ClientError('Area isn\'t locked.')
    if not arg:
        raise ClientError('You must specify a target. Use /area_kick <id> [destination #]')
    arg = arg.split(' ')
    targets = client.server.client_manager.get_targets(client, TargetType.ID, int(arg[0]), False)
    if targets:
        try:
            for c in targets:
                if len(arg) == 1:
                    area = client.server.area_manager.get_area_by_id(int(0))
                    output = 0
                else:
                    try:
                        area = client.server.area_manager.get_area_by_id(int(arg[1]))
                        output = arg[1]
                    except AreaError:
                        raise
                client.send_host_message("Attempting to kick {} to area {}.".format(c.get_char_name(), output))
                c.change_area(area)
                c.send_host_message("You were kicked from the area to area {}.".format(output))
                if client.area.is_locked:
                    client.area.invite_list.pop(c.id)
        except AreaError:
            raise
        except ClientError:
            raise
    else:
        client.send_host_message("No targets found.")

    
def ooc_cmd_ooc_mute(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if len(arg) == 0:
        raise ArgumentError('You must specify a target. Use /ooc_mute <OOC-name>.')
    targets = client.server.client_manager.get_targets(client, TargetType.OOC_NAME, arg, False)
    if not targets:
        raise ArgumentError('Targets not found. Use /ooc_mute <OOC-name>.')
    for target in targets:
        target.is_ooc_muted = True
    client.send_host_message('Muted {} existing client(s).'.format(len(targets)))

def ooc_cmd_ooc_unmute(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if len(arg) == 0:
        raise ArgumentError('You must specify a target. Use /ooc_unmute <OOC-name>.')
    targets = client.server.client_manager.get_ooc_muted_clients()
    if not targets:
        raise ArgumentError('Targets not found. Use /ooc_unmute <OOC-name>.')
    for target in targets:
        target.is_ooc_muted = False
    client.send_host_message('Unmuted {} existing client(s).'.format(len(targets)))

def ooc_cmd_disemvowel(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    elif len(arg) == 0:
        raise ArgumentError('You must specify a target.')
    try:
        targets = client.server.client_manager.get_targets(client, TargetType.ID, int(arg), False)
    except:
        raise ArgumentError('You must specify a target. Use /disemvowel <id>.')
    if targets:
        for c in targets:
            logger.log_server('Disemvowelling {}.'.format(c.get_ip()), client)
            logger.log_mod('Disemvowelling {}.'.format(c.get_ip()), client)
            c.disemvowel = True
        client.send_host_message('Disemvowelled {} existing client(s).'.format(len(targets)))
    else:
        client.send_host_message('No targets found.')

def ooc_cmd_undisemvowel(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    elif len(arg) == 0:
        raise ArgumentError('You must specify a target.')
    try:
        targets = client.server.client_manager.get_targets(client, TargetType.ID, int(arg), False)
    except:
        raise ArgumentError('You must specify a target. Use /undisemvowel <id>.')
    if targets:
        for c in targets:
            logger.log_server('Undisemvowelling {}.'.format(c.get_ip()), client)
            logger.log_mod('Undisemvowelling {}.'.format(c.get_ip()), client)
            c.disemvowel = False
        client.send_host_message('Undisemvowelled {} existing client(s).'.format(len(targets)))
    else:
        client.send_host_message('No targets found.')

def ooc_cmd_shake(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    elif len(arg) == 0:
        raise ArgumentError('You must specify a target.')
    try:
        targets = client.server.client_manager.get_targets(client, TargetType.ID, int(arg), False)
    except:
        raise ArgumentError('You must specify a target. Use /shake <id>.')
    if targets:
        for c in targets:
            logger.log_server('Shaking {}.'.format(c.get_ip()), client)
            logger.log_mod('Shaking {}.'.format(c.get_ip()), client)
            c.shaken = True
        client.send_host_message('Shook {} existing client(s).'.format(len(targets)))
    else:
        client.send_host_message('No targets found.')

def ooc_cmd_unshake(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    elif len(arg) == 0:
        raise ArgumentError('You must specify a target.')
    try:
        targets = client.server.client_manager.get_targets(client, TargetType.ID, int(arg), False)
    except:
        raise ArgumentError('You must specify a target. Use /unshake <id>.')
    if targets:
        for c in targets:
            logger.log_server('Unshaking {}.'.format(c.get_ip()), client)
            logger.log_mod('Unshaking {}.'.format(c.get_ip()), client)
            c.shaken = False
        client.send_host_message('Unshook {} existing client(s).'.format(len(targets)))
    else:
        client.send_host_message('No targets found.')

def ooc_cmd_charcurse(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    elif len(arg) == 0:
        raise ArgumentError('You must specify a target (an ID) and at least one character ID. Consult /charids for the character IDs.')
    elif len(arg) == 1:
        raise ArgumentError('You must specific at least one character ID. Consult /charids for the character IDs.')
    args = arg.split()
    try:
        targets = client.server.client_manager.get_targets(client, TargetType.ID, int(args[0]), False)
    except:
        raise ArgumentError('You must specify a valid target! Make sure it is a valid ID.')
    if targets:
        for c in targets:
            log_msg = ' ' + str(c.get_ip()) + ' to'
            part_msg = ' [' + str(c.id) + '] to'
            for raw_cid in args[1:]:
                try:
                    cid = int(raw_cid)
                    c.charcurse.append(cid)
                    part_msg += ' ' + str(client.server.char_list[cid]) + ','
                    log_msg += ' ' + str(client.server.char_list[cid]) + ','
                except:
                    ArgumentError('' + str(raw_cid) + ' does not look like a valid character ID.')
            part_msg = part_msg[:-1]
            part_msg += '.'
            log_msg = log_msg[:-1]
            log_msg += '.'
            c.char_select()
            logger.log_server('Charcursing' + log_msg, client)
            logger.log_mod('Charcursing' + log_msg, client)
            client.send_host_message('Charcursed' + part_msg)
    else:
        client.send_host_message('No targets found.')

def ooc_cmd_uncharcurse(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    elif len(arg) == 0:
        raise ArgumentError('You must specify a target (an ID).')
    args = arg.split()
    try:
        targets = client.server.client_manager.get_targets(client, TargetType.ID, int(args[0]), False)
    except:
        raise ArgumentError('You must specify a valid target! Make sure it is a valid ID.')
    if targets:
        for c in targets:
            if len(c.charcurse) > 0:
                c.charcurse = []
                logger.log_server('Uncharcursing {}.'.format(c.get_ip()), client)
                logger.log_mod('Uncharcursing {}.'.format(c.get_ip()), client)
                client.send_host_message('Uncharcursed [{}].'.format(c.id))
                c.char_select()
            else:
                client.send_host_message('[{}] is not charcursed.'.format(c.id))
    else:
        client.send_host_message('No targets found.')

def ooc_cmd_charids(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if len(arg) != 0:
        raise ArgumentError("This command doesn't take any arguments")
    msg = 'Here is a list of all available characters on the server:'
    for c in range(0, len(client.server.char_list)):
        msg += '\n[' + str(c) + '] ' + client.server.char_list[c]
    client.send_host_message(msg)

def ooc_cmd_blockdj(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if len(arg) == 0:
        raise ArgumentError('You must specify a target. Use /blockdj <id>.')
    try:
        targets = client.server.client_manager.get_targets(client, TargetType.ID, int(arg), False)
    except:
         raise ArgumentError('You must enter a number. Use /blockdj <id>.')
    if not targets:
        raise ArgumentError('Target not found. Use /blockdj <id>.')
    for target in targets:
        target.is_dj = False
        target.send_host_message('A moderator muted you from changing the music.')
        logger.log_server('BlockDJ\'d {} [{}]({}).'.format(target.get_char_name(), target.id, target.get_ip()), client)
        logger.log_mod('BlockDJ\'d {} [{}]({}).'.format(target.get_char_name(), target.id, target.get_ip()), client)
        target.area.remove_jukebox_vote(target, True)
    client.send_host_message('blockdj\'d {}.'.format(targets[0].get_char_name()))

def ooc_cmd_unblockdj(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if len(arg) == 0:
        raise ArgumentError('You must specify a target. Use /unblockdj <id>.')
    try:
        targets = client.server.client_manager.get_targets(client, TargetType.ID, int(arg), False)
    except:
         raise ArgumentError('You must enter a number. Use /unblockdj <id>.')
    if not targets:
        raise ArgumentError('Target not found. Use /blockdj <id>.')
    for target in targets:
        target.is_dj = True
        target.send_host_message('A moderator unmuted you from changing the music.')
        logger.log_server('UnblockDJ\'d {} [{}]({}).'.format(target.get_char_name(), target.id, target.get_ip()), client)
        logger.log_mod('UnblockDJ\'d {} [{}]({}).'.format(target.get_char_name(), target.id, target.get_ip()), client)
    client.send_host_message('Unblockdj\'d {}.'.format(targets[0].get_char_name()))

def ooc_cmd_blockwtce(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if len(arg) == 0:
        raise ArgumentError('You must specify a target. Use /blockwtce <id>.')
    try:
        targets = client.server.client_manager.get_targets(client, TargetType.ID, int(arg), False)
    except:
        raise ArgumentError('You must enter a number. Use /blockwtce <id>.')
    if not targets:
        raise ArgumentError('Target not found. Use /blockwtce <id>.')
    for target in targets:
        target.can_wtce = False
        target.send_host_message('A moderator blocked you from using judge signs.')
        logger.log_server('BlockWTCE\'d {} [{}]({}).'.format(target.get_char_name(), target.id, target.get_ip()), client)
        logger.log_mod('BlockWTCE\'d {} [{}]({}).'.format(target.get_char_name(), target.id, target.get_ip()), client)
    client.send_host_message('blockwtce\'d {}.'.format(targets[0].get_char_name()))

def ooc_cmd_unblockwtce(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if len(arg) == 0:
        raise ArgumentError('You must specify a target. Use /unblockwtce <id>.')
    try:
        targets = client.server.client_manager.get_targets(client, TargetType.ID, int(arg), False)
    except:
        raise ArgumentError('You must enter a number. Use /unblockwtce <id>.')
    if not targets:
        raise ArgumentError('Target not found. Use /unblockwtce <id>.')
    for target in targets:
        target.can_wtce = True
        target.send_host_message('A moderator unblocked you from using judge signs.')
        logger.log_server('UnblockWTCE\'d {} [{}]({}).'.format(target.get_char_name(), target.id, target.get_ip()), client)
        logger.log_mod('UnblockWTCE\'d {} [{}]({}).'.format(target.get_char_name(), target.id, target.get_ip()), client)
    client.send_host_message('unblockwtce\'d {}.'.format(targets[0].get_char_name()))

def ooc_cmd_notecard(client, arg):
    if len(arg) == 0:
        raise ArgumentError('You must specify the contents of the note card.')
    client.area.cards[client.get_char_name()] = arg
    client.area.send_host_message('{} wrote a note card.'.format(client.get_char_name()))

def ooc_cmd_notecard_clear(client, arg):
    try:
        del client.area.cards[client.get_char_name()]
        client.area.send_host_message('{} erased their note card.'.format(client.get_char_name()))
    except KeyError:
        raise ClientError('You do not have a note card.')

def ooc_cmd_notecard_reveal(client, arg):
    if not client.is_cm and not client.is_mod:
        raise ClientError('You must be a CM or moderator to reveal cards.')
    if len(client.area.cards) == 0:
        raise ClientError('There are no cards to reveal in this area.')
    msg = 'Note cards have been revealed.\n'
    for card_owner, card_msg in client.area.cards.items():
        msg += '{}: {}\n'.format(card_owner, card_msg)
    client.area.cards.clear()
    client.area.send_host_message(msg)

def ooc_cmd_rolla_reload(client, arg):
    if not client.is_mod:
        raise ClientError('You must be a moderator to load the ability dice configuration.')
    rolla_reload(client.area)
    client.send_host_message('Reloaded ability dice configuration.')

def rolla_reload(area):
    try:
        import yaml
        with open('config/dice.yaml', 'r') as dice:
            area.ability_dice = yaml.load(dice)
    except:
        raise ServerError('There was an error parsing the ability dice configuration. Check your syntax.')

def ooc_cmd_rolla_set(client, arg):
    if not hasattr(client.area, 'ability_dice'):
        rolla_reload(client.area)
    available_sets = ', '.join(client.area.ability_dice.keys())
    if len(arg) == 0:
        raise ArgumentError('You must specify the ability set name.\nAvailable sets: {}'.format(available_sets))
    if arg in client.area.ability_dice:
        client.ability_dice_set = arg
        client.send_host_message("Set ability set to {}.".format(arg))
    else:
        raise ArgumentError('Invalid ability set \'{}\'.\nAvailable sets: {}'.format(arg, available_sets))

def ooc_cmd_rolla(client, arg):
    if not hasattr(client.area, 'ability_dice'):
        rolla_reload(client.area)
    if not hasattr(client, 'ability_dice_set'):
        raise ClientError('You must set your ability set using /rolla_set <name>.')
    ability_dice = client.area.ability_dice[client.ability_dice_set]
    max_roll = ability_dice['max'] if 'max' in ability_dice else 6
    roll = random.randint(1, max_roll)
    ability = ability_dice[roll] if roll in ability_dice else "Nothing happens"
    client.area.send_host_message(
        '{} rolled a {} (out of {}): {}.'.format(client.get_char_name(), roll, max_roll, ability))
		
def ooc_cmd_refresh(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if len (arg) > 0:
        raise ClientError('This command does not take in any arguments!')
    else:
        try:
            client.server.refresh()
            client.send_host_message('You have reloaded the server.')
        except ServerError:
            raise

def ooc_cmd_judgelog(client, arg):
    if not client.is_mod:
        raise ClientError('You must be authorized to do that.')
    if len(arg) != 0:
        raise ArgumentError('This command does not take any arguments.')
    jlog = client.area.judgelog
    if len(jlog) > 0:
        jlog_msg = '== Judge Log =='
        for x in jlog:
            jlog_msg += '\r\n{}'.format(x)
        client.send_host_message(jlog_msg)
    else:
        raise ServerError('There have been no judge actions in this area since start of session.')
