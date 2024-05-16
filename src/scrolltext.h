#pragma once

#include <QDebug>
#include <QPainter>
#include <QStaticText>
#include <QTimer>
#include <QWidget>

class ScrollText : public QWidget
{
  Q_OBJECT

  Q_PROPERTY(QString text READ text WRITE setText)
  Q_PROPERTY(QString separator READ separator WRITE setSeparator)

public:
  explicit ScrollText(QWidget *parent = nullptr);

public Q_SLOTS:
  QString text() const;
  void setText(QString text);

  QString separator() const;
  void setSeparator(QString separator);

protected:
  virtual void paintEvent(QPaintEvent *);
  virtual void resizeEvent(QResizeEvent *);

private:
  QString m_text;
  QString _separator;
  QStaticText staticText;
  int singleTextWidth;
  QSize wholeTextSize;
  int leftMargin;
  bool scrollEnabled;
  int scrollPos;
  QImage alphaChannel;
  QImage buffer;
  QTimer timer;

  void updateText();

private Q_SLOTS:
  virtual void timer_timeout();
};
