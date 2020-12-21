#ifndef WIDGETDUMPER_H
#define WIDGETDUMPER_H

// https://gist.github.com/pjwhams/6ebc040db3ab55615eafd831e184e39c

#include <string>
#include <QLayoutItem>
#include <widgets/aocaseannouncerdialog.h>

namespace WidgetDumper {
  std::string toString(const QSizePolicy::Policy& policy);
  std::string toString(const QSizePolicy& policy);
  std::string toString(QLayout::SizeConstraint constraint);

  std::__cxx11::basic_string<char> getLayoutItemInfo(QLayoutItem *item);
  std::string getWidgetInfo(const QWidget &w);

  void dumpWidgetAndChildren(std::ostream &os, const QWidget *w, int level);
  void dumpWidgetHierarchy(const QWidget *w);
};


#endif // WIDGETDUMPER_H
