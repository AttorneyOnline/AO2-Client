#ifndef ENCRYPTION_FUNCTIONS_H
#define ENCRYPTION_FUNCTIONS_H

#include <QString>

#include <cstddef>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <QVector>

QString fanta_encrypt(QString p_input, unsigned int key);
QString fanta_decrypt(QString p_input, unsigned int key);

#endif // ENCRYPTION_FUNCTIONS_H
