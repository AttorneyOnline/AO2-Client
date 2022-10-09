#ifndef AOPATH_H
#define AOPATH_H

#include <QString>
#include "datatypes.h"
/**
 * @brief Main Pathing Class
 *
 * @details AOPath is the main asset pathing class,
 * handling both Qt'S Ressource System and AO's VPath Mounting system.
 */
class AOPath {

public:
    /**
     * @brief getUIAsset Locates the requested asset.
     * @param asset_name The filename of the requested ui asset.
     * @return The full path to the requested ui asset.
     *
     * @details This function considers theme inheritance and will always prefer the active subtheme over the
     * select theme or default theme.
     */
    QString getUIAsset(const QString& f_asset_name) const;

    pos_size_type getDesignDimensions(const QString& f_asset_name, const QString f_key) const;
};

#endif //AOPATH_H
