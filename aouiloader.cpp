#include "aobutton.h"
#include "aoimage.h"
#include "aotextarea.h"
#include "aotextedit.h"
#include "aouiloader.h"

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
  else
  {
    widget = QUiLoader::createWidget(className, parent, name);
  }
  widget->setObjectName(name);
  return widget;
}
