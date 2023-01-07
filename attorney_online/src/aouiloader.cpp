#include "aouiloder.h"

#include "widgets/courtroom/aoviewport.h"

#include <QMessageBox>

#define AO_WIDGET(class, ...)                                                  \
  else if (className == #class)                                                \
  {                                                                            \
    widget = new class(parent, ##__VA_ARGS__);                                 \
  }

QWidget *AOUiLoader::createWidget(const QString &className, QWidget *parent,
                                  const QString &name)
{
  QWidget *widget;

  if (0)
    ; // (a necessary sacrifice for a greater good... muhehe)
  AO_WIDGET(AOViewport, ao_app)
  else
  {
    widget = QUiLoader::createWidget(className, parent, name);
  }

  if (widget == nullptr) {
    QMessageBox::critical(parent, "UI Error",
                          tr("There was an error loading the UI file:\n"
                             "Unrecognized widget class %1")
                              .arg(className));
    return widget;
  }
  widget->setObjectName(name);
  return widget;
}
