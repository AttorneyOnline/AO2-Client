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
  AOImage(AOApplication *p_ao_app, QWidget *parent = nullptr);
  AOImage(AOApplication *p_ao_app, bool make_static, QWidget *parent = nullptr);
  ~AOImage();

  QString file_name();
  bool set_image(QString p_image, QString p_misc = QString());

private:
  AOApplication *ao_app;

  QString m_file_name;
};
