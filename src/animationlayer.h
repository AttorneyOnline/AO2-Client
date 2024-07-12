#pragma once

#include "animationloader.h"
#include "datatypes.h"

#include <QBitmap>
#include <QDebug>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>

// #define DEBUG_MOVIE

#ifdef DEBUG_MOVIE
#include <QElapsedTimer>
#endif

class AOApplication;
class VPath;

// "Brief" explanation of what the hell this is:
//
// AOLayer handles all animations both inside and outside
// the viewport. It was originally devised as a layering
// system, but turned into a full refactor of the existing
// animation code.
//
// AOLayer has six subclasses, all of which differ mainly in
// how they handle path resolution.
//
//  - BackgroundLayer: self-explanatory, handles files found in base/background
//  - CharLayer: handles all the "wonderful" quirks of character path resolution
//  - SplashLayer: handles elements that can either be provided by a misc/ directory
//    or by the theme - speedlines, shouts, WT/CE, et cetera
//  - EffectLayer: this is basically a dummy layer since effects do their own wonky
//    path resolution in a different file
//  - InterfaceLayer: handles UI elements like the chat arrow and the music display
//  - StickerLayer: Crystalwarrior really wanted this. Handles "stickers," whatever those are.
//
// For questions comments or concerns, bother someone else

namespace kal
{
class AnimationLayer : public QLabel
{
  Q_OBJECT

public:
  explicit AnimationLayer(QWidget *parent = nullptr);
  virtual ~AnimationLayer();

  QString fileName();
  void setFileName(QString fileName);

  void startPlayback();
  void stopPlayback();
  void restartPlayback();
  void pausePlayback(bool enabled);

  QSize frameSize();

  int frameCount();
  int currentFrameNumber();
  void jumpToFrame(int number);

  bool isPlayOnce();

  void setPlayOnce(bool enabled);
  void setStretchToFit(bool enabled);
  void setResetCacheWhenStopped(bool enabled);
  void setFlipped(bool enabled);
  void setResizeMode(RESIZE_MODE mode);
  void setMinimumDurationPerFrame(int duration);
  void setMaximumDurationPerFrame(int duration);

public Q_SLOTS:
  void setMaskingRect(QRect rect);

Q_SIGNALS:
  void startedPlayback();
  void stoppedPlayback();  /* Is emitted whenever playback is stopped, whether by user or by reaching the end */
  void finishedPlayback(); /* Is emitted only when playback reaches the end */
  void frameNumberChanged(int frameNumber);

protected:
  void resizeEvent(QResizeEvent *event) override;

private:
  QString m_file_name;
  bool m_play_once = false;
  bool m_stretch_to_fit = false;
  bool m_reset_cache_when_stopped = false;
  bool m_flipped = false;
  int m_minimum_duration = 0;
  int m_maximum_duration = 0;
  RESIZE_MODE m_resize_mode = AUTO_RESIZE_MODE;
  Qt::TransformationMode m_transformation_mode = Qt::FastTransformation;
  AnimationLoader *m_loader = nullptr;
  QSize m_frame_size;
  QRect m_frame_rect;
  QRect m_mask_rect_hint;
  QRect m_mask_rect;
  QSize m_scaled_frame_size;
  bool m_processing = false;
  bool m_pause = false;
  QTimer *m_ticker = nullptr;
  bool m_first_frame = false;
  int m_frame_number = 0;
  int m_target_frame_number = -1;
  int m_frame_count = 0;
  AnimationFrame m_current_frame;

  void createLoader();
  void deleteLoader();

  void resetData();

  void calculateFrameGeometry();

  void finishPlayback();

  void prepareNextTick();

  void displayCurrentFrame();

private Q_SLOTS:
  void frameTicker();
};

class CharacterAnimationLayer : public AnimationLayer
{
  Q_OBJECT

public:
  enum EmoteType
  {
    NoEmoteType,
    PreEmote,
    IdleEmote,
    TalkEmote,
    PostEmote,
  };

  enum EffectType
  {
    SfxEffect,
    ShakeEffect,
    FlashEffect,
  };

  class FrameEffect
  {
  public:
    QString emote_name;
    EffectType type = SfxEffect;
    QString file_name;
  };

  CharacterAnimationLayer(AOApplication *ao_app, QWidget *parent = nullptr);

  void loadCharacterEmote(QString character, QString fileName, EmoteType emoteType, int durationLimit = 0);

  void setFrameEffects(QStringList data);

Q_SIGNALS:
  void finishedPreOrPostEmotePlayback();

  void soundEffect(QString sfx);
  void shakeEffect();
  void flashEffect();

private:
  AOApplication *ao_app;

  QString m_character;
  QString m_emote;
  QString m_resolved_emote;
  EmoteType m_emote_type = NoEmoteType;
  QTimer *m_duration_timer = nullptr;
  int m_duration = 0;

  QMap<int, QList<FrameEffect>> m_effects;

  void startTimeLimit();

private Q_SLOTS:
  void onPlaybackStopped();
  void onPlaybackFinished();
  void onDurationLimitReached();

  void notifyFrameEffect(int frame);
  void notifyEmotePlaybackFinished();
};

class BackgroundAnimationLayer : public AnimationLayer
{
  Q_OBJECT

public:
  BackgroundAnimationLayer(AOApplication *ao_app, QWidget *parent = nullptr);

  void loadAndPlayAnimation(QString fileName);

private:
  AOApplication *ao_app;
};

class SplashAnimationLayer : public AnimationLayer
{
  Q_OBJECT

public:
  SplashAnimationLayer(AOApplication *ao_app, QWidget *parent = nullptr);

  void loadAndPlayAnimation(QString fileName, QString character, QString miscellaneous);

private:
  AOApplication *ao_app;
};

class EffectAnimationLayer : public AnimationLayer
{
  Q_OBJECT

public:
  EffectAnimationLayer(AOApplication *ao_app, QWidget *parent = nullptr);

  void loadAndPlayAnimation(QString fileName, bool repeat = false);

  void setHideWhenStopped(bool enabled);

private:
  AOApplication *ao_app;

  bool m_hide_when_stopped = false;

private Q_SLOTS:
  void maybeHide();
};

class InterfaceAnimationLayer : public AnimationLayer
{
  Q_OBJECT

public:
  InterfaceAnimationLayer(AOApplication *ao_app, QWidget *parent = nullptr);

  void loadAndPlayAnimation(QString fileName, QString miscName);

private:
  AOApplication *ao_app;
};

class StickerAnimationLayer : public AnimationLayer
{
  Q_OBJECT

public:
  StickerAnimationLayer(AOApplication *ao_app, QWidget *parent = nullptr);

  void loadAndPlayAnimation(QString fileName);

private:
  AOApplication *ao_app;
};
} // namespace kal
