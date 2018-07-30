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

import logging

import time


def setup_logger(debug):
    logging.Formatter.converter = time.gmtime
    debug_formatter = logging.Formatter('[%(asctime)s UTC]%(message)s')
    srv_formatter = logging.Formatter('[%(asctime)s UTC]%(message)s')

    debug_log = logging.getLogger('debug')
    debug_log.setLevel(logging.DEBUG)

    debug_handler = logging.FileHandler('logs/debug.log', encoding='utf-8')
    debug_handler.setLevel(logging.DEBUG)
    debug_handler.setFormatter(debug_formatter)
    debug_log.addHandler(debug_handler)

    if not debug:
        debug_log.disabled = True

    server_log = logging.getLogger('server')
    server_log.setLevel(logging.INFO)

    server_handler = logging.FileHandler('logs/server.log', encoding='utf-8')
    server_handler.setLevel(logging.INFO)
    server_handler.setFormatter(srv_formatter)
    server_log.addHandler(server_handler)


def log_debug(msg, client=None):
    msg = parse_client_info(client) + msg
    logging.getLogger('debug').debug(msg)


def log_server(msg, client=None):
    msg = parse_client_info(client) + msg
    logging.getLogger('server').info(msg)


def parse_client_info(client):
    if client is None:
        return ''
    info = client.get_ip()
    if client.is_mod:
        return '[{:<15}][{}][MOD]'.format(info, client.id)
    return '[{:<15}][{}]'.format(info, client.id)
