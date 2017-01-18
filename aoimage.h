//This class represents a static theme-dependent image

#ifndef AOIMAGE_H
#define AOIMAGE_H

#include <QLabel>

class AOImage : public QLabel
{
public:
  AOImage(QWidget *parent);
  ~AOImage();

  void set_image(QString p_image);
  void set_size_and_pos(QString identifier);
};

#endif // AOIMAGE_H
