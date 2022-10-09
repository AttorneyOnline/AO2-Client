#include "aopath.h"
#include "options.h"

#include <QStringList>
#include <QFile>
#include <QDebug>

QString AOPath::getUIAsset(const QString &f_asset_name) const
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
    qCritical() << "Unable to locate ui-asset" << f_asset_name;
    return QString();
}

pos_size_type AOPath::getDesignDimensions(const QString &f_asset_name, const QString f_key) const
{
    //Default return
    pos_size_type l_element_dimensions{0,0,-1,-1};

    QSettings l_design_file(getUIAsset(f_asset_name), QSettings::IniFormat);
    l_design_file.setIniCodec("UTF-8");
    QStringList sub_line_elements = l_design_file.value(f_key).toStringList();

    if (sub_line_elements.size() < 4)
      return l_element_dimensions;

    l_element_dimensions.x = sub_line_elements.at(0).toInt();
    l_element_dimensions.y = sub_line_elements.at(1).toInt();
    l_element_dimensions.width = sub_line_elements.at(2).toInt();
    l_element_dimensions.height = sub_line_elements.at(3).toInt();

    return l_element_dimensions;
}
