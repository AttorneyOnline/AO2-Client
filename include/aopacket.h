#ifndef AOPACKET_H
#define AOPACKET_H

#include <QDebug>
#include <QString>
#include <QStringList>

class AOPacket {
public:
  AOPacket(QString header) : m_header(header){}
  AOPacket(QString header, QStringList p_contents) : m_header(header), m_contents(p_contents){}

  QString get_header() { return m_header; }
  QStringList &get_contents() { return m_contents; }
  QString to_string(bool encoded = false);

  void net_encode();
  void net_decode();

  static void escape(QStringList &contents);
  static void unescape(QStringList &contents);
private:

  QString m_header;
  QStringList m_contents;
};

#endif // AOPACKET_H
