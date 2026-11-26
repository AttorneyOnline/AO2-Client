#ifndef MOUNT_MANAGER_H
#define MOUNT_MANAGER_H

#include <QFileSystemWatcher>
#include <QObject>
#include <QStringList>

class QFileSystemWatcher;

/**
 * @brief Owns the ordered list of mounted asset roots.
 *
 * The order lives in packages/mounts.txt, next to the packages folder. Each
 * non-empty line is one mount, top-to-bottom. Entries beginning with
 * "packages/" are discovered package folders; everything else is an external
 * mount. The implicit base/ mount is not stored here.
 */
class MountManager : public QObject {
  Q_OBJECT

public:
  explicit MountManager(QObject *parent = nullptr);

  /// Creates packages/, migrates the legacy config.ini list if needed, scans
  /// for packages, and starts watching the packages folder.
  void initialize();

  /// Stored entries in priority order (relative package paths and external
  /// paths), exactly as written to mounts.txt.
  QStringList entries() const;

  /// Absolute paths for all stored entries, in priority order. The implicit
  /// base/ path is not included.
  QStringList resolvedMountPaths() const;

  /// True if the entry refers to an immediate child of packages/.
  bool isPackageEntry(const QString &entry) const;

  /// True if an equivalent entry (same resolved path) is already mounted.
  bool containsEntry(const QString &entry) const;

  /// Converts a user-selected folder into the entry that should be stored.
  /// Immediate children of packages/ become "packages/<name>"; other folders
  /// become app-relative when possible. Returns an empty string if the path is
  /// not an existing directory.
  QString entryForPath(const QString &path) const;

  /// Replaces the entire ordered list and writes mounts.txt. Returns true if
  /// the list actually changed.
  bool setEntries(const QStringList &entries);

  /// Path to the packages folder (with trailing slash).
  QString packagesPath() const;

  /// Scans packages immediately without waiting for the watcher debounce.
  void rescanNow();

signals:
  /// Emitted when the effective mount order changes, whether from an explicit
  /// save or from a package folder appearing/disappearing.
  void changed();

private slots:
  void onDirectoryChanged(const QString &path);
  void onApplicationStateChanged(Qt::ApplicationState state);

private:
  static QString normalizedEntry(const QString &entry);
  static QString resolveEntry(const QString &entry);
  static QString dedupeKey(const QString &entry);
  static bool hasPackagesPrefix(const QString &entry);

  QString packageNameFromEntry(const QString &entry) const;
  QStringList sanitizedEntries(const QStringList &entries) const;

  bool readMountsFile(QStringList &entries) const;
  bool writeMountsFile(const QStringList &entries) const;
  bool scanPackages(bool forceWrite);
  void scheduleScan();
  void refreshWatchPath();

  QString m_packagesPath;
  QString m_mountsFilePath;
  QStringList m_entries;

  // resolvedMountPaths() is called for every cache miss, so avoid rebuilding
  // the list unless the mount order actually changed.
  mutable QStringList m_resolvedPathsCache;
  mutable bool m_resolvedPathsValid = false;

  QFileSystemWatcher *m_watcher = nullptr;
  bool m_scanPending = false;
  bool m_scanScheduled = false;
};

#endif // MOUNT_MANAGER_H
