#include "aopath.h"
#include "options.h"

#include <QStringList>
#include <QFile>
#include <QDebug>

QString AOPath::getUIAsset(const QString &f_asset_name)
{
    QStringList l_paths {
        ":/base/themes/" + Options::getInstance().theme() +"/" + f_asset_name,
        ":/base/themes/default/" + f_asset_name
    };

    if (Options::getInstance().subTheme() == "server") {
        if (!Options::getInstance().serverSubTheme().isEmpty()) {
            l_paths.append(":/base/themes/" + Options::getInstance().theme() +"/" + Options::getInstance().subTheme() + "/" + f_asset_name);
        }
    }
    else {
        l_paths.append(":/base/themes/" + Options::getInstance().theme() +"/" + Options::getInstance().subTheme() + "/" + f_asset_name);
    }

    for(const QString& l_path : qAsConst(l_paths)) {
        if (QFile::exists(l_path)) {
            return l_path;
        }
    }

    // This should never happen as we fallback to default gracefully.
    // Unless someone forgets to update the default theme embedded into the application :^)
    qCritical() << "Unable to locate ui-asset " << f_asset_name;
    return QString();
}
