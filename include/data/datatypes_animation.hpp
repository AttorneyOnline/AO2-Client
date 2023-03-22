#ifndef DATATYPES_ANIMATION_H
#define DATATYPES_ANIMATION_H
#pragma once

#include "include/data/datatypes_common.hpp"

namespace AttorneyOnline {
  namespace Core {
    class DataTypesAnimation : public DataTypesCommon {
    public:
      enum class EffectLayer { BEHIND, CHARACTER, OVER, CHAT };
      Q_ENUM(EffectLayer)
    };
  } // namespace Core
} // namespace AttorneyOnline

#endif // DATATYPES_ANIMATION_H
