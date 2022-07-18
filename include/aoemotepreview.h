#ifndef AOEMOTEPREVIEW_H
#define AOEMOTEPREVIEW_H

#include "aolayer.h"
#include <QWidget>

class AOEmotePreview : public QWidget
{
  Q_OBJECT
public:
  AOEmotePreview(QWidget *parent = nullptr,
                 AOApplication *p_ao_app = nullptr);

  void set_widgets();
  void play(QString emote, QString char_name, bool flipped = false, int self_offset = 0, int self_offset_v = 0);
private:
  AOApplication *ao_app;
  QWidget *ui_viewport;
  BackgroundLayer *ui_vp_background;
  SplashLayer *ui_vp_speedlines;
  CharLayer *ui_vp_player_char;
  BackgroundLayer *ui_vp_desk;

  QLabel *ui_size_label;

  QString m_emote = "";
  QString m_char = "";
protected:
  void resizeEvent(QResizeEvent *);
};

#endif // AOEMOTEPREVIEW_H
