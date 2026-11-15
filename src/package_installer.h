#ifndef PACKAGE_INSTALLER_H
#define PACKAGE_INSTALLER_H

#include <QObject>
#include <QStringList>

class MountManager;
class QNetworkAccessManager;
class QWidget;

/**
 * Installs dropped archive files into the packages/ folder.
 *
 * Archives are extracted into a temporary staging folder, validated, and then
 * moved into place. Existing packages prompt for Replace / Keep both / Cancel.
 */
class PackageInstaller : public QObject {
  Q_OBJECT

public:
  enum class InstallTarget { Packages, Base };

  explicit PackageInstaller(MountManager *mounts, QObject *parent = nullptr);

  /// Installs each archive in order. Prompts use parent as their window.
  /// InstallTarget::Base merges package content over the base folder instead.
  void installArchives(const QStringList &archivePaths, QWidget *parent,
                       InstallTarget target = InstallTarget::Packages);

  /// Downloads an archive and installs it into the requested target.
  void installArchiveFromUrl(const QString &url, QWidget *parent,
                             InstallTarget target);

  bool isBusy() const { return m_busy; }

signals:
  /// Emitted after at least one package was installed, including replacements
  /// that don't change the mount order.
  void packagesChanged();
  /// Emitted after content was installed over the base folder.
  void baseChanged();

private:
  MountManager *m_mounts = nullptr;
  QNetworkAccessManager *m_network = nullptr;
  bool m_busy = false;
};

#endif // PACKAGE_INSTALLER_H
