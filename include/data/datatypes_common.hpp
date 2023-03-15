#ifndef DATATYPES_COMMON_H
#define DATATYPES_COMMON_H
#pragma once

#include <QObject>
#include <QVariant>

namespace AttorneyOnline {
  namespace Core {
    class DataTypesCommon {
      Q_GADGET

    public:
      template <typename T> T toDataType(const QString &f_string)
      {
        return QVariant(f_string).value<T>();
      }

      template <typename T> QString fromDataType(const T &f_t)
      {
        return QVariant::fromValue(f_t).toString();
      }

      template <typename T> int fromDataType(const T &f_t)
      {
        return QVariant::fromValue(f_t).toInt();
      }
    };
  } // namespace Core
} // namespace AttorneyOnline

#endif // DATATYPES_COMMON_H
