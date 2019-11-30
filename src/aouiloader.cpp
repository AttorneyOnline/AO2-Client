#include "widgets/aocharmovie.h"
#include "widgets/aoevidencedisplay.h"
#include "widgets/aoimage.h"
#include "widgets/aomovie.h"
#include "widgets/aoscene.h"
#include "widgets/aoserverchat.h"
#include "widgets/aotextedit.h"
#include "widgets/aoviewport.h"
#include "aouiloader.h"

#include <QMessageBox>

QWidget *AOUiLoader::createWidget(const QString &className, QWidget *parent, const QString &name)
{
  QWidget *widget;

  if (0); // (a necessary sacrifice for a greater good... muhehe)
#define AO_WIDGET(class, ...) \
  else if (className == #class) \
  { \
  widget = new class(parent, ##__VA_ARGS__); \
  }

  AO_WIDGET(AOImage, ao_app)
  AO_WIDGET(AOTextEdit)
  AO_WIDGET(AOScene, ao_app)
  AO_WIDGET(AOMovie, ao_app)
  AO_WIDGET(AOCharMovie, ao_app)
  AO_WIDGET(AOEvidenceDisplay, ao_app)
  AO_WIDGET(AOViewport, ao_app)
  AO_WIDGET(AOICLog, ao_app)
  AO_WIDGET(AOServerChat, ao_app)
  AO_WIDGET(AOJukebox, ao_app)
  AO_WIDGET(AORoomChooser, ao_app)
  AO_WIDGET(AOMixer, ao_app)
  AO_WIDGET(AOChat, ao_app)
  AO_WIDGET(AORoomControls, ao_app)
  else
  {
    widget = QUiLoader::createWidget(className, parent, name);
  }

  if (widget == nullptr)
  {
    QMessageBox::critical(parent, "UI Error",
                          tr("There was an error loading the UI file:\n"
                             "Unrecognized widget class %1").arg(className));
  }
  widget->setObjectName(name);
  return widget;
}
