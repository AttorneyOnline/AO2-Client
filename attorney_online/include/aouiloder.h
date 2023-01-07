#ifndef AOUILOADER_H
#define AOUILOADER_H

#include "aoapplication.h"

#include <QObject>
#include <QFile>
#include <QUiLoader>

#define FROM_UI(type, name) ui_##name = findChild<type *>(#name);

class AOUiLoader : public QUiLoader
{
  Q_OBJECT
public:
  AOUiLoader(QObject *parent, AOApplication *ao_app)
    : QUiLoader(parent), ao_app(ao_app) { }

  virtual QWidget *createWidget(const QString &className, QWidget *parent = nullptr,
                                const QString &name = QString());
private:
  AOApplication *ao_app;
};

#endif // AOUILOADER_H
