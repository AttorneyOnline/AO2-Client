#ifndef COURTROOM_H
#define COURTROOM_H

#include <QMainWindow>

class AOApplication;

class Courtroom : public QMainWindow
{
    Q_OBJECT
public:
    explicit Courtroom(AOApplication *parent = 0);

signals:

public slots:
};

#endif // COURTROOM_H
