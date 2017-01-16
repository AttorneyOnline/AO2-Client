#ifndef COURTROOM_H
#define COURTROOM_H

#include "aoimage.h"
#include "aobutton.h"
#include "aopacket.h"

#include <QMainWindow>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QListWidget>

class AOApplication;

class Courtroom : public QMainWindow
{
    Q_OBJECT
public:
    explicit Courtroom(AOApplication *parent = 0);

private:
    AOApplication *ao_app;

    AOImage *ui_background;
    //viewport elements like background, desk, etc.

    QPlainTextEdit *ui_ic_chatlog;

    QPlainTextEdit *ui_server_chatlog;
    QPlainTextEdit *ui_ms_chatlog;

    QListWidget *ui_muted_list;
    QListWidget *ui_area_list;
    QListWidget *ui_music_list;

    QLineEdit *ui_ic_chat_message;

    QLineEdit *ui_ooc_chat_message;
    QLineEdit *ui_ooc_chat_name;

    QLineEdit *ui_area_password;
    QLineEdit *ui_music_search;

    //emote buttons

    AOImage *ui_emote_left;
    AOImage *ui_emote_right;

    AOButton *ui_hold_it;
    AOButton *ui_objection;
    AOButton *ui_take_that;

    AOButton *ui_ooc_toggle;
    AOButton *ui_witness_testimony;
    AOButton *ui_cross_examination;

    AOButton *ui_change_character;
    AOButton *ui_reload_theme;
    AOButton *ui_call_mod;
\
    AOButton *ui_custom_objection;
    AOButton *ui_realization;
    AOButton *ui_mute;


    AOImage *ui_muted;



    //ui_charselect w/ icons

signals:

public slots:
};

#endif // COURTROOM_H
