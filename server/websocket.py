# tsuserver3, an Attorney Online server
#
# Copyright (C) 2017 argoneus <argoneuscze@gmail.com>
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

# Partly authored by Johan Hanssen Seferidis (MIT license):
# https://github.com/Pithikos/python-websocket-server

import asyncio
import re
import struct
from base64 import b64encode
from hashlib import sha1

from server import logger


class Bitmasks:
    FIN = 0x80
    OPCODE = 0x0f
    MASKED = 0x80
    PAYLOAD_LEN = 0x7f
    PAYLOAD_LEN_EXT16 = 0x7e
    PAYLOAD_LEN_EXT64 = 0x7f


class Opcode:
    CONTINUATION = 0x0
    TEXT = 0x1
    BINARY = 0x2
    CLOSE_CONN = 0x8
    PING = 0x9
    PONG = 0xA


class WebSocket:
    """
    State data for clients that are connected via a WebSocket that wraps
    over a conventional TCP connection.
    """

    def __init__(self, client, protocol):
        self.client = client
        self.transport = client.transport
        self.protocol = protocol
        self.keep_alive = True
        self.handshake_done = False
        self.valid = False

    def handle(self, data):
        if not self.handshake_done:
            return self.handshake(data)
        return self.parse(data)

    def parse(self, data):
        b1, b2 = 0, 0
        if len(data) >= 2:
            b1, b2 = data[0], data[1]

        fin    = b1 & Bitmasks.FIN
        opcode = b1 & Bitmasks.OPCODE
        masked = b2 & Bitmasks.MASKED
        payload_length = b2 & Bitmasks.PAYLOAD_LEN

        if not b1:
            # Connection closed
            self.keep_alive = 0
            return
        if opcode == Opcode.CLOSE_CONN:
            # Connection close requested
            self.keep_alive = 0
            return
        if not masked:
            # Client was not masked (spec violation)
            logger.log_debug("ws: client was not masked.", self.client)
            self.keep_alive = 0
            print(data)
            return
        if opcode == Opcode.CONTINUATION:
            # No continuation frames supported
            logger.log_debug("ws: client tried to send continuation frame.", self.client)
            return
        elif opcode == Opcode.BINARY:
            # No binary frames supported
            logger.log_debug("ws: client tried to send binary frame.", self.client)
            return
        elif opcode == Opcode.TEXT:
            def opcode_handler(s, msg):
                return msg
        elif opcode == Opcode.PING:
            opcode_handler = self.send_pong
        elif opcode == Opcode.PONG:
            opcode_handler = lambda s, msg: None
        else:
            # Unknown opcode
            logger.log_debug("ws: unknown opcode!", self.client)
            self.keep_alive = 0
            return

        mask_offset = 2
        if payload_length == 126:
            payload_length = struct.unpack(">H", data[2:4])[0]
            mask_offset = 4
        elif payload_length == 127:
            payload_length = struct.unpack(">Q", data[2:10])[0]
            mask_offset = 10

        masks = data[mask_offset:mask_offset + 4]
        decoded = ""
        for char in data[mask_offset + 4:payload_length + mask_offset + 4]:
            char ^= masks[len(decoded) % 4]
            decoded += chr(char)

        return opcode_handler(self, decoded)

    def send_message(self, message):
        self.send_text(message)

    def send_pong(self, message):
        self.send_text(message, Opcode.PONG)

    def send_text(self, message, opcode=Opcode.TEXT):
        """
        Important: Fragmented (continuation) messages are not supported since
        their usage cases are limited - when we don't know the payload length.
        """

        # Validate message
        if isinstance(message, bytes):
            message = message.decode("utf-8")
        elif isinstance(message, str):
            pass
        else:
            raise TypeError("Message must be either str or bytes")

        header  = bytearray()
        payload = message.encode("utf-8")
        payload_length = len(payload)

        # Normal payload
        if payload_length <= 125:
            header.append(Bitmasks.FIN | opcode)
            header.append(payload_length)

        # Extended payload
        elif payload_length >= 126 and payload_length <= 65535:
            header.append(Bitmasks.FIN | opcode)
            header.append(Bitmasks.PAYLOAD_LEN_EXT16)
            header.extend(struct.pack(">H", payload_length))

        # Huge extended payload
        elif payload_length < (1 << 64):
            header.append(Bitmasks.FIN | opcode)
            header.append(Bitmasks.PAYLOAD_LEN_EXT64)
            header.extend(struct.pack(">Q", payload_length))

        else:
            raise Exception("Message is too big")

        self.transport.write(header + payload)

    def handshake(self, data):
        try:
            message = data[0:1024].decode().strip()
        except UnicodeDecodeError:
            return False

        upgrade = re.search('\nupgrade[\s]*:[\s]*websocket', message.lower())
        if not upgrade:
            self.keep_alive = False
            return False

        key = re.search('\n[sS]ec-[wW]eb[sS]ocket-[kK]ey[\s]*:[\s]*(.*)\r\n', message)
        if key:
            key = key.group(1)
        else:
            logger.log_debug("Client tried to connect but was missing a key", self.client)
            self.keep_alive = False
            return False

        response = self.make_handshake_response(key)
        print(response.encode())
        self.transport.write(response.encode())
        self.handshake_done = True
        self.valid = True
        return True

    def make_handshake_response(self, key):
        return \
          'HTTP/1.1 101 Switching Protocols\r\n'\
          'Upgrade: websocket\r\n'              \
          'Connection: Upgrade\r\n'             \
          'Sec-WebSocket-Accept: %s\r\n'        \
          '\r\n' % self.calculate_response_key(key)

    def calculate_response_key(self, key):
        GUID = '258EAFA5-E914-47DA-95CA-C5AB0DC85B11'
        hash = sha1(key.encode() + GUID.encode())
        response_key = b64encode(hash.digest()).strip()
        return response_key.decode('ASCII')

    def finish(self):
        self.protocol.connection_lost(self)
