#include "lobby.h"

#include "aoapplication.h"
#include "aosfxplayer.h"
#include "debug_functions.h"
#include "demoserver.h"
#include "networkmanager.h"

#include <QImageReader>

Lobby::Lobby(AOApplication *p_ao_app) : QMainWindow()
{
  ao_app = p_ao_app;

  this->setWindowTitle(tr("Attorney Online %1").arg(ao_app->applicationVersion()));
  this->setWindowIcon(QIcon(":/logo.png"));
  this->setWindowFlags( (this->windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);

  ui_loading_background = new AOImage(this, ao_app);
  ui_loading_background->setObjectName("ui_loading_background");
  ui_loading_text = new QTextEdit(ui_loading_background);
  ui_loading_text->setObjectName("ui_loading_text");
  ui_progress_bar = new QProgressBar(ui_loading_background);
  ui_progress_bar->setMinimum(0);
  ui_progress_bar->setMaximum(100);
  ui_progress_bar->setObjectName("ui_progress_bar");

  set_widgets();

  server_type f_server = ao_app->read_server_ini();
  if (f_server.ip.isEmpty()) {
    QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
  }
  ao_app->net_manager->connect_to_server(f_server);
  begin_connection();

}

// sets images, position and size
void Lobby::set_widgets()
{
  ao_app->reload_theme();

  QString filename = "lobby_design.ini";

  pos_size_type f_lobby = ao_app->get_element_dimensions("lobby", filename);

  if (f_lobby.width < 0 || f_lobby.height < 0) {
    qWarning() << "did not find lobby width or height in " << filename;

    #ifdef ANDROID
    if(QtAndroid::checkPermission("android.permission.READ_EXTERNAL_STORAGE")==QtAndroid::PermissionResult::Denied) {
        QtAndroid::requestPermissionsSync({"android.permission.READ_EXTERNAL_STORAGE","android.permission.WRITE_EXTERNAL_STORAGE"});
    }
    #endif

    // Most common symptom of bad config files and missing assets.
    call_notice(
        tr("It doesn't look like your client is set up correctly.\n"
           "Did you download all resources correctly from tiny.cc/getao, "
           "including the large 'base' folder?"));

    this->setFixedSize(517, 666);
  }
  else {
    this->setFixedSize(f_lobby.width, f_lobby.height);
  }

  ui_loading_background->resize(this->width(), this->height());
  ui_loading_background->set_image("loadingbackground");

  set_size_and_pos(ui_loading_text, "loading_label");
  ui_loading_text->setFont(QFont("Arial", 20, QFont::Bold));
  ui_loading_text->setReadOnly(true);
  ui_loading_text->setAlignment(Qt::AlignCenter);
  ui_loading_text->setFrameStyle(QFrame::NoFrame);
  ui_loading_text->append(tr("Loading"));

  set_size_and_pos(ui_progress_bar, "progress_bar");

  ui_loading_background->show();

  set_fonts();
  set_stylesheets();
}

void Lobby::set_size_and_pos(QWidget *p_widget, QString p_identifier)
{
  QString filename = "lobby_design.ini";

  pos_size_type design_ini_result =
      ao_app->get_element_dimensions(p_identifier, filename);

  if (design_ini_result.width < 0 || design_ini_result.height < 0) {
    qWarning() << "could not find" << p_identifier << "in" << filename;
    p_widget->hide();
  }
  else {
    p_widget->move(design_ini_result.x, design_ini_result.y);
    p_widget->resize(design_ini_result.width, design_ini_result.height);
  }
}

void Lobby::set_fonts() { set_font(ui_loading_text, "loading_text"); }

void Lobby::set_stylesheet(QWidget *widget)
{
  QString f_file = "lobby_stylesheets.css";
  QString style_sheet_string = ao_app->get_stylesheet(f_file);
  if (style_sheet_string != "")
    widget->setStyleSheet(style_sheet_string);
}

void Lobby::set_stylesheets()
{
  set_stylesheet(this);
  this->setStyleSheet(
    "QFrame { background-color:transparent; } "
    "QAbstractItemView { background-color: transparent; color: black; } "
    "QLineEdit { background-color:transparent; }"
    + this->styleSheet()
  );
}

void Lobby::set_font(QWidget *widget, QString p_identifier)
{
  QString design_file = "lobby_fonts.ini";
  int f_weight = ao_app->get_font_size(p_identifier, design_file);
  QString class_name = widget->metaObject()->className();
  QString font_name =
      ao_app->get_design_element(p_identifier + "_font", design_file);
  QFont font(font_name, f_weight);
  bool use = ao_app->get_font_size("use_custom_fonts", design_file) == 1;
  if (use) {
    bool bold = ao_app->get_font_size(p_identifier + "_bold", design_file) ==
                1; // is the font bold or not?
    font.setBold(bold);
    widget->setFont(font);
    QColor f_color = ao_app->get_color(p_identifier + "_color", design_file);
    bool center =
        ao_app->get_font_size(p_identifier + "_center", design_file) ==
        1; // should it be centered?
    QString is_center = "";
    if (center)
      is_center = "qproperty-alignment: AlignCenter;";
    QString style_sheet_string =
        class_name + " { background-color: rgba(0, 0, 0, 0);\n" +
        "color: rgba(" + QString::number(f_color.red()) + ", " +
        QString::number(f_color.green()) + ", " +
        QString::number(f_color.blue()) + ", 255);\n" + is_center + "}";
    widget->setStyleSheet(style_sheet_string);
  }
  return;
}

void Lobby::set_loading_text(QString p_text)
{
  ui_loading_text->clear();
  ui_loading_text->setAlignment(Qt::AlignCenter);
  ui_loading_text->append(p_text);
}

void Lobby::set_loading_value(int p_value)
{
  ui_progress_bar->setValue(p_value);
}

void Lobby::begin_connection()
{
  AOPacket *f_packet;

  f_packet = new AOPacket("askchaa#%");

  ao_app->send_server_packet(f_packet);
}

void Lobby::on_about_clicked()
{
  const bool hasApng = QImageReader::supportedImageFormats().contains("apng");

  QString msg =
      tr("<h2>Attorney Online %1</h2>"
         "The courtroom drama simulator."
         "<p><b>Source code:</b> "
         "<a href='https://github.com/AttorneyOnline/AO2-Client'>"
         "https://github.com/AttorneyOnline/AO2-Client</a>"
         "<p><b>Major development:</b><br>"
         "OmniTroid, stonedDiscord, longbyte1, gameboyprinter, Cerapter, "
         "Crystalwarrior, Iamgoofball, in1tiate"
         "<p><b>Client development:</b><br>"
         "Cents02, windrammer, skyedeving"
         "<p><b>QA testing:</b><br>"
         "CaseyCazy, CedricDewitt, Chewable Tablets, CrazyJC, Fantos, "
         "Fury McFlurry, Geck, Gin-Gi, Jamania, Minx, Pandae, "
         "Robotic Overlord, Shadowlions (aka Shali), Sierra, SomeGuy, "
         "Veritas, Wiso"
         "<p><b>Translations:</b><br>"
         "k-emiko (Русский), Pyraq (Polski), scatterflower (日本語), vintprox (Русский), "
         "windrammer (Español, Português)"
         "<p><b>Special thanks:</b><br>"
         "CrazyJC (2.8 release director) and MaximumVolty (2.8 release promotion); "
         "Remy, Hibiki, court-records.net (sprites); Qubrick (webAO); "
         "Rue (website); Draxirch (UI design); "
         "Lewdton and Argoneus (tsuserver); "
         "Fiercy, Noevain, Cronnicossy, and FanatSors (AO1); "
         "server hosts, game masters, case makers, content creators, "
         "and the whole AO2 community!"
         "<p>The Attorney Online networked visual novel project "
         "is copyright (c) 2016-2021 Attorney Online developers. Open-source "
         "licenses apply. All other assets are the property of their "
         "respective owners."
         "<p>Running on Qt version %2 with the BASS audio engine.<br>"
         "APNG plugin loaded: %3"
         "<p>Built on %4")
      .arg(ao_app->get_version_string())
      .arg(QLatin1String(QT_VERSION_STR))
      .arg(hasApng ? tr("Yes") : tr("No"))
      .arg(QLatin1String(__DATE__));
  QMessageBox::about(this, tr("About"), msg);
}

Lobby::~Lobby() {}
