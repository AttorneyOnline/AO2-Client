#ifndef AOPACKET_H
#define AOPACKET_H

#include <QDebug>
#include <QString>
#include <QStringList>

class AOPacket {
public:
  AOPacket(QString p_packet_string);
  AOPacket(QString header, QStringList &p_contents) : m_header(header), m_contents(p_contents){}

  QString get_header() { return m_header; }
  QStringList &get_contents() { return m_contents; }
  QString to_string();

  void net_encode();
  void net_decode();

private:

  QString m_header;
  QStringList m_contents;
};

#endif // AOPACKET_H
