#ifndef AOSCENE_H
#define AOSCENE_H

#include <QLabel>

class Courtroom;

class AOScene : public QLabel
{
  Q_OBJECT
public:
  explicit AOScene(Courtroom *parent);

  void set_image(QString p_image);

private:
  Courtroom *m_courtroom;

};

#endif // AOSCENE_H
