#include "aopacket.h"

QString AOPacket::to_string(bool encoded)
{
  QStringList contents = m_contents;
  if (encoded) {
    escape(contents);
  }
  // Our packet is just the header by itself
  if (contents.isEmpty()) {
    return m_header + "#%";
  }
  return m_header + "#" + contents.join("#") + "#%";
}

void AOPacket::net_encode()
{
  escape(m_contents);
}

void AOPacket::net_decode()
{
  unescape(m_contents);
}

void AOPacket::escape(QStringList &contents)
{
  contents.replaceInStrings("#", "<num>")
    .replaceInStrings("%", "<percent>")
    .replaceInStrings("$", "<dollar>")
    .replaceInStrings("&", "<and>");

}

void AOPacket::unescape(QStringList &contents)
{
  contents.replaceInStrings("<num>", "#")
    .replaceInStrings("<percent>", "%")
    .replaceInStrings("<dollar>", "$")
    .replaceInStrings("<and>", "&");

}
