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
import time
from server import logger


class MasterServerClient:
    def __init__(self, server):
        self.server = server
        self.reader = None
        self.writer = None

    async def connect(self):
        loop = asyncio.get_event_loop()
        while True:
            try:
                self.reader, self.writer = await asyncio.open_connection(self.server.config['masterserver_ip'],
                                                                         self.server.config['masterserver_port'],
                                                                         loop=loop)
                await self.handle_connection()
            except (ConnectionRefusedError, TimeoutError):
                pass
            except (ConnectionResetError, asyncio.IncompleteReadError):
                self.writer = None
                self.reader = None
            finally:
                logger.log_debug("Couldn't connect to the master server, retrying in 30 seconds.")
                print("Couldn't connect to the master server, retrying in 30 seconds.")
                await asyncio.sleep(30)

    async def handle_connection(self):
        logger.log_debug('Master server connected.')
        await self.send_server_info()
        fl = False
        lastping = time.time() - 20
        while True:
            self.reader.feed_data(b'END')
            full_data = await self.reader.readuntil(b'END')
            full_data = full_data[:-3]
            if len(full_data) > 0:
                data_list = list(full_data.split(b'#%'))[:-1]
                for data in data_list:
                    raw_msg = data.decode()
                    cmd, *args = raw_msg.split('#')
                    if cmd != 'CHECK' and cmd != 'PONG':
                        logger.log_debug('[MASTERSERVER][INC][RAW]{}'.format(raw_msg))
                    elif cmd == 'CHECK':
                        await self.send_raw_message('PING#%')
                    elif cmd == 'PONG':
                        fl = False
                    elif cmd == 'NOSERV':
                        await self.send_server_info()
            if time.time() - lastping > 5:
                if fl:
                    return
                lastping = time.time()
                fl = True
                await self.send_raw_message('PING#%')
            await asyncio.sleep(1)

    async def send_server_info(self):
        cfg = self.server.config
        msg = 'SCC#{}#{}#{}#{}#%'.format(cfg['port'], cfg['masterserver_name'], cfg['masterserver_description'],
                                         self.server.software)
        await self.send_raw_message(msg)

    async def send_raw_message(self, msg):
        try:
            self.writer.write(msg.encode())
            await self.writer.drain()
        except ConnectionResetError:
            return
