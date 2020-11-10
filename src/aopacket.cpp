#include "aopacket.h"

AOPacket::AOPacket(QString p_packet_string)
{
  QStringList packet_contents = p_packet_string.split("#");

  m_header = packet_contents.at(0);

  for (int n_string = 1; n_string < packet_contents.size() - 1; ++n_string) {
    m_contents.append(packet_contents.at(n_string));
  }
}

AOPacket::AOPacket(QString p_header, QStringList &p_contents)
{
  m_header = p_header;
  m_contents = p_contents;
}

AOPacket::~AOPacket() {}

QString AOPacket::to_string()
{
  QString f_string = m_header;

  for (QString i_string : m_contents) {
    f_string += ("#" + i_string);
  }

  f_string += "#%";


  return f_string;
}

void AOPacket::net_encode()
{
  for (int n_element = 0; n_element < m_contents.size(); ++n_element) {
    QString f_element = m_contents.at(n_element);
    f_element.replace("#", "<num>")
        .replace("%", "<percent>")
        .replace("$", "<dollar>")
        .replace("&", "<and>");

    m_contents.removeAt(n_element);
    m_contents.insert(n_element, f_element);
  }
}

void AOPacket::net_decode()
{
  for (int n_element = 0; n_element < m_contents.size(); ++n_element) {
    QString f_element = m_contents.at(n_element);
    f_element.replace("<num>", "#")
        .replace("<percent>", "%")
        .replace("<dollar>", "$")
        .replace("<and>", "&");

    m_contents.removeAt(n_element);
    m_contents.insert(n_element, f_element);
  }
}
