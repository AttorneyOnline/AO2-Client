#include "aobutton.h"
#include "aocharmovie.h"
#include "aoevidencedisplay.h"
#include "aoimage.h"
#include "aomovie.h"
#include "aoscene.h"
#include "aotextarea.h"
#include "aotextedit.h"
#include "aouiloader.h"
#include "aoviewport.h"

#include <QMessageBox>

QWidget *AOUiLoader::createWidget(const QString &className, QWidget *parent, const QString &name)
{
  QWidget *widget;
  if (className == "AOButton")
  {
    widget = new AOButton(parent, ao_app);
  }
  else if (className == "AOTextArea")
  {
    widget = new AOTextArea(parent);
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
