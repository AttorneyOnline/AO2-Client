#ifndef AOSCENE_H
#define AOSCENE_H

#include <QDebug>
#include <QLabel>
#include <QMovie>

class Courtroom;
class AOApplication;

class AOScene : public QLabel {
    Q_OBJECT
public:
    explicit AOScene(QWidget *parent, AOApplication *p_ao_app);

    void set_image(QString p_image);
    void set_legacy_desk(QString p_image);

    //Move the label itself around
    void move(int ax, int ay);

    //This is somewhat pointless now as there's no "QMovie" object to resize, aka no "combo" to speak of
    void combo_resize(int w, int h);

private:
    QWidget *m_parent;
    QMovie *m_movie;
    AOApplication *ao_app;
    QString last_image;

    // These are the X and Y values before they are fixed based on the sprite's width.
    int x = 0;
    int y = 0;
    // These are the width and height values before they are fixed based on the sprite's width.
    int f_w = 0;
    int f_h = 0;
};

#endif // AOSCENE_H
