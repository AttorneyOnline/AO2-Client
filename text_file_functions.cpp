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

//returns whatever is to the right of "search_line =" within target_tag and terminator_tag, trimmed
//returns the empty string if the search line couldnt be found
QString AOApplication::read_char_ini(QString p_char, QString p_search_line, QString target_tag, QString terminator_tag)
{
  QString char_ini_path = get_character_path(p_char) + "char.ini";

  QFile char_ini;

  char_ini.setFileName(char_ini_path);

  if (!char_ini.open(QIODevice::ReadOnly))
    return "";

  QTextStream in(&char_ini);

  //because there are char inis that look like [eMoTIonS] for whatever reason
  target_tag = target_tag.toLower();
  terminator_tag = terminator_tag.toLower();
  bool tag_found = false;

  while(!in.atEnd())
  {
    QString line = in.readLine();

    if (line.toLower().startsWith(terminator_tag))
      break;

    if (line.toLower().startsWith(target_tag))
    {
      tag_found = true;
      continue;
    }

    if (!line.startsWith(p_search_line))
      continue;

    QStringList line_elements = line.split("=");

    if (line_elements.at(0).trimmed() != p_search_line)
      continue;

    if (line_elements.size() < 2)
      continue;

    if (tag_found)
    {
      char_ini.close();
      return line_elements.at(1).trimmed();
    }
  }

  char_ini.close();
  return "";
}

QString AOApplication::get_char_name(QString p_char)
{
  QString f_result = read_char_ini(p_char, "name", "[Options]", "[Time]");

  if (f_result == "")
    return p_char;
  else return f_result;
}

QString AOApplication::get_showname(QString p_char)
{
  QString f_result = read_char_ini(p_char, "showname", "[Options]", "[Time]");

  if (f_result == "")
    return p_char;
  else return f_result;
}

QString AOApplication::get_char_side(QString p_char)
{
  QString f_result = read_char_ini(p_char, "side", "[Options]", "[Time]");

  if (f_result == "")
    return "wit";
  else return f_result;
}

QString AOApplication::get_gender(QString p_char)
{
  QString f_result = read_char_ini(p_char, "gender", "[Options]", "[Time]");

  if (f_result == "")
    return "male";
  else return f_result;
}

QString AOApplication::get_chat(QString p_char)
{
  QString f_result = read_char_ini(p_char, "chat", "[Options]", "[Time]");

  //handling the correct order of chat is a bit complicated, we let the caller do it
  return f_result;
}

int AOApplication::get_preanim_duration(QString p_char, QString p_emote)
{
  QString f_result = read_char_ini(p_char, p_emote, "[Time]", "[Emotions]");

  if (f_result == "")
    return 0;
  else return f_result.toInt();
}

int AOApplication::get_emote_number(QString p_char)
{
  QString f_result = read_char_ini(p_char, "number", "[Emotions]", "[SoundN]");

  if (f_result == "")
    return 0;
  else return f_result.toInt();
}

QString AOApplication::get_pre_emote(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "[Emotions]", "[SoundN]");

  qDebug() << "f_result" << f_result;

  QStringList result_contents = f_result.split("#");



  if (result_contents.size() < 4)
  {
    qDebug() << "W: misformatted char.ini: " << p_char << ", " << p_emote;
    return "normal";
  }
  else return result_contents.at(1);
}

QString AOApplication::get_emote(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "[Emotions]", "[SoundN]");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4)
  {
    qDebug() << "W: misformatted char.ini: " << p_char << ", " << p_emote;
    return "normal";
  }
  else return result_contents.at(2);
}

int AOApplication::get_emote_mod(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "[Emotions]", "[SoundN]");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4)
  {
    qDebug() << "W: misformatted char.ini: " << p_char << ", " << QString::number(p_emote);
    return 0;
  }
  else return result_contents.at(3).toInt();
}

QString AOApplication::get_sfx_name(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "[SoundN]", "[SoundT]");

  if (f_result == "")
    return "1";
  else return f_result;
}

int AOApplication::get_sfx_delay(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "[SoundT]", "[TextDelay]");

  if (f_result == "")
    return 1;
  else return f_result.toInt();
}

int AOApplication::get_text_delay(QString p_char, QString p_emote)
{
  QString f_result = read_char_ini(p_char, p_emote, "[TextDelay]", "END_OF_FILE");

  if (f_result == "")
    return -1;
  else return f_result.toInt();
}





