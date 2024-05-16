#pragma once

#include "aoapplication.h"

#include <QMovie>
#include <QPushButton>

class AOButton : public QPushButton
{
  Q_OBJECT

public:
  explicit AOButton(AOApplication *ao_app, QWidget *parent = nullptr);
  virtual ~AOButton();

  void setImage(QString image_name);

private:
  AOApplication *ao_app;

  QMovie *m_movie = nullptr;

  void deleteMovie();

private Q_SLOTS:
  void handleNextFrame();

  void updateIcon(QPixmap icon);
};
