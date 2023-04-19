#include "pathing/aofinder.hpp"
#include "pathing/aocache.hpp"

#include <QDebug>

AOFinder::AOFinder(QObject *parent, QString f_base_path)
    : QObject(parent),
      m_cache(std::make_unique<AttorneyOnline::Core::AOCache>(this, f_base_path))
{
           qInfo() << "[AOFinder]::CTOR: Starting";
}

AOFinder::~AOFinder() {}
