// This class represents a static theme-dependent image

#pragma once

#include "aoapplication.h"

#include <QDebug>
#include <QLabel>
#include <QMovie>

class AOImage : public QLabel
{
  Q_OBJECT

public:
  AOImage(AOApplication *ao_app, QWidget *parent = nullptr);

  QString image();
  bool setImage(QString fileName, QString miscellaneous);
  bool setImage(QString fileName);

private:
  AOApplication *ao_app;

  QString m_file_name;
};
