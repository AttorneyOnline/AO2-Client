#include "package_installer.h"

#include "archive_extractor.h"
#include "file_functions.h"
#include "mount_manager.h"
#include "package_metadata.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProgressDialog>
#include <QPushButton>
#include <QRegularExpression>
#include <QTemporaryDir>
#include <QUrl>
#include <QUuid>
#include <QVersionNumber>

#include <memory>

namespace {
const QStringList &contentFolderNames()
{
  static const QStringList names = {
      QStringLiteral("characters"), QStringLiteral("themes"),
      QStringLiteral("sounds"),     QStringLiteral("background"),
      QStringLiteral("evidence"),   QStringLiteral("misc"),
      QStringLiteral("overlays"),   QStringLiteral("fonts"),
      QStringLiteral("custom sets")};
  return names;
}

// A bare archive has no recognized content folder, so its content type has to
// be inferred from the files themselves. Only unambiguous single-type folders
// can be installed this way; mixed loose content is refused.
enum class BareLayout {
  None,
  Characters,
  Background,
  SoundsGeneral,
  SoundsTree,
  Mixed
};

enum class BareKind { None, Characters, Background, Sounds };

struct PackageRoot {
  QString path;
  BareLayout layout = BareLayout::None;
};

bool isJunkFolder(const QString &name)
{
  return name.compare(QStringLiteral("__MACOSX"), Qt::CaseInsensitive) == 0 ||
         name.compare(QStringLiteral(".git"), Qt::CaseInsensitive) == 0 ||
         name.compare(QStringLiteral(".svn"), Qt::CaseInsensitive) == 0;
}

bool hasContentFolder(const QString &path)
{
  const QStringList folders = QDir(path).entryList(
      QDir::Dirs | QDir::NoDotAndDotDot, QDir::NoSort);
  for (const QString &folder : folders) {
    if (contentFolderNames().contains(folder, Qt::CaseInsensitive)) {
      return true;
    }
  }
  return false;
}

bool isAudioFile(const QString &fileName)
{
  static const QStringList suffixes = {
      QStringLiteral("opus"), QStringLiteral("ogg"), QStringLiteral("wav"),
      QStringLiteral("mp3"),  QStringLiteral("flac"), QStringLiteral("m4a"),
      QStringLiteral("aac")};
  return suffixes.contains(QFileInfo(fileName).suffix().toLower());
}

bool isPackageMetadataFile(const QString &fileName)
{
  return fileName.compare(QStringLiteral("package.ini"), Qt::CaseInsensitive) ==
             0 ||
         fileName.compare(QStringLiteral("icon.png"), Qt::CaseInsensitive) ==
             0 ||
         fileName.compare(QStringLiteral("package.png"), Qt::CaseInsensitive) ==
             0;
}

bool isBackgroundPositionName(const QString &stem)
{
  static const QStringList stems = {
      QStringLiteral("defensedesk"),    QStringLiteral("defenseempty"),
      QStringLiteral("helperdesk"),     QStringLiteral("helperstand"),
      QStringLiteral("judgedesk"),      QStringLiteral("judgestand"),
      QStringLiteral("prosecutiondesk"), QStringLiteral("prosecutorempty"),
      QStringLiteral("seancedesk"),     QStringLiteral("seancestand"),
      QStringLiteral("stand"),          QStringLiteral("witnessempty")};
  return stems.contains(stem, Qt::CaseInsensitive);
}

BareLayout detectBareLayout(const QString &path)
{
  const QDir folder(path);
  const QFileInfoList files =
      folder.entryInfoList(QDir::Files | QDir::Hidden | QDir::System,
                           QDir::NoSort);
  const QStringList folders =
      folder.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::NoSort);

  bool characters = false;
  bool background = false;
  bool sounds = false;

  int positionImages = 0;
  QStringList seenPositions;
  bool directAudio = false;
  bool directAudioOnly = true;
  bool soundFolders = false;

  for (const QFileInfo &file : files) {
    const QString name = file.fileName();
    if (name.startsWith(QLatin1Char('.'))) {
      continue;
    }

    if (name.compare(QStringLiteral("char.ini"), Qt::CaseInsensitive) == 0) {
      characters = true;
    }

    if (name.compare(QStringLiteral("design.ini"), Qt::CaseInsensitive) == 0 ||
        name.compare(QStringLiteral("positions.ini"), Qt::CaseInsensitive) == 0) {
      background = true;
    }

    const QString stem = file.completeBaseName().toLower();
    if (isBackgroundPositionName(stem) && !seenPositions.contains(stem)) {
      seenPositions.append(stem);
      ++positionImages;
    }

    if (isPackageMetadataFile(name)) {
      continue;
    }

    if (isAudioFile(name)) {
      directAudio = true;
    }
    else {
      directAudioOnly = false;
    }
  }

  if (!background && positionImages >= 2) {
    background = true;
  }

  if (directAudio && directAudioOnly) {
    sounds = true;
  }

  for (const QString &name : folders) {
    if (name.compare(QStringLiteral("general"), Qt::CaseInsensitive) == 0 ||
        name.compare(QStringLiteral("music"), Qt::CaseInsensitive) == 0 ||
        name.compare(QStringLiteral("blips"), Qt::CaseInsensitive) == 0) {
      soundFolders = true;
      break;
    }
  }
  if (soundFolders) {
    sounds = true;
  }

  const int matches = (characters ? 1 : 0) + (background ? 1 : 0) +
                      (sounds ? 1 : 0);
  if (matches > 1) {
    return BareLayout::Mixed;
  }
  if (characters) {
    return BareLayout::Characters;
  }
  if (background) {
    return BareLayout::Background;
  }
  if (sounds) {
    return soundFolders ? BareLayout::SoundsTree : BareLayout::SoundsGeneral;
  }
  return BareLayout::None;
}

BareKind kindOf(BareLayout layout)
{
  switch (layout) {
  case BareLayout::Characters:
    return BareKind::Characters;
  case BareLayout::Background:
    return BareKind::Background;
  case BareLayout::SoundsGeneral:
  case BareLayout::SoundsTree:
    return BareKind::Sounds;
  default:
    return BareKind::None;
  }
}

QString bareKindName(BareKind kind)
{
  switch (kind) {
  case BareKind::Characters:
    return QObject::tr("character");
  case BareKind::Background:
    return QObject::tr("background");
  case BareKind::Sounds:
    return QObject::tr("sound");
  default:
    return {};
  }
}

bool collectPackageRoots(const QString &path, QList<PackageRoot> &roots,
                         QString *error)
{
  if (hasContentFolder(path)) {
    roots.append({QDir::cleanPath(path), BareLayout::None});
    return true;
  }

  const BareLayout layout = detectBareLayout(path);
  if (layout == BareLayout::Mixed) {
    if (error != nullptr) {
      *error = QObject::tr(
          "This archive's loose folders mix character, background, or sound "
          "content, so it isn't clear where to install them. Put the content "
          "inside explicit folders such as characters/ or background/, or "
          "install the content in separate archives.");
    }
    return false;
  }
  if (layout != BareLayout::None) {
    roots.append({QDir::cleanPath(path), layout});
    return true;
  }

  const QFileInfoList folders = QDir(path).entryInfoList(
      QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
  for (const QFileInfo &folder : std::as_const(folders)) {
    if (isJunkFolder(folder.fileName())) {
      continue;
    }
    if (!collectPackageRoots(folder.absoluteFilePath(), roots, error)) {
      return false;
    }
  }
  return true;
}

QString sanitizePackageName(QString name)
{
  name = name.trimmed();
  name.replace(QRegularExpression(QStringLiteral(
                   "[\\\\/:*?\"<>|\\x{0000}-\\x{001F}]")),
               QStringLiteral("_"));
  name = name.simplified();

  while (name.endsWith(QLatin1Char('.')) ||
         name.endsWith(QLatin1Char(' '))) {
    name.chop(1);
  }

  if (name.isEmpty() || name == QStringLiteral(".") ||
      name == QStringLiteral("..")) {
    return {};
  }

  // Windows reserved device names are invalid as folder names.
  const QString stem = name.section(QLatin1Char('.'), 0, 0).toUpper();
  static const QRegularExpression reserved(
      QStringLiteral("^(CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])$"));
  if (reserved.match(stem).hasMatch()) {
    name.prepend(QLatin1Char('_'));
  }

  return name.left(120);
}

QString packageNameFor(const QString &root, const QString &stagingRoot,
                       const QString &archiveName,
                       const PackageMetadata &metadata)
{
  QString name = metadata.name;
  if (name.isEmpty()) {
    const QFileInfo rootInfo(root);
    const bool isStagingRoot =
        QDir::cleanPath(rootInfo.absoluteFilePath()) ==
        QDir::cleanPath(QFileInfo(stagingRoot).absoluteFilePath());
    const QString folderName = rootInfo.fileName();

    if (isStagingRoot ||
        folderName.compare(QStringLiteral("base"), Qt::CaseInsensitive) == 0) {
      name = archiveName;
    }
    else {
      name = folderName;
    }
  }

  return sanitizePackageName(name);
}

bool parseVersion(const QString &text, QVersionNumber *version)
{
  const QString trimmed = text.trimmed();
  if (trimmed.isEmpty()) {
    return false;
  }

  qsizetype suffixIndex = -1;
  const QVersionNumber parsed =
      QVersionNumber::fromString(trimmed, &suffixIndex);
  if (parsed.isNull() || suffixIndex != trimmed.size()) {
    return false;
  }

  *version = parsed;
  return true;
}

QString versionDescription(const PackageMetadata &installed,
                           const PackageMetadata &incoming)
{
  const QString installedVersion = installed.version.trimmed();
  const QString incomingVersion = incoming.version.trimmed();

  if (installedVersion.isEmpty() && incomingVersion.isEmpty()) {
    return {};
  }

  QStringList lines;
  lines.append(installedVersion.isEmpty()
                   ? QObject::tr("Installed version: unknown")
                   : QObject::tr("Installed version: %1").arg(installedVersion));
  lines.append(incomingVersion.isEmpty()
                   ? QObject::tr("Archive version: unknown")
                   : QObject::tr("Archive version: %1").arg(incomingVersion));

  if (installedVersion.isEmpty() || incomingVersion.isEmpty()) {
    return lines.join(QLatin1Char('\n'));
  }

  if (installedVersion == incomingVersion) {
    lines.append(QObject::tr("The versions are the same."));
    return lines.join(QLatin1Char('\n'));
  }

  QVersionNumber installedParsed;
  QVersionNumber incomingParsed;
  if (parseVersion(installedVersion, &installedParsed) &&
      parseVersion(incomingVersion, &incomingParsed)) {
    const int comparison =
        QVersionNumber::compare(incomingParsed, installedParsed);
    if (comparison > 0) {
      lines.append(
          QObject::tr("This archive is newer than the installed package."));
    }
    else if (comparison < 0) {
      lines.append(
          QObject::tr("This archive is older than the installed package."));
    }
    else {
      lines.append(QObject::tr("The versions are the same."));
    }
  }
  else {
    lines.append(QObject::tr(
        "The versions differ and can't be compared automatically."));
  }

  return lines.join(QLatin1Char('\n'));
}

enum class DuplicateChoice { Replace, KeepBoth, Cancel };

DuplicateChoice askDuplicate(QWidget *parent, const QString &name,
                             const PackageMetadata &installed,
                             const PackageMetadata &incoming)
{
  QMessageBox box(parent);
  box.setIcon(QMessageBox::Question);
  box.setWindowTitle(QObject::tr("Package already installed"));
  box.setText(
      QObject::tr("A package named \"%1\" is already installed.").arg(name));

  QString information = versionDescription(installed, incoming);
  if (!information.isEmpty()) {
    information += QStringLiteral("\n\n");
  }
  information += QObject::tr(
      "Replace removes the installed package folder and uses this archive "
      "instead.\nKeep both installs this archive under a new name.");

  box.setInformativeText(information);

  auto *replaceButton =
      box.addButton(QObject::tr("Replace"), QMessageBox::DestructiveRole);
  auto *keepBothButton =
      box.addButton(QObject::tr("Keep both"), QMessageBox::ActionRole);
  auto *cancelButton = box.addButton(QMessageBox::Cancel);
  box.setDefaultButton(keepBothButton);
  box.setEscapeButton(cancelButton);

  box.exec();

  if (box.clickedButton() == replaceButton) {
    return DuplicateChoice::Replace;
  }
  if (box.clickedButton() == keepBothButton) {
    return DuplicateChoice::KeepBoth;
  }
  return DuplicateChoice::Cancel;
}

QString uniquePackageName(const QString &packagesPath, const QString &name)
{
  QString candidate = name;
  int suffix = 2;
  while (QFileInfo::exists(QDir(packagesPath).filePath(candidate))) {
    candidate = QStringLiteral("%1 (%2)").arg(name).arg(suffix++);
  }
  return candidate;
}

bool prepareBarePackage(const QString &root, const QString &stagingRoot,
                        const QString &archiveName, BareLayout layout,
                        const QString &packageRoot,
                        const PackageMetadata &metadata, QString *error)
{
  const auto fail = [error](const QString &message) {
    if (error != nullptr) {
      *error = message;
    }
    return false;
  };

  if (!QDir().mkpath(packageRoot)) {
    return fail(QObject::tr("Couldn't create a folder for the package."));
  }

  // Metadata belongs at the package root. For bare layouts that folder is also
  // the content folder, so move it up before the content is wrapped.
  const QString iniSource =
      QDir(root).filePath(QStringLiteral("package.ini"));
  if (QFileInfo(iniSource).isFile()) {
    QString moveError;
    if (!moveFileInto(iniSource,
                      QDir(packageRoot).filePath(QStringLiteral("package.ini")),
                      &moveError)) {
      return fail(moveError);
    }
  }

  // Keep the icon relative to the package root, matching normal packages.
  if (!metadata.icon.isEmpty()) {
    const QString iconSource = QDir(root).filePath(metadata.icon);
    const QString iconTarget = QDir(packageRoot).filePath(metadata.icon);
    if (QFileInfo(iconSource).isFile()) {
      if (!QDir().mkpath(QFileInfo(iconTarget).absolutePath()) ||
          !QFile::copy(iconSource, iconTarget)) {
        qWarning() << "Couldn't copy package icon" << iconSource << "to"
                   << iconTarget;
      }
    }
  }

  const bool isStagingRoot =
      QDir::cleanPath(QFileInfo(root).absoluteFilePath()) ==
      QDir::cleanPath(QFileInfo(stagingRoot).absoluteFilePath());

  QString contentPath;
  QString entryName;
  bool moveContents = false;

  switch (layout) {
  case BareLayout::Characters:
    contentPath = QStringLiteral("characters");
    break;
  case BareLayout::Background:
    contentPath = QStringLiteral("background");
    break;
  case BareLayout::SoundsGeneral:
    contentPath = QStringLiteral("sounds/general");
    moveContents = true;
    break;
  case BareLayout::SoundsTree:
    contentPath = QStringLiteral("sounds");
    moveContents = true;
    break;
  default:
    return fail(QObject::tr("Couldn't determine where to install this content."));
  }

  if (!moveContents) {
    if (isStagingRoot) {
      entryName = archiveName;
    }
    else {
      entryName = QFileInfo(root).fileName();
      if (entryName.compare(QStringLiteral("base"), Qt::CaseInsensitive) == 0) {
        entryName = archiveName;
      }
    }
    moveContents = true;

    entryName = sanitizePackageName(entryName);
    if (entryName.isEmpty()) {
      entryName = sanitizePackageName(archiveName);
    }
    if (entryName.isEmpty()) {
      entryName = QStringLiteral("package");
    }
  }

  QString destination = QDir(packageRoot).filePath(contentPath);
  if (!entryName.isEmpty()) {
    destination = QDir(destination).filePath(entryName);
  }

  if (!QDir().mkpath(QFileInfo(destination).absolutePath())) {
    return fail(QObject::tr("Couldn't create a content folder for the package."));
  }

  if (moveContents) {
    return moveDirectoryContentsInto(root, destination, error);
  }
  return moveDirectoryInto(root, destination, error);
}

bool replacePackage(const QString &packagesPath, const QString &root,
                    const QString &target, QString *error)
{
  const QString backupPath =
      QDir(packagesPath)
          .filePath(QStringLiteral(".replace-%1")
                        .arg(QUuid::createUuid().toString(
                            QUuid::WithoutBraces)));

  if (!QDir().rename(target, backupPath)) {
    if (error != nullptr) {
      *error = QObject::tr("Couldn't prepare the installed package for replacement.");
    }
    return false;
  }

  QString moveError;
  if (!moveDirectoryInto(root, target, &moveError)) {
    QDir().rename(backupPath, target);
    if (error != nullptr) {
      *error = moveError;
    }
    return false;
  }

  QDir(backupPath).removeRecursively();
  return true;
}

} // namespace

PackageInstaller::PackageInstaller(MountManager *mounts, QObject *parent)
    : QObject(parent), m_mounts(mounts),
      m_network(new QNetworkAccessManager(this))
{
}

void PackageInstaller::installArchives(const QStringList &archivePaths,
                                       QWidget *parent,
                                       InstallTarget target)
{
  if (m_busy || m_mounts == nullptr || archivePaths.isEmpty()) {
    return;
  }

  m_busy = true;
  const QString packagesPath = m_mounts->packagesPath();
  const bool installOverBase = target == InstallTarget::Base;
  const QString installBase = installOverBase ? get_base_path() : packagesPath;
  if (installOverBase && !QDir().mkpath(installBase)) {
    m_busy = false;
    QMessageBox::warning(parent, QObject::tr("Content installation"),
                         QObject::tr("Couldn't access the base folder."));
    return;
  }
  QStringList installedPackages;
  QStringList errors;

  for (const QString &archivePath : archivePaths) {
    const QFileInfo archiveInfo(archivePath);
    const QString archiveName = archiveInfo.completeBaseName();
    const QString fileName = archiveInfo.fileName();

    QTemporaryDir staging(
        QDir(installOverBase ? installBase : packagesPath)
            .filePath(QStringLiteral(".install-XXXXXX")));
    if (!staging.isValid()) {
      errors.append(QObject::tr("%1: couldn't create a temporary folder.")
                        .arg(fileName));
      continue;
    }

    QProgressDialog progress(
        QObject::tr("Extracting %1...").arg(fileName), QObject::tr("Cancel"), 0,
        0, parent);
    progress.setWindowTitle(installOverBase ? QObject::tr("Installing content")
                                            : QObject::tr("Installing package"));
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.show();
    QCoreApplication::processEvents();

    QString extractError;
    const bool extracted = extractArchive(
        archivePath, staging.path(), &extractError,
        [&progress, &fileName](const QString &entry) {
          progress.setLabelText(
              QObject::tr("Extracting %1...\n%2").arg(fileName, entry));
          QCoreApplication::processEvents();
          return !progress.wasCanceled();
        });

    const bool wasCancelled = progress.wasCanceled();
    progress.close();

    if (!extracted) {
      if (wasCancelled) {
        errors.append(QObject::tr("%1: installation cancelled.").arg(fileName));
        break;
      }
      errors.append(QObject::tr("%1: %2").arg(fileName, extractError));
      continue;
    }

    QList<PackageRoot> roots;
    QString detectionError;
    if (!collectPackageRoots(staging.path(), roots, &detectionError)) {
      errors.append(QObject::tr("%1: %2").arg(fileName, detectionError));
      continue;
    }
    if (roots.isEmpty()) {
      errors.append(QObject::tr(
          "%1: this archive doesn't contain a recognized package folder or "
          "recognizable loose character, background, or sound content.")
                        .arg(fileName));
      continue;
    }

    if (installOverBase) {
      bool merged = true;
      for (const PackageRoot &candidate : std::as_const(roots)) {
        QString moveError;
        if (!moveDirectoryContentsInto(candidate.path, installBase, &moveError,
                                       true)) {
          errors.append(QObject::tr("%1: %2").arg(fileName, moveError));
          merged = false;
        }
      }
      if (merged)
        installedPackages.append(archiveName);
      continue;
    }

    // Loose content has no wrapper folder, so all of it in one archive must
    // infer to the same content type. Wrapped packages may freely mix types.
    QStringList looseKinds;
    for (const PackageRoot &root : std::as_const(roots)) {
      const QString kind = bareKindName(kindOf(root.layout));
      if (!kind.isEmpty() && !looseKinds.contains(kind)) {
        looseKinds.append(kind);
      }
    }
    if (looseKinds.size() > 1) {
      errors.append(
          QObject::tr("%1: this archive mixes loose %2 content. Loose content "
                      "must be all one type, or placed inside explicit content "
                      "folders.")
              .arg(fileName, looseKinds.join(QObject::tr(" and "))));
      continue;
    }

    for (const PackageRoot &candidate : std::as_const(roots)) {
      const QString root = candidate.path;
      const PackageMetadata metadata = readPackageMetadata(root);
      const QString rawName = packageNameFor(root, staging.path(), archiveName,
                                             metadata);
      if (rawName.isEmpty()) {
        errors.append(QObject::tr(
            "%1: couldn't work out a valid package name for %2.")
                          .arg(fileName, QFileInfo(root).fileName()));
        continue;
      }

      QString targetName = rawName;
      QString targetPath = QDir(packagesPath).filePath(targetName);
      bool replace = false;

      if (QFileInfo::exists(targetPath)) {
        const PackageMetadata installed = readPackageMetadata(targetPath);
        const DuplicateChoice choice =
            askDuplicate(parent, targetName, installed, metadata);

        if (choice == DuplicateChoice::Cancel) {
          continue;
        }
        if (choice == DuplicateChoice::KeepBoth) {
          targetName = uniquePackageName(packagesPath, targetName);
          targetPath = QDir(packagesPath).filePath(targetName);
        }
        else {
          replace = true;
        }
      }

      QTemporaryDir normalized(
          QDir(packagesPath).filePath(QStringLiteral(".package-XXXXXX")));
      QString installRoot = root;
      if (candidate.layout != BareLayout::None) {
        if (!normalized.isValid()) {
          errors.append(QObject::tr("%1: couldn't create a temporary folder.")
                            .arg(targetName));
          continue;
        }

        installRoot = normalized.filePath(QStringLiteral("package"));
        QString prepareError;
        if (!prepareBarePackage(root, staging.path(), archiveName,
                                candidate.layout, installRoot, metadata,
                                &prepareError)) {
          errors.append(
              QObject::tr("%1: %2").arg(targetName, prepareError));
          continue;
        }
      }

      QString moveError;
      const bool moved = replace
                             ? replacePackage(packagesPath, installRoot,
                                             targetPath, &moveError)
                             : moveDirectoryInto(installRoot, targetPath,
                                                 &moveError);

      if (!moved) {
        errors.append(
            QObject::tr("%1: %2").arg(targetName, moveError));
        continue;
      }

      installedPackages.append(targetName);
    }
  }

  if (!installOverBase)
    m_mounts->rescanNow();
  m_busy = false;

  QString installedText;
  if (!installedPackages.isEmpty()) {
    installedText = QObject::tr("Installed:\n%1")
                        .arg(installedPackages.join(QLatin1Char('\n')));
  }

  const QString title = installOverBase
                            ? QObject::tr("Content installation")
                            : QObject::tr("Package installation");
  const QString successTitle =
      installOverBase ? QObject::tr("Content installed")
                      : QObject::tr("Package installed");

  if (!errors.isEmpty()) {
    QString text = errors.join(QStringLiteral("\n\n"));
    if (!installedText.isEmpty()) {
      text.prepend(installedText + QStringLiteral("\n\n"));
    }
    QMessageBox::warning(parent, title, text);
  }
  else if (!installedText.isEmpty()) {
    QMessageBox::information(parent, successTitle, installedText);
  }

  if (installOverBase) {
    if (!installedPackages.isEmpty())
      emit baseChanged();
  }
  else if (!installedPackages.isEmpty()) {
    emit packagesChanged();
  }
}

void PackageInstaller::installArchiveFromUrl(const QString &url,
                                               QWidget *parent,
                                               InstallTarget target)
{
  if (m_busy) {
    return;
  }

  m_busy = true;

  auto tempDir = std::make_shared<QTemporaryDir>(
      QDir(QDir::tempPath()).filePath(QStringLiteral("archive-XXXXXX")));
  if (!tempDir->isValid()) {
    m_busy = false;
    QMessageBox::warning(parent, QObject::tr("Installation"),
                         QObject::tr("Couldn't create a temporary folder."));
    return;
  }

  auto file = std::make_shared<QFile>(
      tempDir->filePath(QStringLiteral("download")));
  if (!file->open(QIODevice::WriteOnly)) {
    m_busy = false;
    QMessageBox::warning(parent, QObject::tr("Installation"),
                         QObject::tr("Couldn't save the downloaded archive."));
    return;
  }

  QNetworkRequest request{QUrl(url)};
  request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                       QNetworkRequest::NoLessSafeRedirectPolicy);
  request.setHeader(QNetworkRequest::UserAgentHeader,
                    QStringLiteral("Attorney Online"));
  QNetworkReply *reply = m_network->get(request);

  auto *progress = new QProgressDialog(QObject::tr("Downloading..."),
                                       QObject::tr("Cancel"), 0, 0, parent);
  progress->setWindowTitle(QObject::tr("Installing"));
  progress->setWindowModality(Qt::WindowModal);
  progress->setMinimumDuration(0);
  progress->show();

  connect(reply, &QNetworkReply::readyRead, this,
          [reply, file] { file->write(reply->readAll()); });
  connect(reply, &QNetworkReply::downloadProgress, progress,
          [progress](qint64 received, qint64 total) {
            if (total > 0) {
              progress->setMaximum(static_cast<int>(total));
              progress->setValue(static_cast<int>(received));
            }
            progress->setLabelText(
                QObject::tr("Downloading... %1 MB")
                    .arg(received / (1024.0 * 1024.0), 0, 'f', 1));
          });
  connect(progress, &QProgressDialog::canceled, reply,
          &QNetworkReply::abort);

  connect(reply, &QNetworkReply::finished, this,
          [this, reply, file, progress, tempDir, parent, target] {
            file->write(reply->readAll());
            file->close();

            const bool cancelled = progress->wasCanceled();
            progress->close();
            progress->deleteLater();

            const QString archivePath = file->fileName();
            const bool downloadOk =
                reply->error() == QNetworkReply::NoError && !cancelled;
            const QString errorString = reply->errorString();
            reply->deleteLater();

            m_busy = false;

            if (!downloadOk) {
              if (!cancelled) {
                QMessageBox::warning(
                    parent, QObject::tr("Installation"),
                    QObject::tr("Couldn't download the archive.\n\n%1")
                        .arg(errorString));
              }
              return;
            }

            installArchives({archivePath}, parent, target);
          });
}
