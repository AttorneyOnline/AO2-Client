//This class represents a static theme-dependent image

#ifndef AOIMAGE_H
#define AOIMAGE_H

#include "text_file_functions.h"

#include <QLabel>
#include <QDebug>

class AOImage : public QLabel
{
public:
  AOImage(QWidget *parent);
  ~AOImage();

  QWidget *m_parent;

  void set_image(QString p_image);
  void set_image_from_path(QString p_path);
  void set_size_and_pos(QString identifier);
};

#endif // AOIMAGE_H
