#include "aoapplication.h"

#include "file_functions.h"

#include <QTextStream>
#include <QStringList>
#include <QVector>
#include <QDebug>
#include <QColor>

/*
 * This may no longer be necessary, if we use the QSettings class.
 *
QString AOApplication::read_config(QString searchline)
{
  QString return_value = "";

  QFile config_file(get_base_path() + "config.ini");
  if (!config_file.open(QIODevice::ReadOnly))
      return return_value;

  QTextStream in(&config_file);

  while(!in.atEnd())
  {
    QString f_line = in.readLine().trimmed();

    if (!f_line.startsWith(searchline))
      continue;

    QStringList line_elements = f_line.split("=");

    if (line_elements.at(0).trimmed() != searchline)
      continue;

    if (line_elements.size() < 2)
      continue;

    return_value = line_elements.at(1).trimmed();
    break;
  }

  config_file.close();

  return return_value;
}
*/

QString AOApplication::read_theme()
{
  QString result = configini->value("theme", "default").value<QString>();
  return result;
}

int AOApplication::read_blip_rate()
{
  int result = configini->value("blip_rate", 1).toInt();
  return result;
}

int AOApplication::get_default_music()
{
  int result = configini->value("default_music", 50).toInt();
  return result;
}

int AOApplication::get_default_sfx()
{
  int result = configini->value("default_sfx", 50).toInt();
  return result;
}

int AOApplication::get_default_blip()
{
  int result = configini->value("default_blip", 50).toInt();
  return result;
}

int AOApplication::get_max_log_size()
{
  int result = configini->value("log_maximum", 200).toInt();
  return result;
}

bool AOApplication::get_log_goes_downwards()
{
  QString result = configini->value("log_goes_downwards", "false").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::get_showname_enabled_by_default()
{
  QString result = configini->value("show_custom_shownames", "false").value<QString>();
  return result.startsWith("true");
}

QString AOApplication::get_default_username()
{
  QString result = configini->value("default_username", "").value<QString>();
  return result;
}

QString AOApplication::get_audio_output_device()
{
  QString result = configini->value("default_audio_device", "default").value<QString>();
  return result;
}

QStringList AOApplication::get_call_words()
{
  QStringList return_value;

  QFile callwords_ini;

  callwords_ini.setFileName(get_base_path() + "callwords.ini");

  if (!callwords_ini.open(QIODevice::ReadOnly))
    return return_value;

  QTextStream in(&callwords_ini);

  while (!in.atEnd())
  {
    QString line = in.readLine();
    return_value.append(line);
  }

  return return_value;
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

  out << "\r\n" << p_line;

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

QString AOApplication::read_design_ini(QString p_identifier, QString p_design_path)
{
  QFile design_ini;

  design_ini.setFileName(p_design_path);

  if (!design_ini.open(QIODevice::ReadOnly))
  {
    return "";
  }
  QTextStream in(&design_ini);

  QString result = "";

  while (!in.atEnd())
  {
    QString f_line = in.readLine().trimmed();

    if (!f_line.startsWith(p_identifier))
      continue;

    QStringList line_elements = f_line.split("=");

    if (line_elements.at(0).trimmed() != p_identifier)
      continue;

    if (line_elements.size() < 2)
      continue;

    result = line_elements.at(1).trimmed();
    break;
  }

  design_ini.close();

  return result;
}

QPoint AOApplication::get_button_spacing(QString p_identifier, QString p_file)
{
  QString design_ini_path = get_theme_path() + p_file;
  QString default_path = get_default_theme_path() + p_file;
  QString f_result = read_design_ini(p_identifier, design_ini_path);

  QPoint return_value;

  return_value.setX(0);
  return_value.setY(0);

  if (f_result == "")
  {
    f_result = read_design_ini(p_identifier, default_path);

    if (f_result == "")
      return return_value;
  }

  QStringList sub_line_elements = f_result.split(",");

  if (sub_line_elements.size() < 2)
    return return_value;

  return_value.setX(sub_line_elements.at(0).toInt());
  return_value.setY(sub_line_elements.at(1).toInt());

  return return_value;
}

pos_size_type AOApplication::get_element_dimensions(QString p_identifier, QString p_file)
{
  QString design_ini_path = get_theme_path() + p_file;
  QString default_path = get_default_theme_path() + p_file;
  QString f_result = read_design_ini(p_identifier, design_ini_path);

  pos_size_type return_value;

  return_value.x = 0;
  return_value.y = 0;
  return_value.width = -1;
  return_value.height = -1;

  if (f_result == "")
  {
    f_result = read_design_ini(p_identifier, default_path);

    if (f_result == "")
      return return_value;
  }

  QStringList sub_line_elements = f_result.split(",");

  if (sub_line_elements.size() < 4)
    return return_value;

  return_value.x = sub_line_elements.at(0).toInt();
  return_value.y = sub_line_elements.at(1).toInt();
  return_value.width = sub_line_elements.at(2).toInt();
  return_value.height = sub_line_elements.at(3).toInt();

  return return_value;
}

int AOApplication::get_font_size(QString p_identifier, QString p_file)
{
  QString design_ini_path = get_theme_path() + p_file;
  QString default_path = get_default_theme_path() + p_file;
  QString f_result = read_design_ini(p_identifier, design_ini_path);

  if (f_result == "")
  {
    f_result = read_design_ini(p_identifier, default_path);

    if (f_result == "")
      return 10;
  }

  return f_result.toInt();
}

QColor AOApplication::get_color(QString p_identifier, QString p_file)
{
  QString design_ini_path = get_theme_path() + p_file;
  QString default_path = get_default_theme_path() + p_file;
  QString f_result = read_design_ini(p_identifier, design_ini_path);

  QColor return_color(255, 255, 255);

  if (f_result == "")
  {
    f_result = read_design_ini(p_identifier, default_path);

    if (f_result == "")
      return return_color;
  }

  QStringList color_list = f_result.split(",");

  if (color_list.size() < 3)
    return return_color;

  return_color.setRed(color_list.at(0).toInt());
  return_color.setGreen(color_list.at(1).toInt());
  return_color.setBlue(color_list.at(2).toInt());

  return return_color;
}

QString AOApplication::get_sfx(QString p_identifier)
{
  QString design_ini_path = get_theme_path() + "courtroom_sounds.ini";
  QString default_path = get_default_theme_path() + "courtroom_sounds.ini";
  QString f_result = read_design_ini(p_identifier, design_ini_path);

  QString return_sfx = "";

  if (f_result == "")
  {
    f_result = read_design_ini(p_identifier, default_path);

    if (f_result == "")
      return return_sfx;
  }

  return_sfx = f_result;

  return return_sfx;
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

  bool tag_found = false;

  while(!in.atEnd())
  {
    QString line = in.readLine();

    if (QString::compare(line, terminator_tag, Qt::CaseInsensitive) == 0)
      break;

    if (line.startsWith(target_tag, Qt::CaseInsensitive))
    {
      tag_found = true;
      continue;
    }

    if (!line.startsWith(p_search_line, Qt::CaseInsensitive))
      continue;

    QStringList line_elements = line.split("=");

    if (QString::compare(line_elements.at(0).trimmed(), p_search_line, Qt::CaseInsensitive) != 0)
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
  return f_result.toLower();
}

QString AOApplication::get_char_shouts(QString p_char)
{
  QString f_result = read_char_ini(p_char, "shouts", "[Options]", "[Time]");

  return f_result.toLower();
}

int AOApplication::get_preanim_duration(QString p_char, QString p_emote)
{
  QString f_result = read_char_ini(p_char, p_emote, "[Time]", "[Emotions]");

  if (f_result == "")
    return -1;
  else return f_result.toInt();
}

int AOApplication::get_ao2_preanim_duration(QString p_char, QString p_emote)
{
  QString f_result = read_char_ini(p_char, "%" + p_emote, "[Time]", "[Emotions]");

  if (f_result == "")
    return -1;
  else return f_result.toInt();
}

int AOApplication::get_emote_number(QString p_char)
{
  QString f_result = read_char_ini(p_char, "number", "[Emotions]", "[SoundN]");

  if (f_result == "")
    return 0;
  else return f_result.toInt();
}

QString AOApplication::get_emote_comment(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "[Emotions]", "[SoundN]");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4)
  {
    qDebug() << "W: misformatted char.ini: " << p_char << ", " << p_emote;
    return "normal";
  }
  else return result_contents.at(0);
}

QString AOApplication::get_pre_emote(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "[Emotions]", "[SoundN]");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4)
  {
    qDebug() << "W: misformatted char.ini: " << p_char << ", " << p_emote;
    return "";
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

int AOApplication::get_desk_mod(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "[Emotions]", "[SoundN]");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 5)
    return -1;

  QString string_result = result_contents.at(4);
  if (string_result == "")
    return -1;

  else return string_result.toInt();
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

bool AOApplication::get_blank_blip()
{
    QString result = configini->value("blank_blip", "false").value<QString>();
    return result.startsWith("true");
}

bool AOApplication::is_discord_enabled()
{
    QString result = configini->value("discord", "true").value<QString>();
    return result.startsWith("true");
}



