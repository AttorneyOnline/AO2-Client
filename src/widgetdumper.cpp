#include "widgetdumper.h"

#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <QtWidgets/QLayout>
#include <QtWidgets/QWidget>

#if _MSC_VER
#define snprintf _snprintf
#endif


std::string WidgetDumper::toString(const QSizePolicy::Policy &policy) {
  switch (policy) {
    case QSizePolicy::Fixed:
      return "Fixed";
    case QSizePolicy::Minimum:
      return "Minimum";
    case QSizePolicy::Maximum:
      return "Maximum";
    case QSizePolicy::Preferred:
      return "Preferred";
    case QSizePolicy::MinimumExpanding:
      return "MinimumExpanding";
    case QSizePolicy::Expanding:
      return "Expanding";
    case QSizePolicy::Ignored:
      return "Ignored";
  }
  return "unknown";
}

std::string WidgetDumper::toString(const QSizePolicy &policy) {
  return "(" + toString(policy.horizontalPolicy()) + ", " + toString(policy.verticalPolicy()) + ")";
}

std::string WidgetDumper::toString(QLayout::SizeConstraint constraint) {
  switch (constraint) {
    case QLayout::SetDefaultConstraint:
      return "SetDefaultConstraint";
    case QLayout::SetNoConstraint:
      return "SetNoConstraint";
    case QLayout::SetMinimumSize:
      return "SetMinimumSize";
    case QLayout::SetFixedSize:
      return "SetFixedSize";
    case QLayout::SetMaximumSize:
      return "SetMaximumSize";
    case QLayout::SetMinAndMaxSize:
      return "SetMinAndMaxSize";
  }
  return "unknown";
}

std::string WidgetDumper::getWidgetInfo(const QWidget &w) {
  const QRect &geom = w.geometry();
  QSize hint = w.sizeHint();
  char buf[1024];
  snprintf(buf, 1023, "%s %p ('%s'), pos (%d, %d), size (%d x %d), hint (%d x %d) pol: %s %s\n",
           w.metaObject()->className(), (void *) &w, w.objectName().toStdString().c_str(),
           geom.x(), geom.y(), geom.width(), geom.height(),
           hint.width(), hint.height(),
           toString(w.sizePolicy()).c_str(),
           (w.isVisible() ? "" : "<hidden>")
  );
  return buf;
}

std::string WidgetDumper::getLayoutItemInfo(QLayoutItem *item) {
  if (dynamic_cast<QWidgetItem *>(item)) {
    auto *wi = dynamic_cast<QWidgetItem *>(item);
    if (wi->widget()) {
      return getWidgetInfo(*wi->widget());
    }
  }
  else if (dynamic_cast<QSpacerItem *>(item)) {
    auto *si = dynamic_cast<QSpacerItem *>(item);
    QSize hint = si->sizeHint();
    std::string constraint = "<no layout>";
    if (si->layout()) {
      constraint = toString(si->layout()->sizeConstraint());
    }
    std::stringstream ss;
    ss << "SpacerItem hint (" << hint.width() << " x " << hint.height() << ") policy: "
       << toString(si->sizePolicy()) << " constraint: " << constraint << "\n";
    return ss.str();
  }
  return "";
}

//------------------------------------------------------------------------
void WidgetDumper::dumpWidgetAndChildren(std::ostream &os, const QWidget *w, int level) {
  std::string padding("");
  for (int i = 0; i <= level; i++)
    padding += "  ";

  QLayout *layout = w->layout();
  QList<QWidget *> dumpedChildren;
  if (layout && !layout->isEmpty()) {

    os << padding << "Layout ";
    QMargins margins = layout->contentsMargins();
    os << " margin: (" << margins.left() << "," << margins.top()
       << "," << margins.right() << "," << margins.bottom() << "), constraint: "
       << toString(layout->sizeConstraint());

    if (dynamic_cast<QBoxLayout *>(layout)) {
      auto *boxLayout = dynamic_cast<QBoxLayout *>(layout);
      os << " spacing: " << boxLayout->spacing();
    }
    os << ":\n";

    int numItems = layout->count();
    for (int i = 0; i < numItems; i++) {
      QLayoutItem *layoutItem = layout->itemAt(i);
      std::string itemInfo = getLayoutItemInfo(layoutItem);

      os << padding << " " << itemInfo;

      auto *wi = dynamic_cast<QWidgetItem *>(layoutItem);
      if (wi && wi->widget()) {
        dumpWidgetAndChildren(os, wi->widget(), level + 1);
        dumpedChildren.push_back(wi->widget());
      }
    }
  }

  // now output any child widgets that weren't dumped as part of the layout
  QList<QWidget *> widgets = w->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly);
  QList<QWidget *> undumpedChildren;
      foreach (QWidget *child, widgets) {
      if (dumpedChildren.indexOf(child) == -1) {
        undumpedChildren.push_back(child);
      }
    }

  if (!undumpedChildren.empty()) {
    os << padding << " non-layout children:\n";
        foreach (QWidget *child, undumpedChildren) {
        dumpWidgetAndChildren(os, child, level + 1);
      }
  }
}

//------------------------------------------------------------------------
void WidgetDumper::dumpWidgetHierarchy(const QWidget *w) {
  std::ostringstream oss;
  oss << getWidgetInfo(*w);
  dumpWidgetAndChildren(oss, w, 0);
  std::cout << oss.str();
}