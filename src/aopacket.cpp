#include "aopacket.h"

AOPacket::AOPacket(QString p_packet_string)
{
  QStringList packet_contents = p_packet_string.split("#");

  m_header = packet_contents.first();
  m_contents = packet_contents.mid(1, packet_contents.size()-2); // trims %
}

QString AOPacket::to_string()
{
  return m_header + "#" + m_contents.join("#") + "#%";
}

void AOPacket::net_encode()
{
  m_contents.replaceInStrings("#", "<num>")
      .replaceInStrings("%", "<percent>")
      .replaceInStrings("$", "<dollar>")
      .replaceInStrings("&", "<and>");
}

void AOPacket::net_decode()
{
  m_contents.replaceInStrings("<num>", "#")
    .replaceInStrings("<percent>", "%")
    .replaceInStrings("<dollar>", "$")
    .replaceInStrings("<and>", "&");
}
