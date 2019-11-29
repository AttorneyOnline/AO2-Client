#include "widgets/aomixer.h"
#include "aouiloader.h"

#include <QVBoxLayout>

AOMixer::AOMixer(QWidget *parent, AOApplication *p_ao_app)
  : QWidget(parent), ao_app(p_ao_app)
{
  AOUiLoader loader(this, ao_app);
  QFile uiFile(":/resource/ui/mixer.ui");
  uiFile.open(QFile::ReadOnly);
  QWidget *windowWidget = loader.load(&uiFile, this);
  QMetaObject::connectSlotsByName(this);

  QVBoxLayout *parentLayout = new QVBoxLayout;
  parentLayout->setMargin(0);
  parentLayout->addWidget(windowWidget);
  setLayout(parentLayout);

  ui_master_level = findChild<QProgressBar *>("master_level");
  ui_music_slider = findChild<QSlider *>("music_slider");
  ui_sfx_slider = findChild<QSlider *>("sfx_slider");
  ui_blips_slider = findChild<QSlider *>("blips_slider");

  const Options options;
  ui_music_slider->setValue(options.defaultMusicVolume());
  ui_sfx_slider->setValue(options.defaultSfxVolume());
  ui_blips_slider->setValue(options.defaultBlipVolume());
}

void AOMixer::set_master_level(int level)
{
  ui_master_level->setValue(level);
}

void AOMixer::on_musicSlider_valueChanged(int volume)
{
  emit volumeChanged(MUSIC, volume);
}

void AOMixer::on_sfxSlider_valueChanged(int volume)
{
  emit volumeChanged(SFX, volume);
}

void AOMixer::on_blipsSlider_valueChanged(int volume)
{
  emit volumeChanged(BLIPS, volume);
}
