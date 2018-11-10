from enum import Enum

class TargetType(Enum):
        #possible keys: ip, OOC, id, cname, ipid, hdid
        IP = 0
        OOC_NAME = 1
        ID = 2
        CHAR_NAME = 3
        IPID = 4
        HDID = 5
        ALL = 6