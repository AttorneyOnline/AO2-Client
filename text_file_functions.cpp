#include "text_file_functions.h"

#include "path_functions.h"
#include "file_functions.h"

#include <QTextStream>
#include <QStringList>
#include <QVector>

QString get_user_theme(){
  QFile config_file(get_base_path() + "config.ini");
  if (!config_file.open(QIODevice::ReadOnly))
      return "default";

  QTextStream in(&config_file);

  while(!in.atEnd())
  {
    QString line = in.readLine();

    if (line.startsWith("theme"))
    {
      QStringList line_elements = line.split("=");

      if (line_elements.size() > 1)
        return line_elements.at(1).trimmed();
    }
  }

  return "default";
}

void write_to_serverlist_txt(QString p_line)
{
  QFile serverlist_txt;
  QString serverlist_txt_path = get_base_path() + "serverlist.txt";

  serverlist_txt.setFileName(serverlist_txt_path);

  if (!serverlist_txt.open(QIODevice::WriteOnly | QIODevice::Append))
  {
    return;
  }

  QTextStream out(&serverlist_txt);

  out << p_line << '\n';

  serverlist_txt.close();
}

QVector<server_type> read_serverlist_txt()
{
  QVector<server_type> f_server_list;

  QFile serverlist_txt;
  QString serverlist_txt_path = get_base_path() + "serverlist.txt";

  serverlist_txt.setFileName(serverlist_txt_path);

  if (!serverlist_txt.open(QIODevice::ReadOnly))
  {
    return f_server_list;
  }

  QTextStream in(&serverlist_txt);

  while(!in.atEnd())
  {
    QString line = in.readLine();
    server_type f_server;
    QStringList line_contents = line.split(":");

    if (line_contents.size() < 3)
        continue;

    f_server.ip = line_contents.at(0);
    f_server.port = line_contents.at(1).toInt();
    f_server.name = line_contents.at(2);
    f_server.desc = "";

    f_server_list.append(f_server);
  }

  return f_server_list;
}
