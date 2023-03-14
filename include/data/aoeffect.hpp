#ifndef AOEFFECT_H
#define AOEFFECT_H
#pragma once

#include "include/interfaces/data/effect.hpp"

#include <QSettings>
#include <datatypes.h>

class AOEffect : public AttorneyOnline::Core::Effect {
public:
    AOEffect(const QString effect_ini_path);
    ~AOEffect();

    QString name() override;
    QString sound() override;
    bool cull() override;
    EffectLayerLocation layer() override;
    bool loop() override;
    int maxDuration() override;
    bool applyFlip() override;
    bool applyOffset() override;
    Qt::TransformationMode scaling() override;
    bool remainSelected() override;
    bool stretchToViewport() override;
private:
    QSettings m_effect_ini;
};

#endif // AOEFFECT_H
