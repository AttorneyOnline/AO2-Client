#include "text_file_functions.h"

QString AOApplication::read_theme()
{
  QString result = configini->value("theme", "default").value<QString>();
  return result;
}

int AOApplication::read_blip_rate()
{
  int result = configini->value("blip_rate", 2).toInt();

  if (result < 1)
    return 1;

  return result;
}

QString AOApplication::get_ooc_name()
{
  QString result = configini->value("ooc_name").value<QString>();
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

int AOApplication::stay_time()
{
  int result = configini->value("stay_time", 200).toInt();
  return result;
}

int AOApplication::get_chat_ratelimit()
{
  int result = configini->value("chat_ratelimit", 300).toInt();
  return result;
}

bool AOApplication::get_log_goes_downwards()
{
  QString result =
      configini->value("log_goes_downwards", "true").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::get_log_newline()
{
  QString result = configini->value("log_newline", "false").value<QString>();
  return result.startsWith("true");
}

int AOApplication::get_log_margin()
{
  int result = configini->value("log_margin", 0).toInt();
  return result;
}

bool AOApplication::get_log_timestamp()
{
  QString result = configini->value("log_timestamp", "false").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::get_log_ic_actions()
{
  QString result =
      configini->value("log_ic_actions", "true").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::get_showname_enabled_by_default()
{
  QString result =
      configini->value("show_custom_shownames", "true").value<QString>();
  return result.startsWith("true");
}

QString AOApplication::get_default_username()
{
  QString result = configini->value("default_username", "").value<QString>();
  if (result.isEmpty())
    return get_ooc_name();
  else
    return result;
}

QString AOApplication::get_audio_output_device()
{
  QString result =
      configini->value("default_audio_device", "default").value<QString>();
  return result;
}

QStringList AOApplication::get_call_words()
{
  return get_list_file(get_base_path() + "callwords.ini");
}

QStringList AOApplication::get_list_file(QString p_file)
{
  QStringList return_value;

  QFile p_ini;

  p_ini.setFileName(p_file);

  if (!p_ini.open(QIODevice::ReadOnly))
    return return_value;

  QTextStream in(&p_ini);

  while (!in.atEnd()) {
    QString line = in.readLine();
    return_value.append(line);
  }

  return return_value;
}

QString AOApplication::read_file(QString filename)
{
  QFile f_log(filename);

  if (!f_log.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Couldn't open" << filename;
    return "";
  }

  QTextStream in(&f_log);
  QString text = in.readAll();
  f_log.close();
  return text;
}

bool AOApplication::write_to_file(QString p_text, QString p_file, bool make_dir)
{
  QString path = QFileInfo(p_file).path();
  if (make_dir) {
    // Create the dir if it doesn't exist yet
    QDir dir(path);
    if (!dir.exists())
      if (!dir.mkpath("."))
        return false;
  }

  QFile f_log(p_file);
  if (f_log.open(QIODevice::WriteOnly | QIODevice::Text |
                 QIODevice::Truncate)) {
    QTextStream out(&f_log);

    out << p_text;

    f_log.flush();
    f_log.close();
    return true;
  }
  return false;
}

bool AOApplication::append_to_file(QString p_text, QString p_file,
                                   bool make_dir)
{
  if(!file_exists(p_file)) //Don't create a newline if file didn't exist before now
  {
    return write_to_file(p_text, p_file, make_dir);
  }
  QString path = QFileInfo(p_file).path();
  // Create the dir if it doesn't exist yet
  if (make_dir) {
    QDir dir(path);
    if (!dir.exists())
      if (!dir.mkpath("."))
        return false;
  }

  QFile f_log(p_file);
  if (f_log.open(QIODevice::WriteOnly | QIODevice::Append)) {
    QTextStream out(&f_log);

    out << "\r\n" << p_text;

    f_log.flush();
    f_log.close();
    return true;
  }
  return false;
}

void AOApplication::write_to_serverlist_txt(QString p_line)
{
  QFile serverlist_txt;
  QString serverlist_txt_path = get_base_path() + "serverlist.txt";

  serverlist_txt.setFileName(serverlist_txt_path);

  if (!serverlist_txt.open(QIODevice::WriteOnly | QIODevice::Append)) {
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

  if (!serverlist_txt.open(QIODevice::ReadOnly)) {
    return f_server_list;
  }

  QTextStream in(&serverlist_txt);

  while (!in.atEnd()) {
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

  server_type demo_server;
  demo_server.ip = "127.0.0.1";
  demo_server.port = 99999;
  demo_server.name = "Demo playback";
  demo_server.desc = "Play back demos you have previously recorded";
  f_server_list.append(demo_server);

  return f_server_list;
}

QString AOApplication::read_design_ini(QString p_identifier,
                                       QString p_design_path)
{
  QSettings settings(p_design_path, QSettings::IniFormat);
  QVariant value = settings.value(p_identifier);
  if (value.type() == QVariant::StringList) {
    return value.toStringList().join(",");
  }
  else {
    return value.toString();
  }
}

Qt::TransformationMode AOApplication::get_scaling(QString p_scaling)
{
  if (p_scaling == "smooth")
    return Qt::SmoothTransformation;
  return Qt::FastTransformation;
}

QPoint AOApplication::get_button_spacing(QString p_identifier, QString p_file)
{
  QString design_ini_path = get_theme_path(p_file);
  QString default_path = get_default_theme_path(p_file);
  QString f_result = read_design_ini(p_identifier, design_ini_path);

  QPoint return_value;

  return_value.setX(0);
  return_value.setY(0);

  if (f_result == "") {
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

pos_size_type AOApplication::get_element_dimensions(QString p_identifier,
                                                    QString p_file,
                                                    QString p_char)
{
  pos_size_type return_value;
  return_value.x = 0;
  return_value.y = 0;
  return_value.width = -1;
  return_value.height = -1;
  QString f_result = get_design_element(p_identifier, p_file, p_char);

  QStringList sub_line_elements = f_result.split(",");

  if (sub_line_elements.size() < 4)
    return return_value;

  return_value.x = sub_line_elements.at(0).toInt();
  return_value.y = sub_line_elements.at(1).toInt();
  return_value.width = sub_line_elements.at(2).toInt();
  return_value.height = sub_line_elements.at(3).toInt();

  return return_value;
}
QString AOApplication::get_design_element(QString p_identifier, QString p_file,
                                          QString p_char)
{
  QStringList paths{get_theme_path("misc/" + get_chat(p_char) + "/" +
                                   p_file), // user theme overrides base/misc
                    get_base_path() + "misc/" + get_chat(p_char) + "/" + p_file,
                    get_theme_path(p_file), get_default_theme_path(p_file)};
  for (const QString &path : paths) {
    QString value = read_design_ini(p_identifier, path);
    if (!value.isEmpty())
      return value;
  }
  return "";
}
QString AOApplication::get_font_name(QString p_identifier, QString p_file)
{
  QString design_ini_path = get_theme_path(p_file);
  QString f_result = read_design_ini(p_identifier, design_ini_path);
  QString default_path = get_default_theme_path(p_file);
  if (f_result == "") {
    f_result = read_design_ini(p_identifier, default_path);
    if (f_result == "")
      return "";
  }
  return f_result;
}
int AOApplication::get_font_size(QString p_identifier, QString p_file)
{
  QString design_ini_path = get_theme_path(p_file);
  QString default_path = get_default_theme_path(p_file);
  QString f_result = read_design_ini(p_identifier, design_ini_path);

  if (f_result == "") {
    f_result = read_design_ini(p_identifier, default_path);

    if (f_result == "")
      return 10;
  }

  return f_result.toInt();
}

QColor AOApplication::get_color(QString p_identifier, QString p_file)
{
  QString design_ini_path = get_theme_path(p_file);
  QString default_path = get_default_theme_path(p_file);
  QString f_result = read_design_ini(p_identifier, design_ini_path);

  QColor return_color(0, 0, 0);

  if (f_result == "") {
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

QString AOApplication::get_stylesheet(QString p_file)
{
  QString design_ini_path = get_theme_path(p_file);
  QString default_path = get_default_theme_path(p_file);

  QFile design_ini;

  design_ini.setFileName(design_ini_path);

  if (!design_ini.open(QIODevice::ReadOnly)) {
    design_ini.setFileName(default_path);
    if (!design_ini.open(QIODevice::ReadOnly))
      return "";
  }

  QTextStream in(&design_ini);

  QString f_text;

  while (!in.atEnd()) {
    f_text.append(in.readLine());
  }

  design_ini.close();
  return f_text;
}

QString AOApplication::get_tagged_stylesheet(QString target_tag, QString p_file)
{
  QString design_ini_path = get_theme_path(p_file);

  QFile design_ini;

  design_ini.setFileName(design_ini_path);

  if (!design_ini.open(QIODevice::ReadOnly))
    return "";

  QTextStream in(&design_ini);

  QString f_text;

  bool tag_found = false;

  while (!in.atEnd()) {
    QString line = in.readLine();

    if (line.startsWith(target_tag, Qt::CaseInsensitive)) {
      tag_found = true;
      continue;
    }

    if (tag_found) {
      if ((line.startsWith("[") && line.endsWith("]")))
        break;
      f_text.append(line);
    }
  }

  design_ini.close();
  return f_text;
}

QString AOApplication::get_chat_markup(QString p_identifier, QString p_chat)
{
  QStringList paths{get_theme_path("misc/" + get_chat(p_chat) + "/config.ini"),
                    get_base_path() + "misc/" + get_chat(p_chat) +
                        "/config.ini",
                    get_base_path() + "misc/default/config.ini",
                    get_theme_path("misc/default/config.ini")};

  for (const QString &path : paths) {
    QString value = read_design_ini(p_identifier, path);
    if (!value.isEmpty()) {
      return value.toLatin1();
    }
  }

  return "";
}

QColor AOApplication::get_chat_color(QString p_identifier, QString p_chat)
{
  QColor return_color(255, 255, 255);
  QString f_result = get_chat_markup(p_identifier, p_chat);
  if (f_result == "")
    return return_color;

  QStringList color_list = f_result.split(",");

  if (color_list.size() < 3)
    return return_color;

  return_color.setRed(color_list.at(0).toInt());
  return_color.setGreen(color_list.at(1).toInt());
  return_color.setBlue(color_list.at(2).toInt());

  return return_color;
}

QString AOApplication::get_sfx(QString p_identifier, QString p_misc)
{
  QStringList paths{get_theme_path("misc/" + p_misc + "/courtroom_sounds.ini"),
                    get_misc_path(p_misc, "courtroom_sounds.ini"),
                    get_theme_path("courtroom_sounds.ini"),
                    get_default_theme_path("courtroom_sounds.ini")};

  QString return_sfx = "";

  for (const QString &path : paths) {
    QString value = read_design_ini(p_identifier, path);
    if (!value.isEmpty()) {
      return value.toLatin1();
    }
  }
  return return_sfx;
}

QString AOApplication::get_sfx_suffix(QString sound_to_check)
{
  if (file_exists(sound_to_check))
    return sound_to_check;
  if (file_exists(sound_to_check + ".opus"))
    return sound_to_check + ".opus";
  if (file_exists(sound_to_check + ".ogg"))
    return sound_to_check + ".ogg";
  if (file_exists(sound_to_check + ".mp3"))
    return sound_to_check + ".mp3";
  if (file_exists(sound_to_check + ".mp4"))
    return sound_to_check + ".mp4";
  return sound_to_check + ".wav";
}

QString AOApplication::get_image_suffix(QString path_to_check)
{
  if (file_exists(path_to_check))
    return path_to_check;
  if (file_exists(path_to_check + ".webp"))
    return path_to_check + ".webp";
  if (file_exists(path_to_check + ".apng"))
    return path_to_check + ".apng";
  if (file_exists(path_to_check + ".gif"))
    return path_to_check + ".gif";
  return path_to_check + ".png";
}

QString AOApplication::get_static_image_suffix(QString path_to_check)
{
  return path_to_check + ".png";
}

// returns whatever is to the right of "search_line =" within target_tag and
// terminator_tag, trimmed returns the empty string if the search line couldnt
// be found
QString AOApplication::read_char_ini(QString p_char, QString p_search_line,
                                     QString target_tag)
{
  QSettings settings(get_character_path(p_char, "char.ini"),
                     QSettings::IniFormat);
  settings.beginGroup(target_tag);
  QString value = settings.value(p_search_line).toString();
  settings.endGroup();
  return value;
}

void AOApplication::set_char_ini(QString p_char, QString value,
                                 QString p_search_line, QString target_tag)
{
  QSettings settings(get_character_path(p_char, "char.ini"),
                     QSettings::IniFormat);
  settings.beginGroup(target_tag);
  settings.setValue(p_search_line, value);
  settings.endGroup();
}

// returns all the values of target_tag
QStringList AOApplication::read_ini_tags(QString p_path, QString target_tag)
{
  QStringList r_values;
  QSettings settings(p_path, QSettings::IniFormat);
  if (!target_tag.isEmpty())
    settings.beginGroup(target_tag);
  QStringList keys = settings.allKeys();
  foreach (QString key, keys) {
    QString value = settings.value(key).toString();
    r_values << key + "=" + value;
  }
  if (!settings.group().isEmpty())
    settings.endGroup();
  return r_values;
}

QString AOApplication::get_char_name(QString p_char)
{
  QString f_result = read_char_ini(p_char, "name", "Options");

  if (f_result == "")
    return p_char;
  return f_result;
}

QString AOApplication::get_showname(QString p_char)
{
  QString f_result = read_char_ini(p_char, "showname", "Options");
  QString f_needed = read_char_ini(p_char, "needs_showname", "Options");

  if (f_needed.startsWith("false"))
    return "";
  if (f_result == "")
    return p_char;
  return f_result;
}

QString AOApplication::get_char_side(QString p_char)
{
  QString f_result = read_char_ini(p_char, "side", "Options");

  if (f_result == "")
    return "wit";
  return f_result;
}

QString AOApplication::get_blips(QString p_char)
{
  QString f_result = read_char_ini(p_char, "blips", "Options");

  if (f_result == "") {
    f_result = read_char_ini(p_char, "gender", "Options"); // not very PC, FanatSors
    if (f_result == "")
      f_result = "male";
  }

  if (!file_exists(get_sfx_suffix(get_sounds_path(f_result)))) {
    if (file_exists(get_sfx_suffix(get_sounds_path("../blips/" + f_result))))
      return "../blips/" + f_result; // Return the cool kids variant

    return "sfx-blip" + f_result; // Return legacy variant
  }
  return f_result;
}

QString AOApplication::get_emote_property(QString p_char, QString p_emote,
                                          QString p_property)
{
  QString f_result =
      read_char_ini(p_char, p_emote, p_property); // per-emote override
  if (f_result == "")
    f_result = read_char_ini(p_char, p_property,
                             "Options"); // global for this character
  return f_result;
}

Qt::TransformationMode AOApplication::get_misc_scaling(QString p_miscname)
{
  if (p_miscname != "") {
    QString misc_transform_mode = read_design_ini(
        "scaling", get_theme_path("misc/" + p_miscname + "/config.ini"));
    if (misc_transform_mode == "")
      misc_transform_mode =
          read_design_ini("scaling", get_misc_path(p_miscname, "config.ini"));
    if (misc_transform_mode == "smooth")
      return Qt::SmoothTransformation;
  }
  return Qt::FastTransformation;
}

QString AOApplication::get_category(QString p_char)
{
  QString f_result = read_char_ini(p_char, "category", "Options");
  return f_result;
}

QString AOApplication::get_chat(QString p_char)
{
  if (p_char == "default")
    return "default";
  QString f_result = read_char_ini(p_char, "chat", "Options");

  // handling the correct order of chat is a bit complicated, we let the caller
  // do it
  return f_result;
}

QString AOApplication::get_chat_font(QString p_char)
{
  QString f_result = read_char_ini(p_char, "chat_font", "Options");

  return f_result;
}

int AOApplication::get_chat_size(QString p_char)
{
  QString f_result = read_char_ini(p_char, "chat_size", "Options");

  if (f_result == "")
    return -1;
  return f_result.toInt();
}

QString AOApplication::get_char_shouts(QString p_char)
{
  QString f_result = read_char_ini(p_char, "shouts", "Options");
  if (f_result == "")
    return current_theme; // The default option is the current theme.
  return f_result;
}

int AOApplication::get_preanim_duration(QString p_char, QString p_emote)
{
  QString f_result = read_char_ini(p_char, p_emote, "Time");

  if (f_result == "")
    return -1;
  return f_result.toInt();
}

int AOApplication::get_ao2_preanim_duration(QString p_char, QString p_emote)
{
  QString f_result = read_char_ini(p_char, "%" + p_emote, "Time");

  if (f_result == "")
    return -1;
  return f_result.toInt();
}

int AOApplication::get_emote_number(QString p_char)
{
  QString f_result = read_char_ini(p_char, "number", "Emotions");

  if (f_result == "")
    return 0;
  return f_result.toInt();
}

QString AOApplication::get_emote_comment(QString p_char, int p_emote)
{
  QString f_result =
      read_char_ini(p_char, QString::number(p_emote + 1), "Emotions");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4) {
    qDebug() << "W: misformatted char.ini: " << p_char << ", " << p_emote;
    return "normal";
  }
  return result_contents.at(0);
}

QString AOApplication::get_pre_emote(QString p_char, int p_emote)
{
  QString f_result =
      read_char_ini(p_char, QString::number(p_emote + 1), "Emotions");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4) {
    qDebug() << "W: misformatted char.ini: " << p_char << ", " << p_emote;
    return "";
  }
  return result_contents.at(1);
}

QString AOApplication::get_emote(QString p_char, int p_emote)
{
  QString f_result =
      read_char_ini(p_char, QString::number(p_emote + 1), "Emotions");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4) {
    qDebug() << "W: misformatted char.ini: " << p_char << ", " << p_emote;
    return "normal";
  }
  return result_contents.at(2);
}

int AOApplication::get_emote_mod(QString p_char, int p_emote)
{
  QString f_result =
      read_char_ini(p_char, QString::number(p_emote + 1), "Emotions");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4) {
    qDebug() << "W: misformatted char.ini: " << p_char << ", "
             << QString::number(p_emote);
    return 0;
  }
  return result_contents.at(3).toInt();
}

int AOApplication::get_desk_mod(QString p_char, int p_emote)
{
  QString f_result =
      read_char_ini(p_char, QString::number(p_emote + 1), "Emotions");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 5)
    return -1;

  QString string_result = result_contents.at(4);
  if (string_result == "")
    return -1;

  return string_result.toInt();
}

QString AOApplication::get_sfx_name(QString p_char, int p_emote)
{
  QString f_result =
      read_char_ini(p_char, QString::number(p_emote + 1), "SoundN");

  if (f_result == "")
    return "1";
  return f_result;
}

QString AOApplication::get_emote_blip(QString p_char, int p_emote)
{
  QString f_result =
      read_char_ini(p_char, QString::number(p_emote + 1), "SoundB");
  return f_result;
}

int AOApplication::get_sfx_delay(QString p_char, int p_emote)
{
  QString f_result =
      read_char_ini(p_char, QString::number(p_emote + 1), "SoundT");

  if (f_result == "")
    return 1;
  return f_result.toInt();
}

QString AOApplication::get_sfx_looping(QString p_char, int p_emote)
{
  QString f_result =
      read_char_ini(p_char, QString::number(p_emote + 1), "SoundL");

  qDebug() << f_result;
  if (f_result == "")
    return "0";
  else
    return f_result;
}

QString AOApplication::get_sfx_frame(QString p_char, QString p_emote,
                                     int n_frame)
{
  QString f_result = read_char_ini(p_char, QString::number(n_frame),
                                   p_emote.append("_FrameSFX"));

  if (f_result == "")
    return "";
  return f_result;
}

QString AOApplication::get_screenshake_frame(QString p_char, QString p_emote,
                                             int n_frame)
{
  QString f_result = read_char_ini(p_char, QString::number(n_frame),
                                   p_emote.append("_FrameScreenshake"));

  if (f_result == "")
    return "";
  return f_result;
}

QString AOApplication::get_flash_frame(QString p_char, QString p_emote,
                                       int n_frame)
{
  QString f_result = read_char_ini(p_char, QString::number(n_frame),
                                   p_emote.append("_FrameRealization"));

  if (f_result == "")
    return "";
  return f_result;
}

int AOApplication::get_text_delay(QString p_char, QString p_emote)
{
  QString f_result = read_char_ini(p_char, p_emote, "stay_time");

  if (f_result == "")
    return -1;
  return f_result.toInt();
}

QStringList AOApplication::get_theme_effects()
{
  QString p_path = get_theme_path("effects/effects.ini");
  QString default_path = get_default_theme_path("effects/effects.ini");

  QStringList effects;
  if (!file_exists(p_path)) {
    p_path = default_path;
    if (!file_exists(p_path))
      return effects;
  }

  QStringList lines = read_file(p_path).split("\n");
  foreach (QString effect, lines) {
    effect = effect.split("=")[0].trimmed().split("_")[0];
    if (!effect.isEmpty() && !effects.contains(effect))
      effects.append(effect);
  }
  return effects;
}

QStringList AOApplication::get_effects(QString p_char)
{
  QString p_effect = read_char_ini(p_char, "effects", "Options");
  QString p_path = get_base_path() + "misc/" + p_effect + "/effects.ini";

  QStringList effects = get_theme_effects();
  if (!file_exists(p_path))
    return effects;

  QStringList lines = read_file(p_path).split("\n");
  foreach (QString effect, lines) {
    effect = effect.split("=")[0].trimmed().split("_")[0];
    if (!effect.isEmpty() && !effects.contains(effect))
      effects.append(effect);
  }

  return effects;
}

QString AOApplication::get_effect(QString effect, QString p_char,
                                  QString p_folder)
{
  QString p_effect = p_folder;
  if (p_folder == "")
    p_effect = read_char_ini(p_char, "effects", "Options");

  QString p_path =
      get_image_suffix(get_base_path() + "misc/" + p_effect + "/" + effect);
  QString design_ini_path =
      get_image_suffix(get_theme_path("effects/" + effect));
  QString default_path =
      get_image_suffix(get_default_theme_path("effects/" + effect));

  if (!file_exists(p_path)) {
    p_path = design_ini_path;
    if (!file_exists(p_path)) {
      p_path = default_path;
      if (!file_exists(p_path)) {
        return "";
      }
    }
  }

  return p_path;
}

QString AOApplication::get_effect_property(QString fx_name, QString p_char,
                                           QString p_property)
{
  QString f_property;
  if (p_property == "sound")
    f_property = fx_name;
  else
    f_property = fx_name + "_" + p_property;
  QString p_effect = read_char_ini(p_char, "effects", "Options");
  QString p_path = get_base_path() + "misc/" + p_effect + "/effects.ini";
  QString design_ini_path = get_theme_path("effects/effects.ini");
  QString default_path = get_default_theme_path("effects/effects.ini");

  QString f_result = read_design_ini(f_property, p_path);
  if (f_result == "") {
    f_result = read_design_ini(f_property, design_ini_path);
    if (f_result == "") {
      f_result = read_design_ini(f_property, default_path);
    }
  }

  if (fx_name == "realization" && p_property == "sound") {
    f_result = get_custom_realization(p_char);
  }

  qDebug() << "got" << f_property << "of" << fx_name << "==" << f_result;

  return f_result;
}

QString AOApplication::get_custom_realization(QString p_char)
{
  QString f_result = read_char_ini(p_char, "realization", "Options");

  if (f_result == "")
    return get_sfx("realization");
  else
    return get_sfx_suffix(get_sounds_path(f_result));
}

bool AOApplication::get_blank_blip()
{
  QString result = configini->value("blank_blip", "false").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::get_looping_sfx()
{
  QString result = configini->value("looping_sfx", "true").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::objection_stop_music()
{
  QString result =
      configini->value("objection_stop_music", "false").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::is_instant_objection_enabled()
{
  QString result = configini->value("instant_objection", "true").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::is_desyncrhonized_logs_enabled()
{
  QString result = configini->value("desync_logs", "false").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::is_discord_enabled()
{
  QString result = configini->value("discord", "true").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::is_shake_enabled()
{
  QString result = configini->value("shake", "true").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::is_effects_enabled()
{
  QString result = configini->value("effects", "true").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::is_frame_network_enabled()
{
  QString result = configini->value("framenetwork", "true").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::is_colorlog_enabled()
{
  QString result = configini->value("colorlog", "true").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::is_stickysounds_enabled()
{
  QString result = configini->value("stickysounds", "false").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::is_stickyeffects_enabled()
{
  QString result = configini->value("stickyeffects", "false").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::is_stickypres_enabled()
{
  QString result = configini->value("stickypres", "false").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::is_customchat_enabled()
{
  QString result = configini->value("customchat", "true").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::is_continuous_enabled()
{
  QString result = configini->value("continuous_playback", "true").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::get_casing_enabled()
{
  QString result = configini->value("casing_enabled", "false").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::get_casing_defence_enabled()
{
  QString result =
      configini->value("casing_defence_enabled", "false").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::get_casing_prosecution_enabled()
{
  QString result =
      configini->value("casing_prosecution_enabled", "false").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::get_casing_judge_enabled()
{
  QString result =
      configini->value("casing_judge_enabled", "false").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::get_casing_juror_enabled()
{
  QString result =
      configini->value("casing_juror_enabled", "false").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::get_casing_steno_enabled()
{
  QString result =
      configini->value("casing_steno_enabled", "false").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::get_casing_cm_enabled()
{
  QString result =
      configini->value("casing_cm_enabled", "false").value<QString>();
  return result.startsWith("true");
}

QString AOApplication::get_casing_can_host_cases()
{
  QString result =
      configini->value("casing_can_host_cases", "Turnabout Check Your Settings")
          .value<QString>();
  return result;
}
bool AOApplication::get_auto_logging_enabled()
{
  QString result =
      configini->value("automatic_logging_enabled", "true").value<QString>();
  return result.startsWith("true");
}
