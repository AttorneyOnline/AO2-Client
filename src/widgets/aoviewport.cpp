#include "aouiloader.h"
#include "widgets/aoviewport.h"

#include <QVBoxLayout>

AOViewport::AOViewport(QWidget *parent, AOApplication *p_ao_app)
  : QWidget(parent), ao_app(p_ao_app)
{
  AOUiLoader loader(this, ao_app);
  QFile uiFile(":/resource/ui/viewport.ui");
  uiFile.open(QFile::ReadOnly);
  QWidget *windowWidget = loader.load(&uiFile, this);
  QMetaObject::connectSlotsByName(this);

  QVBoxLayout *parentLayout = new QVBoxLayout;
  parentLayout->addWidget(windowWidget);
  setLayout(parentLayout);

  chat_tick_timer = new QTimer(this);

  text_delay_timer = new QTimer(this);
  text_delay_timer->setSingleShot(true);

  sfx_delay_timer = new QTimer(this);
  sfx_delay_timer->setSingleShot(true);

  realization_timer = new QTimer(this);
  realization_timer->setSingleShot(true);

  testimony_show_timer = new QTimer(this);
  testimony_show_timer->setSingleShot(true);

  testimony_hide_timer = new QTimer(this);
  testimony_hide_timer->setSingleShot(true);

  sfx_player = new AOSfxPlayer(this, ao_app);
  sfx_player->set_volume(0);

  objection_player = new AOSfxPlayer(this, ao_app);
  objection_player->set_volume(0);

  blip_player = new AOBlipPlayer(this, ao_app);
  blip_player->set_volume(0);

  blip_rate = options.blipRate();
  blank_blip = options.blankBlipsEnabled();

  ui_vp_background = findChild<AOScene *>("ui_vp_background");
  ui_vp_speedlines = findChild<AOMovie *>("ui_vp_speedlines");

  ui_vp_player_char = findChild<AOCharMovie *>("ui_vp_player_char");
  ui_vp_sideplayer_char = findChild<AOCharMovie *>("ui_vp_sideplayer_char");

  ui_vp_desk = findChild<AOScene *>("ui_vp_desk");
  ui_vp_legacy_desk = findChild<AOScene *>("ui_vp_legacy_desk");

  ui_vp_evidence_display = findChild<AOEvidenceDisplay *>("ui_vp_evidence_display");

  ui_vp_chatbox = findChild<AOImage *>("ui_vp_chatbox");
  ui_vp_showname = findChild<QLabel *>("ui_vp_showname");
  ui_vp_message = findChild<QTextEdit *>("ui_vp_message");

  ui_vp_testimony = findChild<AOImage *>("ui_vp_testimony");
  ui_vp_realization = findChild<AOImage *>("ui_vp_realization");
  ui_vp_wtce = findChild<AOMovie *>("ui_vp_wtce");
  ui_vp_objection = findChild<AOMovie *>("ui_vp_objection");

  ui_vp_background->raise();
  ui_vp_speedlines->raise();
  ui_vp_player_char->raise();
  ui_vp_sideplayer_char->raise();
  ui_vp_desk->raise();
  ui_vp_legacy_desk->raise();
  ui_vp_evidence_display->raise();
  ui_vp_testimony->raise();
  ui_vp_wtce->raise();
  ui_vp_objection->raise();
  ui_vp_realization->raise();
  ui_vp_chatbox->raise();
  ui_vp_showname->raise();
  ui_vp_message->raise();

  ui_vp_speedlines->set_play_once(false);

  ui_vp_sideplayer_char->hide();

  ui_vp_message->setFrameStyle(QFrame::NoFrame);
  ui_vp_message->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_message->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_message->setTextInteractionFlags(Qt::NoTextInteraction);
  ui_vp_message->setReadOnly(true);

  ui_vp_testimony->set_image("testimony.png");
  ui_vp_testimony->hide();

  ui_vp_realization->set_image("realizationflash.png");
  ui_vp_realization->hide();

  //the size of the ui_vp_legacy_desk element relies on various factors and is set in set_scene()
  double y_modifier = 147.0 / 192.0;
  int final_y = static_cast<int>(y_modifier * height());
  ui_vp_legacy_desk->move(0, final_y);
  ui_vp_legacy_desk->hide();

  ui_vp_chatbox->set_image("chatmed.png");
  ui_vp_chatbox->hide();

  connect(ui_vp_objection, SIGNAL(done()), this, SLOT(objection_done()));
  connect(ui_vp_player_char, SIGNAL(done()), this, SLOT(preanim_done()));

  connect(text_delay_timer, SIGNAL(timeout()), this, SLOT(start_chat_ticking()));
  connect(sfx_delay_timer, SIGNAL(timeout()), this, SLOT(play_sfx()));

  connect(chat_tick_timer, SIGNAL(timeout()), this, SLOT(chat_tick()));

  connect(realization_timer, SIGNAL(timeout()), this, SLOT(realization_done()));

  connect(testimony_show_timer, SIGNAL(timeout()), this, SLOT(hide_testimony()));
  connect(testimony_hide_timer, SIGNAL(timeout()), this, SLOT(show_testimony()));
}

void AOViewport::set_background(QString p_background)
{
  testimony_in_progress = false;

  current_background = p_background;

  is_ao2_bg = file_exists(get_background_path("defensedesk.png")) &&
              file_exists(get_background_path("prosecutiondesk.png")) &&
              file_exists(get_background_path("stand.png"));

  if (!is_ao2_bg)
    qWarning().nospace() << "Some background files not found for "
                         << p_background << ". Falling back to AO1 filenames.";
}

void AOViewport::objection_done()
{
  handle_chatmessage_2();
}

void AOViewport::chat(chat_message_type chatmessage, QString showname,
  QString evidence_image)
{
  this->m_chatmessage = chatmessage;
  this->showname = showname;
  this->evidence_image = evidence_image;

  text_state = 0;
  anim_state = 0;
  ui_vp_objection->stop();
  ui_vp_player_char->stop();
  chat_tick_timer->stop();
  ui_vp_evidence_display->reset();

  chatmessage_is_empty = m_chatmessage.message == " " || m_chatmessage.message == "";

  int objection_modifier = m_chatmessage.objection_modifier;
  QString f_char = m_chatmessage.character;
  QString f_custom_theme = ao_app->get_char_shouts(f_char);

  //if an objection is used
  if (objection_modifier <= 4 && objection_modifier >= 1)
  {
    switch (objection_modifier)
    {
    case 1:
      ui_vp_objection->play("holdit", f_char, f_custom_theme);
      objection_player->play("holdit.wav", f_char, f_custom_theme);
      break;
    case 2:
      ui_vp_objection->play("objection", f_char, f_custom_theme);
      objection_player->play("objection.wav", f_char, f_custom_theme);
      break;
    case 3:
      ui_vp_objection->play("takethat", f_char, f_custom_theme);
      objection_player->play("takethat.wav", f_char, f_custom_theme);
      break;
    //case 4 is AO2 only
    case 4:
      ui_vp_objection->play("custom", f_char, f_custom_theme);
      objection_player->play("custom.wav", f_char, f_custom_theme);
      break;
    default:
      qDebug() << "W: Logic error in objection switch statement!";
    }

    int emote_mod = m_chatmessage.emote_modifier;

    if (emote_mod == 0)
      m_chatmessage.emote_modifier = PREANIM;

    // Will call handle_chatmessage_2() via objection_done()
    return;
  }

  handle_chatmessage_2();
}

void AOViewport::stop_sounds()
{
  sfx_player->stop();
  objection_player->stop();
}

void AOViewport::stop_talking()
{
  anim_state = 4;
  text_state = 3;
}

void AOViewport::handle_chatmessage_2()
{
  ui_vp_speedlines->stop();
  ui_vp_player_char->stop();

  ui_vp_showname->setText(showname);

  ui_vp_message->clear();
  ui_vp_chatbox->hide();
  ui_vp_showname->hide();

  QString chatbox = ao_app->get_chat(m_chatmessage.character);

  if (chatbox == "")
    ui_vp_chatbox->set_image("chatmed.png");
  else
  {
    QString chatbox_path = ao_app->get_base_path() + "misc/" + chatbox + "/chatbox.png";
    ui_vp_chatbox->set_image_from_path(chatbox_path);
  }

  ui_vp_showname->setStyleSheet("QLabel { color : " + get_text_color("_showname").name() + "; }");

  set_scene();
  set_text_color();

  // Check if the message needs to be centered.
  QString f_message = m_chatmessage.message;
  if (f_message.size() >= 2)
  {
    message_is_centered = f_message.startsWith("~~");
  }
  else
  {
    ui_vp_message->setAlignment(Qt::AlignLeft);
  }

  EMOTE_MODIFIER emote_mod = m_chatmessage.emote_modifier;

  if (m_chatmessage.flip == 1)
    ui_vp_player_char->set_flipped(true);
  else
    ui_vp_player_char->set_flipped(false);

  QString side = m_chatmessage.side;

  // Making the second character appear.
  if (m_chatmessage.pair_char_id == -1)
  {
    // If there is no second character, hide 'em, and center the first.
    ui_vp_sideplayer_char->hide();
    ui_vp_sideplayer_char->move(0,0);

    ui_vp_player_char->move(0,0);
  }
  else
  {
    // If there is, show them!
    ui_vp_sideplayer_char->show();

    // Depending on where we are, we offset the characters, and reorder their stacking.
    if (side == "def")
    {
      // We also move the character down depending on how far the are to the right.
      int hor_offset = m_chatmessage.self_offset;
      int vert_offset = 0;
      if (hor_offset > 0)
      {
        vert_offset = hor_offset / 10;
      }
      ui_vp_player_char->move(width() * hor_offset / 100, height() * vert_offset / 100);

      // We do the same with the second character.
      int hor2_offset = m_chatmessage.pair_offset;
      int vert2_offset = 0;
      if (hor2_offset > 0)
      {
        vert2_offset = hor2_offset / 10;
      }
      ui_vp_sideplayer_char->move(width() * hor2_offset / 100, height() * vert2_offset / 100);

      // Finally, we reorder them based on who is more to the left.
      // The person more to the left is more in the front.
      if (hor2_offset >= hor_offset)
      {
        ui_vp_sideplayer_char->stackUnder(ui_vp_player_char);
      }
      else
      {
        ui_vp_player_char->stackUnder(ui_vp_sideplayer_char);
      }
    }
    else if (side == "pro")
    {
      // Almost the same thing happens here, but in reverse.
      int hor_offset = m_chatmessage.self_offset;
      int vert_offset = 0;
      if (hor_offset < 0)
      {
        // We don't want to RAISE the char off the floor.
        vert_offset = -1 * hor_offset / 10;
      }
      ui_vp_player_char->move(width() * hor_offset / 100, height() * vert_offset / 100);

      // We do the same with the second character.
      int hor2_offset = m_chatmessage.pair_offset;
      int vert2_offset = 0;
      if (hor2_offset < 0)
      {
        vert2_offset = -1 * hor2_offset / 10;
      }
      ui_vp_sideplayer_char->move(width() * hor2_offset / 100, height() * vert2_offset / 100);

      // Finally, we reorder them based on who is more to the right.
      if (hor2_offset <= hor_offset)
      {
        ui_vp_sideplayer_char->stackUnder(ui_vp_player_char);
      }
      else
      {
        ui_vp_player_char->stackUnder(ui_vp_sideplayer_char);
      }
//          ui_vp_desk->raise();
//          ui_vp_legacy_desk->raise();
    }
    else
    {
      // In every other case, the person more to the left is on top.
      // These cases also don't move the characters down.
      int hor_offset = m_chatmessage.self_offset;
      ui_vp_player_char->move(width() * hor_offset / 100, 0);

      // We do the same with the second character.
      int hor2_offset = m_chatmessage.pair_offset;
      ui_vp_sideplayer_char->move(width() * hor2_offset / 100, 0);

      // Finally, we reorder them based on who is more to the left.
      // The person more to the left is more in the front.
      if (hor2_offset >= hor_offset)
      {
        ui_vp_sideplayer_char->stackUnder(ui_vp_player_char);
      }
      else
      {
        ui_vp_player_char->stackUnder(ui_vp_sideplayer_char);
      }
//          ui_vp_desk->raise();
//          ui_vp_legacy_desk->raise();
    }

    // We should probably also play the other character's idle emote.
    if (m_chatmessage.pair_flip == 1)
      ui_vp_sideplayer_char->set_flipped(true);
    else
      ui_vp_sideplayer_char->set_flipped(false);
    ui_vp_sideplayer_char->play_idle(m_chatmessage.pair_character.toString(),
                                     m_chatmessage.pair_anim);
  }

  switch (emote_mod)
  {
  case PREANIM: case OBJECTION_PREANIM: case OBJECTION_ZOOM:
    play_preanim(false);
    break;
  case NO_PREANIM: case ZOOM:
    if (!m_chatmessage.noninterrupting_preanim)
      handle_chatmessage_3();
    else
      play_preanim(true);
    break;
  default:
    qDebug() << "W: invalid emote mod: " << QString::number(emote_mod);
  }
}

void AOViewport::handle_chatmessage_3()
{
  start_chat_ticking();

  if (m_chatmessage.realization)
  {
    realization_timer->start(60);
    ui_vp_realization->show();
    sfx_player->play(ao_app->get_custom_realization(m_chatmessage.character));
  }

  QString side = m_chatmessage.side;

  if (!evidence_image.isEmpty())
  {
    //def jud and hlp should display the evidence icon on the RIGHT side
    bool is_left_side = !(side == "def" || side == "hlp" || side == "jud" || side == "jur");
    ui_vp_evidence_display->show_evidence(evidence_image, is_left_side, sfx_player->get_volume());
  }

  int emote_mod = m_chatmessage.emote_modifier;

  if (emote_mod == 5 ||
      emote_mod == 6)
  {
    ui_vp_desk->hide();
    ui_vp_legacy_desk->hide();

    // Since we're zooming, hide the second character, and centre the first.
    ui_vp_sideplayer_char->hide();
    ui_vp_player_char->move(0,0);

    if (side == "pro" ||
        side == "hlp" ||
        side == "wit")
      ui_vp_speedlines->play("prosecution_speedlines");
    else
      ui_vp_speedlines->play("defense_speedlines");

  }

  int f_anim_state = 0;
  //BLUE is from an enum in datatypes.h
  bool text_is_blue = m_chatmessage.text_color == BLUE;

  if (!text_is_blue && text_state == 1)
  {
    //talking
    f_anim_state = 2;
    entire_message_is_blue = false;
  }
  else
  {
    //idle
    f_anim_state = 3;
    entire_message_is_blue = true;
  }

  if (f_anim_state <= anim_state)
    return;

  ui_vp_player_char->stop();
  QString f_char = m_chatmessage.character;
  QString f_emote = m_chatmessage.anim;

  if (f_anim_state == 2) {
    ui_vp_player_char->play_talking(f_char, f_emote);
    anim_state = 2;
  }
  else
  {
    ui_vp_player_char->play_idle(f_char, f_emote);
    anim_state = 3;
  }
}

void AOViewport::play_preanim(bool noninterrupting)
{
  QString f_char = m_chatmessage.character;
  QString f_preanim = m_chatmessage.preanim;

  //all time values in char.inis are multiplied by a constant(time_mod) to get the actual time
  int ao2_duration = ao_app->get_ao2_preanim_duration(f_char, f_preanim);
  int text_delay = ao_app->get_text_delay(f_char, f_preanim) * time_mod;
  int sfx_delay = m_chatmessage.sfx_delay * 60;

  int preanim_duration;

  if (ao2_duration < 0)
    preanim_duration = ao_app->get_preanim_duration(f_char, f_preanim);
  else
    preanim_duration = ao2_duration;

  sfx_delay_timer->start(sfx_delay);
  QString anim_to_find = ao_app->get_image_suffix(ao_app->get_character_path(f_char, f_preanim));
  if (!file_exists(anim_to_find) ||
      preanim_duration < 0)
  {
    if (noninterrupting)
      anim_state = 4;
    else
      anim_state = 1;
    preanim_done();
    qDebug() << "could not find " + anim_to_find;
    return;
  }

  ui_vp_player_char->play_pre(f_char, f_preanim, preanim_duration);

  if (noninterrupting)
    anim_state = 4;
  else
    anim_state = 1;

  if (text_delay >= 0)
    text_delay_timer->start(text_delay);

  if (noninterrupting)
    handle_chatmessage_3();
}

void AOViewport::preanim_done()
{
  anim_state = 1;
  handle_chatmessage_3();
}

void AOViewport::realization_done()
{
  ui_vp_realization->hide();
}

void AOViewport::start_chat_ticking()
{
  //we need to ensure that the text isn't already ticking because this function can be called by two logic paths
  if (text_state != 0)
    return;

  ui_vp_message->clear();
  set_text_color();
  rainbow_counter = 0;

  if (chatmessage_is_empty)
  {
    //since the message is empty, it's technically done ticking
    text_state = 2;
    return;
  }

  // At this point, we'd do well to clear the inline colour stack.
  // This stops it from flowing into next messages.
  while (!inline_colour_stack.empty())
  {
      inline_colour_stack.pop();
  }

  ui_vp_chatbox->show();
  ui_vp_showname->show();

  tick_pos = 0;
  blip_pos = 0;

  // Just in case we somehow got inline blue text left over from a previous message,
  // let's set it to false.
  inline_blue_depth = 0;

  // At the start of every new message, we set the text speed to the default.
  current_display_speed = 3;
  chat_tick_timer->start(message_display_speed[current_display_speed]);

  QString f_gender = ao_app->get_gender(m_chatmessage.character);

  blip_player->set_blips("sfx-blip" + f_gender + ".wav");

  //means text is currently ticking
  text_state = 1;
}

void AOViewport::chat_tick()
{
  //note: this is called fairly often(every 60 ms when char is talking)
  //do not perform heavy operations here

  QString f_message = m_chatmessage.message;

  // Due to our new text speed system, we always need to stop the timer now.
  chat_tick_timer->stop();

  // Stops blips from playing when we have a formatting option.
  bool formatting_char = false;

  // If previously, we have detected that the message is centered, now
  // is the time to remove those two tildes at the start.
  if (message_is_centered)
  {
    f_message.remove(0,2);
  }

  if (tick_pos >= f_message.size())
  {
    text_state = 2;
    if (anim_state != 4)
    {
      anim_state = 3;
      ui_vp_player_char->play_idle(m_chatmessage.character, m_chatmessage.anim);
    }
  }

  else
  {
    QString f_character = f_message.at(tick_pos);
    f_character = f_character.toHtmlEscaped();

    if (f_character == " ")
      ui_vp_message->insertPlainText(" ");
    else if (m_chatmessage.text_color == RAINBOW)
    {
      QString html_color;

      switch (rainbow_counter)
      {
      case 0:
        html_color = get_text_color(QString::number(RED)).name();
        break;
      case 1:
        html_color = get_text_color(QString::number(ORANGE)).name();
        break;
      case 2:
        html_color = get_text_color(QString::number(YELLOW)).name();
        break;
      case 3:
        html_color = get_text_color(QString::number(GREEN)).name();
        break;
      default:
        html_color = get_text_color(QString::number(BLUE)).name();
        rainbow_counter = -1;
      }

      ++rainbow_counter;

      ui_vp_message->insertHtml("<font color=\"" + html_color + "\">" + f_character + "</font>");
    }

    // Escape character.
    else if (f_character == "\\" and !next_character_is_not_special)
    {
        next_character_is_not_special = true;
        formatting_char = true;
    }

    // Text speed modifier.
    else if (f_character == "{" and !next_character_is_not_special)
    {
        // ++, because it INCREASES delay!
        current_display_speed++;
        formatting_char = true;
    }
    else if (f_character == "}" and !next_character_is_not_special)
    {
        current_display_speed--;
        formatting_char = true;
    }

    // Orange inline colourisation.
    else if (f_character == "|" and !next_character_is_not_special)
    {
        if (!inline_colour_stack.empty())
        {
            if (inline_colour_stack.top() == INLINE_ORANGE)
            {
                inline_colour_stack.pop();
            }
            else
            {
                inline_colour_stack.push(INLINE_ORANGE);
            }
        }
        else
        {
            inline_colour_stack.push(INLINE_ORANGE);
        }
        formatting_char = true;
    }

    // Blue inline colourisation.
    else if (f_character == "(" and !next_character_is_not_special)
    {
        inline_colour_stack.push(INLINE_BLUE);
        ui_vp_message->insertHtml("<font color=\""+ get_text_color(QString::number(BLUE)).name() +"\">" + f_character + "</font>");

        // Increase how deep we are in inline blues.
        inline_blue_depth++;

        // Here, we check if the entire message is blue.
        // If it isn't, we stop talking.
        if (!entire_message_is_blue and anim_state != 4)
        {
          QString f_char = m_chatmessage.character;
          QString f_emote = m_chatmessage.anim;
          ui_vp_player_char->play_idle(f_char, f_emote);
        }
    }
    else if (f_character == ")" and !next_character_is_not_special
             and !inline_colour_stack.empty())
    {
        if (inline_colour_stack.top() == INLINE_BLUE)
        {
            inline_colour_stack.pop();
            ui_vp_message->insertHtml("<font color=\""+ get_text_color(QString::number(BLUE)).name() +"\">" + f_character + "</font>");

            // Decrease how deep we are in inline blues.
            // Just in case, we do a check if we're above zero, but we should be.
            if (inline_blue_depth > 0)
            {
              inline_blue_depth--;
              // Here, we check if the entire message is blue.
              // If it isn't, we start talking if we have completely climbed out of inline blues.
              if (!entire_message_is_blue)
              {
                // We should only go back to talking if we're out of inline blues, not during a non. int. pre, and not on the last character.
                if (inline_blue_depth == 0 and anim_state != 4 and !(tick_pos+1 >= f_message.size()))
                {
                  QString f_char = m_chatmessage.character;
                  QString f_emote = m_chatmessage.anim;
                  ui_vp_player_char->play_talking(f_char, f_emote);
                }
              }
            }
        }
        else
        {
            next_character_is_not_special = true;
            tick_pos--;
        }
    }

    // Grey inline colourisation.
    else if (f_character == "[" and !next_character_is_not_special)
    {
        inline_colour_stack.push(INLINE_GREY);
        ui_vp_message->insertHtml("<font color=\""+ get_text_color("_inline_grey").name() +"\">" + f_character + "</font>");
    }
    else if (f_character == "]" and !next_character_is_not_special
             and !inline_colour_stack.empty())
    {
        if (inline_colour_stack.top() == INLINE_GREY)
        {
            inline_colour_stack.pop();
            ui_vp_message->insertHtml("<font color=\""+ get_text_color("_inline_grey").name() +"\">" + f_character + "</font>");
        }
        else
        {
            next_character_is_not_special = true;
            tick_pos--;
        }
    }

    // Green inline colourisation.
    else if (f_character == "`" and !next_character_is_not_special)
    {
        if (!inline_colour_stack.empty())
        {
            if (inline_colour_stack.top() == INLINE_GREEN)
            {
                inline_colour_stack.pop();
                formatting_char = true;
            }
            else
            {
                inline_colour_stack.push(INLINE_GREEN);
                formatting_char = true;
            }
        }
        else
        {
            inline_colour_stack.push(INLINE_GREEN);
            formatting_char = true;
        }
    }
    else
    {
      next_character_is_not_special = false;
      if (!inline_colour_stack.empty())
      {
          switch (inline_colour_stack.top()) {
          case INLINE_ORANGE:
              ui_vp_message->insertHtml("<font color=\""+ get_text_color(QString::number(ORANGE)).name() +"\">" + f_character + "</font>");
              break;
          case INLINE_BLUE:
              ui_vp_message->insertHtml("<font color=\""+ get_text_color(QString::number(BLUE)).name() +"\">" + f_character + "</font>");
              break;
          case INLINE_GREEN:
              ui_vp_message->insertHtml("<font color=\""+ get_text_color(QString::number(GREEN)).name() +"\">" + f_character + "</font>");
              break;
          case INLINE_GREY:
              ui_vp_message->insertHtml("<font color=\""+ get_text_color("_inline_grey").name() +"\">" + f_character + "</font>");
              break;
          default:
              ui_vp_message->insertHtml(f_character);
              break;
          }

      }
      else
      {
          ui_vp_message->insertHtml(f_character);
      }

      if (message_is_centered)
      {
          ui_vp_message->setAlignment(Qt::AlignCenter);
      }
      else
      {
          ui_vp_message->setAlignment(Qt::AlignLeft);
      }
    }

    QScrollBar *scroll = ui_vp_message->verticalScrollBar();
    scroll->setValue(scroll->maximum());

    if (f_message.at(tick_pos) != ' ' || blank_blip)
    {

      if (blip_pos % blip_rate == 0 && !formatting_char)
      {
        blip_pos = 0;
        blip_player->blip_tick();
      }

      ++blip_pos;
    }

    ++tick_pos;

    // Restart the timer, but according to the newly set speeds, if there were any.
    // Keep the speed at bay.
    if (current_display_speed < 0)
    {
        current_display_speed = 0;
    }

    if (current_display_speed > 6)
    {
        current_display_speed = 6;
    }

    // If we had a formatting char, we shouldn't wait so long again, as it won't appear!
    if (formatting_char)
    {
        chat_tick_timer->start(1);
    }
    else
    {
        chat_tick_timer->start(message_display_speed[current_display_speed]);
    }

  }
}

void AOViewport::show_testimony()
{
  if (!testimony_in_progress || m_chatmessage.side != "wit")
    return;

  ui_vp_testimony->show();

  testimony_show_timer->start(testimony_show_time);
}

void AOViewport::hide_testimony()
{
  ui_vp_testimony->hide();

  if (!testimony_in_progress)
    return;

  testimony_hide_timer->start(testimony_hide_time);
}

void AOViewport::play_sfx()
{
  QString sfx_name = m_chatmessage.sfx_name;

  if (sfx_name.length() <= 1)
    return;

  sfx_player->play(ao_app->get_sfx_suffix(sfx_name));
}

void AOViewport::set_scene()
{
  if (testimony_in_progress)
    show_testimony();

  //witness is default if pos is invalid
  QString f_background = "witnessempty";
  QString f_desk_image = "stand";
  DESK_MODIFIER f_desk_mod = m_chatmessage.desk;
  QString f_side = m_chatmessage.side;

  if (f_side == "def")
  {
    f_background = "defenseempty";
    if (is_ao2_bg)
      f_desk_image = "defensedesk";
    else
      f_desk_image = "bancodefensa";
  }
  else if (f_side == "pro")
  {
    f_background = "prosecutorempty";
    if (is_ao2_bg)
      f_desk_image = "prosecutiondesk";
    else
      f_desk_image = "bancoacusacion";
  }
  else if (f_side == "jud")
  {
    f_background = "judgestand";
    f_desk_image = "judgedesk";
  }
  else if (f_side == "hld")
  {
    f_background = "helperstand";
    f_desk_image = "helperdesk";
  }
  else if (f_side == "hlp")
  {
    f_background = "prohelperstand";
    f_desk_image = "prohelperdesk";
  }
  else if (f_side == "jur" && (file_exists(get_background_path("jurystand.png")) ||
                               file_exists(get_background_path("jurystand.gif"))))
  {
    f_background = "jurystand";
    f_desk_image = "jurydesk";
  }
  else if (f_side == "sea" && (file_exists(get_background_path("seancestand.png")) ||
                               file_exists(get_background_path("seancestand.gif"))))
  {
    f_background = "seancestand";
    f_desk_image = "seancedesk";
  }
  else
  {
    if (is_ao2_bg)
      f_desk_image = "stand";
    else
      f_desk_image = "estrado";
  }

  ui_vp_background->set_image(current_background, f_background);
  ui_vp_desk->set_image(current_background, f_desk_image);
  ui_vp_legacy_desk->set_legacy_desk(current_background, f_desk_image);

  if (f_desk_mod == NO_DESK ||
      (f_desk_mod == DEFAULT_DESK_MOD &&
       (f_side == "jud" ||
        f_side == "hld" ||
        f_side == "hlp")))
  {
    ui_vp_desk->hide();
    ui_vp_legacy_desk->hide();
  }
  else if (is_ao2_bg || (f_side == "jud" ||
                         f_side == "hld" ||
                         f_side == "hlp"))
  {
    ui_vp_legacy_desk->hide();
    ui_vp_desk->show();
  }
  else
  {
    if (f_side == "wit")
    {
      ui_vp_desk->show();
      ui_vp_legacy_desk->hide();
    }
    else
    {
      ui_vp_desk->hide();
      ui_vp_legacy_desk->show();
    }
  }
}

void AOViewport::set_text_color()
{
  QColor textcolor = ao_app->get_chat_color(QString::number(m_chatmessage.text_color),
                                            ao_app->get_chat(m_chatmessage.character));

  ui_vp_message->setTextBackgroundColor(QColor(0,0,0,0));
  ui_vp_message->setTextColor(textcolor);

  QString style = "background-color: rgba(0, 0, 0, 0);";
  style.append("color: rgb(");
  style.append(QString::number(textcolor.red()));
  style.append(", ");
  style.append(QString::number(textcolor.green()));
  style.append(", ");
  style.append(QString::number(textcolor.blue()));
  style.append(")");

  ui_vp_message->setStyleSheet(style);
}

QColor AOViewport::get_text_color(QString color)
{
  return ao_app->get_chat_color(color, ao_app->get_chat(m_chatmessage.character));
}

void AOViewport::wtce(WTCE_TYPE type)
{
  switch (type)
  {
  case WITNESS_TESTIMONY:
    sfx_player->play(ao_app->get_sfx("witness_testimony"));
    ui_vp_wtce->play("witnesstestimony");
    testimony_in_progress = true;
    show_testimony();
    break;
  case CROSS_EXAMINATION:
    sfx_player->play(ao_app->get_sfx("cross_examination"));
    ui_vp_wtce->play("crossexamination");
    testimony_in_progress = false;
    break;
  case NOT_GUILTY:
    sfx_player->play(ao_app->get_sfx("not_guilty"));
    ui_vp_wtce->play("notguilty");
    testimony_in_progress = false;
    break;
  case GUILTY:
    sfx_player->play(ao_app->get_sfx("guilty"));
    ui_vp_wtce->play("guilty");
    testimony_in_progress = false;
    break;
  }
}

void AOViewport::set_sfx_volume(int value)
{
  sfx_player->set_volume(value);
  objection_player->set_volume(value);
}

void AOViewport::set_blip_volume(int value) {
  blip_player->set_volume(value);
}

AOViewport::~AOViewport()
{
  delete sfx_player;
  delete objection_player;
  delete blip_player;
}
