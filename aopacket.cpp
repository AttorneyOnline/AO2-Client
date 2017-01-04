#include "aopacket.h"

AOPacket::AOPacket(QString p_packet_string)
{
  QStringList packet_contents = p_packet_string.split("#");

  m_header = packet_contents.at(0);

  for(int n_string = 1 ; n_string < packet_contents.size() - 1 ; ++n_string)
  {
    m_contents.append(packet_contents.at(n_string));
  }
}

AOPacket::~AOPacket()
{

}

QString AOPacket::to_string()
{
  QString f_string = m_header;

  for (QString i_string : m_contents)
  {
    f_string += ("#" + i_string);
  }

  f_string += "#%";

  return f_string;
}
