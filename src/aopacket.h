#pragma once

#include <QString>
#include <QStringList>

class AOPacket
{
public:
  static QString encode(QString data);
  static QString decode(QString data);

  AOPacket(QString header);
  AOPacket(QString header, QStringList content);

  QString get_header();
  QStringList &get_content();
  QString to_string(bool ensureEncoded = false);

private:
  QString m_header;
  QStringList m_content;
};
