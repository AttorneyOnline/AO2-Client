#pragma once

#include "aoapplication.h"

#include <QString>

void call_error(QString message);
void call_notice(QString message);

class debug_functions : public QObject
{
  Q_OBJECT

public:
  explicit debug_functions(AOApplication *parent);
  AOApplication *ao_app;

  void call_notice_reconnect(QString message);
};
