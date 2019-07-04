#include "widgets/aobutton.h"
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
  if (className == "AOButton")
  {
    widget = new AOButton(parent, ao_app);
  }
  else if (className == "AOImage")
  {
    widget = new AOImage(parent, ao_app);
  }
  else if (className == "AOTextEdit")
  {
    widget = new AOTextEdit(parent);
  }
  else if (className == "AOScene")
  {
    widget = new AOScene(parent, ao_app);
  }
  else if (className == "AOMovie")
  {
    widget = new AOMovie(parent, ao_app);
  }
  else if (className == "AOCharMovie")
  {
    widget = new AOCharMovie(parent, ao_app);
  }
  else if (className == "AOEvidenceDisplay")
  {
    widget = new AOEvidenceDisplay(parent, ao_app);
  }
  else if (className == "AOViewport")
  {
    widget = new AOViewport(parent, ao_app);
  }
  else if (className == "AOICLog")
  {
    widget = new AOICLog(parent, ao_app);
  }
  else if (className == "AOServerChat")
  {
    widget = new AOServerChat(parent, ao_app);
  }
  else if (className == "AOJukebox")
  {
    widget = new AOJukebox(parent, ao_app);
  }
  else if (className == "AORoomChooser")
  {
    widget = new AORoomChooser(parent, ao_app);
  }
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
