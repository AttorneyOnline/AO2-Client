#ifndef AOUTILS_H
#define AOUTILS_H

#include <QFile>

class AOUtils {

public :

    static void migrateEffects(QFile* p_file);

private :
    AOUtils(){};
};

#endif //AOUTILS_H
