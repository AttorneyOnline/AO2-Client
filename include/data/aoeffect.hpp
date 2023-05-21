#ifndef AOEFFECT_H
#define AOEFFECT_H
#pragma once

#include "include/interfaces/data/effect.hpp"

#include "include/data/datatypes_animation.hpp"
#include <QSettings>

using namespace AttorneyOnline::Core;

class AOEffect : public AttorneyOnline::Core::Effect {
public:
  AOEffect(const QString effect_ini_path);
  ~AOEffect();

  QString name() override;
  QString sound() override;
  bool cull() override;
  DataTypesAnimation::EffectLayer layer() override;
  bool loop() override;
  int maxDuration() override;
  bool applyFlip() override;
  bool applyOffset() override;
  Qt::TransformationMode scaling() override;
  bool remainSelected() override;
  bool stretchToViewport() override;

private:
  const QSettings m_effect_ini;
};

#endif // AOEFFECT_H
