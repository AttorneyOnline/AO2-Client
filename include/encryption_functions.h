#ifndef ENCRYPTION_FUNCTIONS_H
#define ENCRYPTION_FUNCTIONS_H

#include <QString>

#include <QVector>
#include <cstddef>
#include <iomanip>
#include <sstream>
#include <stdlib.h>

QString fanta_encrypt(QString p_input, unsigned int key);
QString fanta_decrypt(QString p_input, unsigned int key);

#endif // ENCRYPTION_FUNCTIONS_H
