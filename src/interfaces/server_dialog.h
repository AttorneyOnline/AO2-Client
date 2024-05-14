#pragma once

#include <QDialog>

namespace AttorneyOnline
{
namespace UI
{
class FavoriteServerDialog : public QDialog
{
public:
  const QString DEFAULT_UI = "favorite_server_dialog.ui";
  const int TCP_INDEX = 0;

private Q_SLOTS:
  virtual void onSavePressed() = 0;
  virtual void onCancelPressed() = 0;
};
} // namespace UI
} // namespace AttorneyOnline
