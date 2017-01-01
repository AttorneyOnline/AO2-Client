//This class represents a static theme-dependent image

#ifndef AOIMAGE_H
#define AOIMAGE_H

#include <QLabel>

class AOImage : public QLabel
{
public:
  AOImage(QWidget *parent, int x_pos, int y_pos, int x_size, int y_size);
  ~AOImage();

  void set_image(QString p_image);
};

#endif // AOIMAGE_H
