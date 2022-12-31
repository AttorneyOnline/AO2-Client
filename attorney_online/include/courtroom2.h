#ifndef COURTROOM2_H
#define COURTROOM2_H

#include <QMainWindow>

#include "network/client.h"

using namespace AttorneyOnline;

class Courtroom2 : public QMainWindow {
    Q_OBJECT
public:
    Courtroom2(std::shared_ptr<Client> client);
    ~Courtroom2();

private:

    std::shared_ptr<Client> client;
};

#endif
