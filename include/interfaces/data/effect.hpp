#ifndef EFFECT_H
#define EFFECT_H
#pragma once

#include <QString>
#include <include/data/datatypes_animation.hpp>

namespace AttorneyOnline {
  namespace Core {
    class Effect {
    public:
      Effect() = default;
      ~Effect() = default;

      virtual QString name() = 0;
      virtual QString sound() = 0;
      virtual bool cull() = 0;
      virtual DataTypesAnimation::EffectLayer layer() = 0;
      virtual bool loop() = 0;
      virtual int maxDuration() = 0;
      virtual bool applyFlip() = 0;
      virtual bool applyOffset() = 0;
      virtual Qt::TransformationMode scaling() = 0;
      virtual bool remainSelected() = 0;
      virtual bool stretchToViewport() = 0;
    };
  } // namespace Core
} // namespace AttorneyOnline

#endif // EFFECT_H
