#include "aoapplication.h"

#include "file_functions.h"

#include <QTextStream>
#include <QStringList>
#include <QVector>
#include <QDebug>

QString AOApplication::read_user_theme(){
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

void AOApplication::write_to_serverlist_txt(QString p_line)
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

QVector<server_type> AOApplication::read_serverlist_txt()
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

pos_size_type AOApplication::get_pos_and_size(QString p_identifier, QString p_design_path)
{
  QFile design_ini;

  pos_size_type return_value;

  design_ini.setFileName(p_design_path);

  if (!design_ini.open(QIODevice::ReadOnly))
  {
    qDebug() << "W: Could not open or read " << p_design_path;
    //caller should deal with the result properly(check width and height of output for negatives)
    return_value.height = -1;
    return_value.width = -1;

    return return_value;
  }

  QTextStream in(&design_ini);

  while (!in.atEnd())
  {
    QString f_line = in.readLine();

    if (!f_line.startsWith(p_identifier))
      continue;

    QStringList line_elements = f_line.split("=");

    if (line_elements.size() < 2)
      continue;

    QStringList sub_line_elements = line_elements.at(1).split(",");

    if (sub_line_elements.size() < 4)
      continue;

    //T0D0 check if integer conversion actually succeeded
    return_value.x = sub_line_elements.at(0).toInt();
    return_value.y = sub_line_elements.at(1).toInt();
    return_value.width = sub_line_elements.at(2).toInt();
    return_value.height = sub_line_elements.at(3).toInt();

    return return_value;
  }

  qDebug() << "W: Could not find proper " << p_identifier << " in " << p_design_path;
  //caller should deal with the result properly(check width and height of output for negatives)
  return_value.height = -1;
  return_value.width = -1;

  return return_value;
}

QString AOApplication::get_char_side(QString p_char)
{
  QString char_ini_path = get_character_path(p_char) + "char.ini";

  QFile char_ini;

  char_ini.setFileName(char_ini_path);

  if (!char_ini.open(QIODevice::ReadOnly))
  {
    //default to wit and don't make a big deal about it
    return "wit";
  }

  QTextStream in(&char_ini);

  while(!in.atEnd())
  {
    QString line = in.readLine();

    if (!line.startsWith("side"))
      continue;

    QStringList line_elements = line.split("=");

    if (line_elements.size() < 2)
      continue;

    //note that we do not validate if this is a valid side or not. that's up to the caller
    return line_elements.at(1).trimmed().toLower();
  }

  return "wit";
}

QString AOApplication::get_showname(QString p_char)
{
  QString char_ini_path = get_character_path(p_char) + "char.ini";

  QFile char_ini;

  char_ini.setFileName(char_ini_path);

  if (!char_ini.open(QIODevice::ReadOnly))
  {
    //default to empty string
    return "";
  }

  QTextStream in(&char_ini);

  while(!in.atEnd())
  {
    QString line = in.readLine();

    if (!line.startsWith("showname"))
      continue;

    QStringList line_elements = line.split("=");

    if (line_elements.size() < 2)
      continue;

    return line_elements.at(1).trimmed();
  }

  return "";
}

QString AOApplication::get_chat(QString p_char)
{
  QString char_ini_path = get_character_path(p_char) + "char.ini";

  QFile char_ini;

  char_ini.setFileName(char_ini_path);

  if (!char_ini.open(QIODevice::ReadOnly))
  {
    return "";
  }

  QTextStream in(&char_ini);

  while(!in.atEnd())
  {
    QString line = in.readLine();

    if (!line.startsWith("chat"))
      continue;

    QStringList line_elements = line.split("=");

    if (line_elements.size() < 2)
      continue;

    return line_elements.at(1).trimmed().toLower() + ".png";
  }

  return "";
}

int AOApplication::get_preanim_duration(QString p_char, QString p_emote)
{
  QString char_ini_path = get_character_path(p_char) + "char.ini";

  QFile char_ini;

  char_ini.setFileName(char_ini_path);

  if (!char_ini.open(QIODevice::ReadOnly))
  {
    //means preanim will finish instantly(i.e. not play)
    return 0;
  }

  QTextStream in(&char_ini);

  while(!in.atEnd())
  {
    QString line = in.readLine();

    if (!line.startsWith(p_emote))
      continue;

    QStringList line_elements = line.split("=");

    if (line_elements.size() < 2)
      continue;

    return line_elements.at(1).trimmed().toInt();
  }

  return 0;
}

int AOApplication::get_text_delay(QString p_char, QString p_emote)
{
  //T0D0: make a sane format for this and implement function
  p_char.toLower();
  p_emote.toLower();
  return -1;
}

QString AOApplication::get_char_name(QString p_char)
{
  QString char_ini_path = get_character_path(p_char) + "char.ini";

  QFile char_ini;

  char_ini.setFileName(char_ini_path);

  if (!char_ini.open(QIODevice::ReadOnly))
  {
    return "";
  }

  QTextStream in(&char_ini);

  while(!in.atEnd())
  {
    QString line = in.readLine();

    if (!line.startsWith("name"))
      continue;

    QStringList line_elements = line.split("=");

    if (line_elements.size() < 2)
      continue;

    return line_elements.at(1).trimmed();
  }

  return "";
}

int AOApplication::get_emote_number(QString p_char)
{
  QString char_ini_path = get_character_path(p_char) + "char.ini";
  QFile char_ini;
  char_ini.setFileName(char_ini_path);

  if (!char_ini.open(QIODevice::ReadOnly))
  {
    return 0;
  }

  QTextStream in(&char_ini);
  bool emotions_found = false;

  while(!in.atEnd())
  {
    QString line = in.readLine();

    if (line.startsWith("[SoundN]"))
      return 0;

    if (line.startsWith("[Emotions]"))
      emotions_found = true;

    if (!line.startsWith("number"))
      continue;

    QStringList line_elements = line.split("=");

    if (line_elements.size() < 2)
      continue;

    if (emotions_found)
      return line_elements.at(1).trimmed().toInt();
  }

  return 0;
}

QString AOApplication::get_pre_emote(QString p_char, int p_emote)
{
  QString char_ini_path = get_character_path(p_char) + "char.ini";
  QFile char_ini;
  char_ini.setFileName(char_ini_path);

  if (!char_ini.open(QIODevice::ReadOnly))
  {
    return "normal";
  }

  QTextStream in(&char_ini);
  bool emotions_found = false;
  QString search_line = QString::number(p_emote + 1);

  while(!in.atEnd())
  {
    QString line = in.readLine();

    if (line.startsWith("[SoundN]"))
      return "normal";

    if (line.startsWith("[Emotions]"))
      emotions_found = true;

    if (!line.startsWith(search_line))
      continue;

    QStringList line_elements = line.split("=");

    if (line_elements.size() < 4)
      continue;

    if (emotions_found)
      return line_elements.at(1).trimmed();
  }

  return "normal";
}

QString AOApplication::get_emote(QString p_char, int p_emote)
{
  QString char_ini_path = get_character_path(p_char) + "char.ini";
  QFile char_ini;
  char_ini.setFileName(char_ini_path);

  if (!char_ini.open(QIODevice::ReadOnly))
  {
    return "normal";
  }

  QTextStream in(&char_ini);
  bool emotions_found = false;
  QString search_line = QString::number(p_emote + 1);

  while(!in.atEnd())
  {
    QString line = in.readLine();

    if (line.startsWith("[SoundN]"))
      return "normal";

    if (line.startsWith("[Emotions]"))
      emotions_found = true;

    if (!line.startsWith(search_line))
      continue;

    QStringList line_elements = line.split("=");

    if (line_elements.size() < 4)
      continue;

    if (emotions_found)
      return line_elements.at(2).trimmed();
  }

  return "normal";
}

QString AOApplication::get_sfx_name(QString p_char, int p_emote)
{
  QString char_ini_path = get_character_path(p_char) + "char.ini";
  QFile char_ini;
  char_ini.setFileName(char_ini_path);

  if (!char_ini.open(QIODevice::ReadOnly))
  {
    return "1";
  }

  QTextStream in(&char_ini);
  bool soundn_found = false;
  QString search_line = QString::number(p_emote + 1);

  while(!in.atEnd())
  {
    QString line = in.readLine();

    if (line.startsWith("[SoundT]"))
      return "1";

    if (line.startsWith("[SoundN]"))
      soundn_found = true;

    if (!soundn_found)
      continue;

    if (!line.startsWith(search_line))
      continue;

    QStringList line_elements = line.split("=");

    if (line_elements.size() < 2)
      continue;


    return line_elements.at(1).trimmed();
  }

  return "1";
}


int AOApplication::get_sfx_delay(QString p_char, int p_emote)
{
  QString char_ini_path = get_character_path(p_char) + "char.ini";
  QFile char_ini;
  char_ini.setFileName(char_ini_path);

  if (!char_ini.open(QIODevice::ReadOnly))
  {
    return 0;
  }

  QTextStream in(&char_ini);
  bool soundt_found = false;
  QString search_line = QString::number(p_emote + 1);

  while(!in.atEnd())
  {
    QString line = in.readLine();

    if (line.startsWith("[SoundT]"))
      soundt_found = true;

    if (!soundt_found)
      continue;

    if (!line.startsWith(search_line))
      continue;

    QStringList line_elements = line.split("=");

    if (line_elements.size() < 2)
      continue;

    return line_elements.at(1).trimmed().toInt();
  }

  return 0;
}

int AOApplication::get_emote_mod(QString p_char, int p_emote)
{
  QString char_ini_path = get_character_path(p_char) + "char.ini";
  QFile char_ini;
  char_ini.setFileName(char_ini_path);

  if (!char_ini.open(QIODevice::ReadOnly))
  {
    return 0;
  }

  QTextStream in(&char_ini);
  bool emotions_found = false;
  QString search_line = QString::number(p_emote + 1);

  while(!in.atEnd())
  {
    QString line = in.readLine();

    if (line.startsWith("[SoundN]"))
      return 0;

    if (line.startsWith("[Emotions]"))
      emotions_found = true;

    if (!line.startsWith(search_line))
      continue;

    QStringList line_elements = line.split("=");

    if (line_elements.size() < 4)
      continue;

    if (emotions_found)
      return line_elements.at(3).trimmed().toInt();
  }

  return 0;
}


