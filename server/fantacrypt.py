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

# fantacrypt was a mistake, just hardcoding some numbers is good enough

import binascii

CRYPT_CONST_1 = 53761
CRYPT_CONST_2 = 32618
CRYPT_KEY = 5


def fanta_decrypt(data):
    data_bytes = [int(data[x:x + 2], 16) for x in range(0, len(data), 2)]
    key = CRYPT_KEY
    ret = ''
    for byte in data_bytes:
        val = byte ^ ((key & 0xffff) >> 8)
        ret += chr(val)
        key = ((byte + key) * CRYPT_CONST_1) + CRYPT_CONST_2
    return ret


def fanta_encrypt(data):
    key = CRYPT_KEY
    ret = ''
    for char in data:
        val = ord(char) ^ ((key & 0xffff) >> 8)
        ret += binascii.hexlify(val.to_bytes(1, byteorder='big')).decode().upper()
        key = ((val + key) * CRYPT_CONST_1) + CRYPT_CONST_2
    return ret
