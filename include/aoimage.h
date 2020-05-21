// This class represents a static theme-dependent image

#ifndef AOIMAGE_H
#define AOIMAGE_H

#include "aoapplication.h"

#include <QDebug>
#include <QLabel>

class AOImage : public QLabel {
public:
  AOImage(QWidget *parent, AOApplication *p_ao_app);
  ~AOImage();

  QWidget *m_parent;
  AOApplication *ao_app;

  bool set_image(QString p_image);
  bool set_chatbox(QString p_path);
  void set_size_and_pos(QString identifier);
};

#endif // AOIMAGE_H
