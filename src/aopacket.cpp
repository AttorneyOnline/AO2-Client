#include "aopacket.h"

QString AOPacket::encode(QString data)
{
  return data.replace("#", "<num>").replace("%", "<percent>").replace("$", "<dollar>").replace("&", "<and>");
}

QString AOPacket::decode(QString data)
{
  return data.replace("<num>", "#").replace("<percent>", "%").replace("<dollar>", "$").replace("<and>", "&");
}

AOPacket::AOPacket()
{}

AOPacket::AOPacket(QString header)
    : m_header(header)
{}

AOPacket::AOPacket(QString header, QStringList content)
    : m_header(header)
    , m_content(content)
{}

QString AOPacket::header()
{
  return m_header;
}

QStringList &AOPacket::content()
{
  return m_content;
}

QString AOPacket::toString(bool ensureEncoded)
{
  QString message = m_header;
  if (!m_content.isEmpty())
  {
    for (QString item : std::as_const(m_content))
    {
      if (ensureEncoded)
      {
        item = encode(item);
      }
      message += "#" + item;
    }
  }

  return message + "#%";
}
