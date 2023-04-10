#ifndef VIEWPORT_H
#define VIEWPORT_H
#pragma once

#include <QWidget>

namespace AttorneyOnline {
  namespace UI {
    class Viewport : public QWidget {
    public:
      Viewport(QWidget *parent) : QWidget(parent){};
      ~Viewport() = default;

      virtual bool isViewportBusy() = 0;

      virtual void setBackground(const QString &f_background) = 0;
    };
  } // namespace UI
} // namespace AttorneyOnline

#endif // VIEWPORT_H
