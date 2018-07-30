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

from server import logger


class DistrictClient:
    def __init__(self, server):
        self.server = server
        self.reader = None
        self.writer = None
        self.message_queue = []

    async def connect(self):
        loop = asyncio.get_event_loop()
        while True:
            try:
                self.reader, self.writer = await asyncio.open_connection(self.server.config['district_ip'],
                                                                         self.server.config['district_port'], loop=loop)
                await self.handle_connection()
            except (ConnectionRefusedError, TimeoutError):
                pass
            except (ConnectionResetError, asyncio.IncompleteReadError):
                self.writer = None
                self.reader = None
            finally:
                logger.log_debug("Couldn't connect to the district, retrying in 30 seconds.")
                await asyncio.sleep(30)

    async def handle_connection(self):
        logger.log_debug('District connected.')
        self.send_raw_message('AUTH#{}'.format(self.server.config['district_password']))
        while True:
            data = await self.reader.readuntil(b'\r\n')
            if not data:
                return
            raw_msg = data.decode()[:-2]
            logger.log_debug('[DISTRICT][INC][RAW]{}'.format(raw_msg))
            cmd, *args = raw_msg.split('#')
            if cmd == 'GLOBAL':
                glob_name = '{}[{}:{}][{}]'.format('<dollar>G', args[1], args[2], args[3])
                if args[0] == '1':
                    glob_name += '[M]'
                self.server.send_all_cmd_pred('CT', glob_name, args[4], pred=lambda x: not x.muted_global)
            elif cmd == 'NEED':
                need_msg = '=== Cross Advert ===\r\n{} at {} in {} [{}] needs {}\r\n====================' \
                    .format(args[1], args[0], args[2], args[3], args[4])
                self.server.send_all_cmd_pred('CT', '{}'.format(self.server.config['hostname']), need_msg,
                                              pred=lambda x: not x.muted_adverts)

    async def write_queue(self):
        while self.message_queue:
            msg = self.message_queue.pop(0)
            try:
                self.writer.write(msg)
                await self.writer.drain()
            except ConnectionResetError:
                return

    def send_raw_message(self, msg):
        if not self.writer:
            return
        self.message_queue.append('{}\r\n'.format(msg).encode())
        asyncio.ensure_future(self.write_queue(), loop=asyncio.get_event_loop())
