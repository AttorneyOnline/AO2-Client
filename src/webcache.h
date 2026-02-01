#pragma once

#include <QDateTime>
#include <QDir>
#include <QHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QSet>
#include <QString>
#include <QVector>

class AOApplication;

class WebCache : public QObject
{
  Q_OBJECT

public:
  explicit WebCache(AOApplication *parent);
  ~WebCache();

  /**
   * @brief Returns the cached file path if it exists and is not expired.
   * @param relativePath The virtual path relative to the base (e.g., "sounds/music/song.mp3")
   * @return The absolute path to the cached file, or empty string if not cached/expired.
   */
  QString getCachedPath(const QString &relativePath) const;

  /**
   * @brief Check cache and initiate async download if file is missing or expired.
   * @param relativePath The virtual path relative to the base.
   * @param suffixes List of file extensions to try (e.g., {"", ".opus", ".wav"})
   */
  void resolveOrDownload(const QString &relativePath, const QStringList &suffixes = {""});

  /**
   * @brief Clears the entire webcache directory.
   */
  void clearCache();

  /**
   * @brief Returns the total size of the webcache directory in bytes.
   */
  qint64 getCacheSize() const;

  /**
   * @brief Checks if a cached file has expired based on its modification time.
   * @param localPath The absolute path to the cached file.
   * @return True if the file is expired, false otherwise.
   */
  bool isExpired(const QString &localPath) const;

  /**
   * @brief Returns the webcache directory path.
   */
  QString cacheDir() const;

private Q_SLOTS:
  void onDownloadFinished(QNetworkReply *reply);

private:
  AOApplication *ao_app;
  QNetworkAccessManager *m_network_manager;

  // Track pending downloads to avoid duplicate requests
  // Key: relative path, Value: list of suffixes being tried
  QHash<QString, bool> m_pending_downloads;

  // Track failed downloads to avoid retrying them repeatedly
  QSet<QString> m_failed_downloads;

  /**
   * @brief Initiates an async download for the given remote URL.
   * @param remoteUrl The full URL to download from.
   * @param localPath The local path where the file should be saved.
   * @param relativePath The relative path key for tracking pending downloads.
   */
  void startDownload(const QString &remoteUrl, const QString &localPath, const QString &relativePath);

  /**
   * @brief Calculates directory size recursively.
   */
  qint64 calculateDirSize(const QDir &dir) const;
};
