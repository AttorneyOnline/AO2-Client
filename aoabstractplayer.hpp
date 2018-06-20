#ifndef AOABSTRACTPLAYER_HPP
#define AOABSTRACTPLAYER_HPP

#include <QObject>
#include <QString>

#include "aoapplication.h"
#include "aobasshandle.hpp"

class AOAbstractPlayer : public QObject
{
  Q_OBJECT

public:
  AOAbstractPlayer(QObject *p_parent, AOApplication *p_ao_app);

  int get_volume();

public slots:
  void set_volume(int p_volume);

signals:
  void starting();
  void stopping();
  void new_volume(int p_volume);

protected:
  AOApplication* ao_app = nullptr;

private:
  int m_volume = 0;
};

#endif // AOABSTRACTPLAYER_HPP
