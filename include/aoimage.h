// This class represents a static theme-dependent image

#ifndef AOIMAGE_H
#define AOIMAGE_H

#include "aoapplication.h"

#include <QDebug>
#include <QLabel>
#include <QMovie>

class AOImage : public QLabel {
public:
  AOImage(QWidget *parent, AOApplication *p_ao_app, bool make_static = false);
  ~AOImage();

  QWidget* const m_parent;
  AOApplication* const ao_app;
  QMovie *movie;

  QString path;

  const bool is_static;

  bool masked = false;

  bool set_image(QString p_image, QString p_misc = "");
  void set_size_and_pos(QString identifier);
};

#endif // AOIMAGE_H
