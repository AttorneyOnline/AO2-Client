#include "text_file_functions.h"
#include "aoutils.h"

QString AOApplication::read_theme()
{
  QString result = configini->value("theme", "default").value<QString>();
  return result;
}

int AOApplication::read_blip_rate()
{
  int result = configini->value("blip_rate", 2).toInt();

  if (result < 0)
    return 0;

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

int AOApplication::get_default_suppress_audio()
{
  int result = configini->value("suppress_audio", 50).toInt();
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

int AOApplication::get_text_crawl()
{
  int result = configini->value("text_crawl", 40).toInt();
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

QString AOApplication::get_log_timestamp_format()
{
  QString result = configini->value("log_timestamp_format", "h:mm:ss AP").value<QString>();
  return result;
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

QString AOApplication::get_default_showname()
{
    QString result = configini->value("default_showname", "").value<QString>();
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

QStringList AOApplication::get_list_file(VPath path)
{
  return get_list_file(get_real_path(path));
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
  if (filename.isEmpty())
    return QString();

  QFile f_log(filename);

  if (!f_log.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "Couldn't open" << filename << "for reading";
    return QString();
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

    out.setCodec("UTF-8");
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

    out.setCodec("UTF-8");
    out << "\r\n" << p_text;

    f_log.flush();
    f_log.close();
    return true;
  }
  return false;
}

QVector<server_type> AOApplication::read_favorite_servers()
{
  QVector<server_type> serverlist;

  // demo server is always at the top
  server_type demo_server;
  demo_server.ip = "127.0.0.1";
  demo_server.port = 99999;
  demo_server.name = tr("Demo playback");
  demo_server.desc = tr("Play back demos you have previously recorded");
  serverlist.append(demo_server);

  QString fav_servers_ini_path(get_base_path() + "favorite_servers.ini");
  if (!QFile::exists(fav_servers_ini_path)) {
    qWarning() << "failed to locate favorite_servers.ini, falling back to legacy serverlist.txt";
    serverlist += read_legacy_favorite_servers();
  }
  else {
    QSettings fav_servers_ini(fav_servers_ini_path, QSettings::IniFormat);
    fav_servers_ini.setIniCodec("UTF-8");

    auto grouplist = fav_servers_ini.childGroups();
    { // remove all negative and non-numbers
      auto filtered_grouplist = grouplist;
      for (const QString &group : qAsConst(grouplist)) {
        bool ok = false;
        const int l_num = group.toInt(&ok);
        if (ok && l_num >= 0) {
          continue;
        }
        filtered_grouplist.append(group);
      }
      std::sort(filtered_grouplist.begin(), filtered_grouplist.end(), [](const auto &a, const auto &b) -> bool {
        return a.toInt() < b.toInt();
      });
      grouplist = std::move(filtered_grouplist);
    }

    for(const QString &group: qAsConst(grouplist)) {
      server_type f_server;
      fav_servers_ini.beginGroup(group);
      f_server.ip = fav_servers_ini.value("address", "127.0.0.1").toString();
      f_server.port = fav_servers_ini.value("port", 27016).toInt();
      f_server.name = fav_servers_ini.value("name", "Missing Name").toString();
      f_server.desc = fav_servers_ini.value("desc", "No description").toString();
      f_server.socket_type = to_connection_type.value(fav_servers_ini.value("protocol", "tcp").toString());
      serverlist.append(std::move(f_server));
      fav_servers_ini.endGroup();
    }
  }

  return serverlist;
}

QVector<server_type> AOApplication::read_legacy_favorite_servers()
{
  QVector<server_type> serverlist;

  QFile serverlist_txt(get_base_path() + "serverlist.txt");
  if (!serverlist_txt.exists()) {
    qWarning() << "serverlist.txt does not exist";
  } else if (!serverlist_txt.open(QIODevice::ReadOnly)) {
    qWarning() << "failed to open serverlist.txt";
  } else {
    QTextStream stream(&serverlist_txt);
    stream.setCodec("UTF-8");

    while (!stream.atEnd())
    {
      QStringList contents = stream.readLine().split(":");

      int item_count = contents.size();
      if (item_count < 3 || item_count > 4) {
        continue;
      }

      server_type server;
      server.ip = contents.at(0);
      server.port = contents.at(1).toInt();
      server.name = contents.at(2);
      if (item_count == 4) {
        server.socket_type = connection_type(contents.at(3).toInt());
      } else {
        server.socket_type = TCP;
      }
      serverlist.append(std::move(server));
    }
    serverlist_txt.close();
  }

  return serverlist;
}

QString AOApplication::read_design_ini(QString p_identifier,
                                       VPath p_design_path)
{
  return read_design_ini(p_identifier, get_real_path(p_design_path));
}

QString AOApplication::read_design_ini(QString p_identifier,
                                       QString p_design_path)
{
  QSettings settings(p_design_path, QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  QVariant value = settings.value(p_identifier);
  if (value.type() == QVariant::StringList) {
    return value.toStringList().join(",");
  }
  else if (!value.isNull()) {
    return value.toString();
  }
  return "";
}

Qt::TransformationMode AOApplication::get_scaling(QString p_scaling)
{
  if (p_scaling.isEmpty())
    p_scaling = get_default_scaling();

  if (p_scaling == "smooth")
    return Qt::SmoothTransformation;
  return Qt::FastTransformation;
}

QPoint AOApplication::get_button_spacing(QString p_identifier, QString p_file)
{
  QString value = get_config_value(p_identifier, p_file, current_theme, get_subtheme(), default_theme);
  QPoint return_value;

  return_value.setX(0);
  return_value.setY(0);

  if (value == "")
    return return_value;

  QStringList sub_line_elements = value.split(",");

  if (sub_line_elements.size() < 2)
    return return_value;

  return_value.setX(sub_line_elements.at(0).toInt());
  return_value.setY(sub_line_elements.at(1).toInt());

  return return_value;
}

pos_size_type AOApplication::get_element_dimensions(QString p_identifier,
                                                    QString p_file,
                                                    QString p_misc)
{
  pos_size_type return_value;
  return_value.x = 0;
  return_value.y = 0;
  return_value.width = -1;
  return_value.height = -1;
  QString f_result = get_design_element(p_identifier, p_file, p_misc);

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
                                          QString p_misc)
{
  QString value = get_config_value(p_identifier, p_file, current_theme, get_subtheme(), default_theme, p_misc);
  if (!value.isEmpty())
    return value;
  return "";
}

// tfw this function is only used for lobby and nowhere else
int AOApplication::get_font_size(QString p_identifier, QString p_file)
{
  QString value = get_config_value(p_identifier, p_file, current_theme, get_subtheme(), default_theme);
  if (!value.isEmpty())
    return value.toInt();
  return 10;
}

QColor AOApplication::get_color(QString p_identifier, QString p_file)
{
  QString value = get_config_value(p_identifier, p_file, current_theme, get_subtheme(), default_theme);
  QColor return_color(0, 0, 0);

  if (value.isEmpty())
    return return_color;

  QStringList color_list = value.split(",");

  if (color_list.size() < 3)
    return return_color;

  return_color.setRed(color_list.at(0).toInt());
  return_color.setGreen(color_list.at(1).toInt());
  return_color.setBlue(color_list.at(2).toInt());

  return return_color;
}

QString AOApplication::get_stylesheet(QString p_file)
{
  QString path = get_asset(p_file, current_theme, get_subtheme(), default_theme);
  QFile design_ini;
  design_ini.setFileName(path);
  if (!design_ini.open(QIODevice::ReadOnly))
    return "";

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
  QString path = get_asset(p_file, current_theme, get_subtheme(), default_theme);
  QFile design_ini;
  design_ini.setFileName(path);
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
  // New Chadly method
  QString value = get_config_value(p_identifier, "chat_config.ini", current_theme, get_subtheme(), default_theme, p_chat);
  if (!value.isEmpty())
    return value.toUtf8();

  // Backwards ass compatibility
  QVector<VPath> backwards_paths {
    get_theme_path("misc/" + p_chat + "/config.ini"),
    VPath("misc/" + p_chat + "/config.ini"),
    get_theme_path("misc/default/config.ini"),
    VPath("misc/default/config.ini")
  };

  for (const VPath &p : backwards_paths) {
    QString value = read_design_ini(p_identifier, p);
    if (!value.isEmpty()) {
      return value.toUtf8();
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

QString AOApplication::get_penalty_value(QString p_identifier)
{
  return get_config_value(p_identifier, "penalty/penalty.ini", current_theme,
                          get_subtheme(), default_theme, "");
}


QString AOApplication::get_court_sfx(QString p_identifier, QString p_misc)
{
  QString value = get_config_value(p_identifier, "courtroom_sounds.ini", current_theme, get_subtheme(), default_theme, p_misc);
  if (!value.isEmpty())
    return value.toUtf8();
  return "";
}

QString AOApplication::get_sfx_suffix(VPath sound_to_check)
{
  QStringList suffixes = {".opus", ".ogg", ".mp3", ".wav", ".mid", ".midi", ".xm", ".it", ".s3m", ".mod", ".mtm", ".umx" };
  // Check if we were provided a direct filepath with a suffix already
  QString path = sound_to_check.toQString();
  // Loop through our suffixes
  for (const QString &suffix : suffixes) {
    // If our VPath ends with a valid suffix
    if (path.endsWith(suffix, Qt::CaseInsensitive))
      // Return that as the path
      return get_real_path(sound_to_check);
  }
  // Otherwise, ignore the provided suffix and check our own
  return get_real_path(sound_to_check, suffixes);
}

QString AOApplication::get_image_suffix(VPath path_to_check, bool static_image)
{
  QStringList suffixes {};
  if (!static_image) {
    suffixes.append({ ".webp", ".apng", ".gif" });
  }
  suffixes.append(".png");

  // Check if we were provided a direct filepath with a suffix already
  QString path = path_to_check.toQString();
  // Loop through our suffixes
  for (const QString &suffix : suffixes) {
    // If our VPath ends with a valid suffix
    if (path.endsWith(suffix, Qt::CaseInsensitive))
      // Return that as the path
      return get_real_path(path_to_check);
  }
  // Otherwise, ignore the provided suffix and check our own
  return get_real_path(path_to_check, suffixes);
}

// returns whatever is to the right of "search_line =" within target_tag and
// terminator_tag, trimmed returns the empty string if the search line couldnt
// be found
QString AOApplication::read_char_ini(QString p_char, QString p_search_line,
                                     QString target_tag)
{
  QSettings settings(get_real_path(get_character_path(p_char, "char.ini")),
                     QSettings::IniFormat);
  settings.beginGroup(target_tag);
  settings.setIniCodec("UTF-8");
  QString value = settings.value(p_search_line).value<QString>();
  settings.endGroup();
  return value;
}

void AOApplication::set_char_ini(QString p_char, QString value,
                                 QString p_search_line, QString target_tag)
{
  QSettings settings(get_real_path(get_character_path(p_char, "char.ini")),
                     QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(target_tag);
  settings.setValue(p_search_line, value);
  settings.endGroup();
}

// returns all the values of target_tag
QStringList AOApplication::read_ini_tags(VPath p_path, QString target_tag)
{
  QStringList r_values;
  QSettings settings(get_real_path(p_path), QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  if (!target_tag.isEmpty())
    settings.beginGroup(target_tag);
  QStringList keys = settings.allKeys();
  foreach (QString key, keys) {
    QString value = settings.value(key).value<QString>();
    r_values << key + "=" + value;
  }
  if (!settings.group().isEmpty())
    settings.endGroup();
  return r_values;
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

int AOApplication::get_preanim_duration(QString p_char, QString p_emote)
{
  QString f_result = read_char_ini(p_char, p_emote, "Time");

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
    qWarning() << "misformatted char.ini: " << p_char << ", " << p_emote;
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
    qWarning() << "misformatted char.ini: " << p_char << ", " << p_emote;
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
    qWarning() << "misformatted char.ini: " << p_char << ", " << p_emote;
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
    qWarning() << "misformatted char.ini: " << p_char << ", "
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

QStringList AOApplication::get_effects(QString p_char)
{
  const QStringList l_filepath_list{
    get_asset("effects/effects.ini", current_theme, get_subtheme(), default_theme, ""),
    get_asset("effects.ini", current_theme, get_subtheme(), default_theme, read_char_ini(p_char, "effects", "Options")),
  };

  QStringList l_effect_name_list;
  for (const QString &i_filepath : l_filepath_list)
  {
    if (!QFile::exists(i_filepath))
    {
      continue;
    }

    QSettings l_effects_ini(i_filepath, QSettings::IniFormat);
    l_effects_ini.setIniCodec("UTF-8");

         // port legacy effects
    if (!l_effects_ini.contains("version/major") || l_effects_ini.value("version/major").toInt() < 2)
    {
      QFile effects_old(i_filepath);
      if (QFile::copy(i_filepath, i_filepath + ".old")) {
        AOUtils::migrateEffects(l_effects_ini);
      }
      else {
        qWarning() << "Unable to copy effects.ini, skipping migration.";
      }
    }

    QStringList l_group_list;
    for (const QString &i_group : l_effects_ini.childGroups())
    {
      bool l_result;
      i_group.toInt(&l_result);
      if (l_result)
      {
        l_group_list.append(i_group);
      }
    }

    std::sort(l_group_list.begin(), l_group_list.end(), [](const QString &lhs, const QString &rhs) {
      return lhs.toInt() < rhs.toInt();
    });

    for (const QString &i_group : qAsConst(l_group_list))
    {
      const QString l_key = i_group + "/name";
      if (!l_effects_ini.contains(l_key))
      {
        continue;
      }

      const QString l_effect_name = l_effects_ini.value(l_key).toString();
      if (l_effect_name.isEmpty())
      {
        continue;
      }

      l_effect_name_list.append(l_effect_name);
    }
  }
  return l_effect_name_list;
}

QString AOApplication::get_effect(QString effect, QString p_char,
                                  QString p_folder)
{
  if (p_folder == "")
    p_folder = read_char_ini(p_char, "effects", "Options");

  QStringList paths {
    get_image("effects/" + effect, current_theme, get_subtheme(), default_theme, ""),
    get_image(effect, current_theme, get_subtheme(), default_theme, p_folder)
  };

  for (const auto &p : paths)
    if (file_exists(p))
      return p;
  return {};
}

QString AOApplication::get_effect_property(QString fx_name, QString p_char,
                                           QString p_folder, QString p_property)
{
  if (p_folder == "")
    p_folder = read_char_ini(p_char, "effects", "Options");

  const auto paths = get_asset_paths("effects/effects.ini", current_theme, get_subtheme(), default_theme, "");
  const auto misc_paths = get_asset_paths("effects.ini", current_theme, get_subtheme(), default_theme, p_folder);
  QString path;
  QString f_result;
  for (const VPath &p : paths + misc_paths) {
    path = get_real_path(p);
    if (!path.isEmpty()) {
      QSettings settings(path, QSettings::IniFormat);
      settings.setIniCodec("UTF-8");
      QStringList char_effects = settings.childGroups();
      for (int i = 0; i < char_effects.size(); ++i) {
        QString effect = settings.value(char_effects[i] + "/name").toString();
        if (effect.toLower() == fx_name.toLower()) {
          f_result = settings.value(char_effects[i] + "/" + p_property).toString();
          if (!f_result.isEmpty()) {
            // Only break the loop if we get a non-empty result, continue the search otherwise
            break;
          }
        }
      }
    }
  }
  if (fx_name == "realization" && p_property == "sound") {
    f_result = get_custom_realization(p_char);
  }
  return f_result;
}

QString AOApplication::get_custom_realization(QString p_char)
{
  QString f_result = read_char_ini(p_char, "realization", "Options");
  if (f_result == "")
    return get_court_sfx("realization");
  return get_sfx_suffix(get_sounds_path(f_result));
}

bool AOApplication::get_pos_is_judge(const QString &p_pos)
{
  QStringList positions = read_design_ini("judges", get_background_path("design.ini")).split(",");
  if (positions.size() == 1 && positions[0] == "") {
      return p_pos == "jud"; //Hardcoded BS only if we have no judges= defined
  }
  return positions.contains(p_pos.trimmed());
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

bool AOApplication::is_streaming_disabled()
{
    return configini->value("streaming_disabled", false).toBool();
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

bool AOApplication::is_sticker_enabled()
{
  QString result = configini->value("sticker", "true").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::is_continuous_enabled()
{
  QString result = configini->value("continuous_playback", "true").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::is_category_stop_enabled()
{
  QString result = configini->value("category_stop", "true").value<QString>();
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

bool AOApplication::get_text_logging_enabled()
{
  QString result =
      configini->value("automatic_logging_enabled", "true").value<QString>();
  return result.startsWith("true");
}

bool AOApplication::get_demo_logging_enabled()
{
  QString result =
      configini->value("demo_logging_enabled", "true").value<QString>();
  return result.startsWith("true");
}

QString AOApplication::get_subtheme()
{
  QString result =
      configini->value("subtheme", "server").value<QString>();
  // Server means we want the server to decide for us
  if (result == "server")
      // 'subtheme' variable is affected by the server
      result = subtheme;
  // Default means we don't want any subthemes
  else if (result == "default")
      result = "";
  return result;
}

bool AOApplication::get_animated_theme()
{
  QString result =
      configini->value("animated_theme", "true").value<QString>();
  return result.startsWith("true");
}

QString AOApplication::get_default_scaling()
{
  return configini->value("default_scaling", "fast").value<QString>();
}

QStringList AOApplication::get_mount_paths()
{
  return configini->value("mount_paths").value<QStringList>();
}

bool AOApplication::get_player_count_optout()
{
  return configini->value("player_count_optout", "false").value<QString>()
      .startsWith("true");
}

bool AOApplication::get_sfx_on_idle()
{
  return configini->value("sfx_on_idle", "false").value<QString>()
      .startsWith("true");
}

bool AOApplication::get_evidence_double_click()
{
  return configini->value("evidence_double_click", "true").value<QString>()
      .startsWith("true");
}
