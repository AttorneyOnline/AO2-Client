#include <QDialog>
#pragma once

#ifndef AO_UI_FAVORITESERVERDIALOG
#define AO_UI_FAVORITESRRVERDIALOG
namespace AttorneyOnline {
  namespace UI {
    class FavoriteServerDialog : public QDialog {
    public:
      FavoriteServerDialog() = default;
      ~FavoriteServerDialog() = default;

      const QString DEFAULT_UI = "favorite_server_dialog.ui";
      const int TCP_INDEX = 0;
    private slots:
      virtual void onSavePressed() = 0;
      virtual void onCancelPressed() = 0;
    };
  } // namespace UI
} // namespace AttorneyOnline
#endif // AO_UI_FAVORITESERVERDIALOG
