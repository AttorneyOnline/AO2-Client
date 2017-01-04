#ifndef AOPACKET_H
#define AOPACKET_H

#include <QString>
#include <QStringList>

class AOPacket
{
public:
  AOPacket(QString p_packet_string);
  ~AOPacket();

  QString get_header() {return m_header;}
  QStringList &get_contents() {return m_contents;}
  QString to_string();

private:
  QString m_header;
  QStringList m_contents;
};

#endif // AOPACKET_H
