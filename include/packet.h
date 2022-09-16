#ifndef PACKET_H
#define PACKET_H

#include "aoapplication.h"
#include "aopacket.h"

#include "QString"

class AOApplication;

class Packet {
public:
    const QString header;
    bool logToDemo;
    virtual void handler(AOApplication*, AOPacket*){}
};

class Decryptor : public Packet {
public:
    const QString header = "decryptor";
    bool logToDemo = true;
    void handler(AOApplication* app, AOPacket* packet);
};

class MS : public Packet {
public:
    const QString header = "MS";
    bool logToDemo = true;
    void handler(AOApplication* app, AOPacket* packet);

};

class CharsCheck : public Packet {
public:
    const QString header = "CharsCheck";
    bool logToDemo = true;
    void handler(AOApplication* app, AOPacket* packet);
};

class ID : public Packet {
public:
    const QString header = "ID";
    bool logToDemo = true;
    void handler(AOApplication* app, AOPacket* packet);
};

class CT : public Packet {
public:
    const QString header = "CT";
    bool logToDemo = true;
    void handler(AOApplication *, AOPacket *);
};
class FL : public Packet {
public:
    const QString header = "FL";
    bool logToDemo = true;
    void handler(AOApplication *, AOPacket *);
};
class PN : public Packet {
public:
    const QString header = "PN";
    bool logToDemo = true;
    void handler(AOApplication *, AOPacket *);
};

class SI : public Packet {
public:
    const QString header = "SI";
    bool logToDemo = true;
    void handler(AOApplication *, AOPacket *);
};

class SC : public Packet {
public:
    const QString header = "SC";
    bool logToDemo = true;
    void handler(AOApplication *, AOPacket *);
};

class SM : public Packet {
public:
    const QString header = "SM";
    bool logToDemo = true;
    void handler(AOApplication *, AOPacket *);
};

class FM : public Packet {
public:
    const QString header = "FM";
    bool logToDemo = true;
    void handler(AOApplication *, AOPacket *);
};

class FA : public Packet {
public:
    const QString header = "FA";
    bool logToDemo = true;
    void handler(AOApplication *, AOPacket *);
};

class DONE : public Packet {
public:
    const QString header = "DONE";
    bool logToDemo = true;
    void handler(AOApplication *, AOPacket *);
};

#endif // PACKET_H
