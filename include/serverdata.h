#ifndef SERVERDATA_H
#define SERVERDATA_H

#include <QObject>
#include <QString>
#include <QStringList>

namespace server {
  Q_NAMESPACE

  /// The base feature set that almost all servers are supposed to support.
  enum class BASE_FEATURE_SET {
    YELLOWTEXT,       ///< Yellow text in in-character messages.
                      ///< @since 2.1.0
    FLIPPING,         ///< The ability to mirror a character in-game.
                      ///< @since 2.1.0
    CUSTOMOBJECTIONS, ///< Enables the use of a single custom objection named
                      ///< `custom`.
                      ///< @since 2.1.0
    FASTLOADING,  ///< Enables the use of "fast loading" instead of the legacy
                  ///< loading protocol.
                  ///< @since 2.1.0
    NOENCRYPTION, ///< Disables "FantaCrypt" for the remainder of  the session.
                  ///< @since 2.1.0
    DESKMOD,  ///< Allows forcing the appearance or disappearance of the desk.
              ///< @since 2.3 to 2.5
    EVIDENCE, ///< Allows creating and presenting evidence.
              ///< @since 2.3 to 2.5
    CCCC_IC_SUPPORT, ///< The ability to pair up with another user's character,
                     ///< the ability to change one's displayed name
                     ///< in-character ("showname"), and the ability to perform
                     ///< "immediate" preanimations (ones that happen alongside
                     ///< text display).
                     ///< @since 2.6.0
    ARUP, ///< Areas have more data about them than just their name (including
          ///< their status, their "lockedness", and who is the Case Master in
          ///< the area), and sets the client up to receive and send ARUP
          ///< packets.
          ///< @since 2.6.0
    CASING_ALERTS, ///< The client gains a new window to announce cases with,
                   ///< and the settings to set itself up to receive case alerts
                   ///< based on casing preferences. No longer used.
                   ///< @since 2.6.0
    MODCALL_REASON, ///< Enables entering a custom reason for calling
                    ///< moderators.
                    ///< @since 2.6.0
    LOOPING_SFX,    ///< Enables looping SFX extensions for the in-character
                    ///< command.
                    ///< @since 2.8.0
    ADDITIVE, ///< Enables "additive" text that allows in-character messages to
              ///< concatenate to the previous one sent by the same character.
              ///< @since 2.8.0
    EFFECTS,  ///< Enables effect overlays.
              ///< @since 2.8.0
    Y_OFFSET, ///< Enables support for vertical offsets.
              ///< @since 2.9.0
    EXPANDED_DESK_MODS, ///< Enables desk modifiers 2 through 5.
                        ///< @since 2.9.0
    AUTH_PACKET,        ///< Enables the use of the AUTH packet.
                        ///< @since 2.9.1
    PREZOOM             ///< Preanim zoom.
  };
  Q_ENUM_NS(BASE_FEATURE_SET)

  /**
   * @brief Arranges data about the server the client is connected to.
   */
  class ServerData {
  public:
    /**
     * @brief Returns true if the feature exists on the server.
     *
     * @param f_feature The feature to check for. Case insensitive.
     *
     * @return True if the feature exists on the server.
     */
    bool get_feature(const BASE_FEATURE_SET &f_feature) const;

    /**
     * @brief Returns true if the feature exists on the server.
     *
     * @param f_feature The feature to check for. Case insensitive.
     *
     * @return True if the feature exists on the server.
     */
    bool get_feature(const QString &f_feature) const;

    /**
     * @brief Sets the feature list, overwriting the existing one.
     *
     * @param f_feature_list The new feature list of the server.
     */
    void set_features(const QStringList &f_feature_list);

    /**
     * @brief Self-explanatory: gets the software the server is running on.
     *
     * @return As brief description.
     */
    QString get_server_software() const;

    /**
     * @brief Self-explanatory: setter for server software.
     *
     * @param f_software The new server software.
     */
    void set_server_software(const QString &f_software);

    /**
     * @brief Getter for the server's asset URL.
     *
     * @return As brief description.
     *
     * @see m_asset_url
     */
    QString get_asset_url() const;

    /**
     * @brief Attempts to set the new asset URL for the server.
     *
     * @details The function converts the sole parameter into UTF-8, then
     * attempts to percent decode it. The  URL is only set if all these steps
     * successfully happen.
     *
     * @param f_asset_url A QString that contains the URL.
     */
    void set_asset_url(const QString &f_asset_url);

  private:
    /// The features available on the server. Determines what
    QStringList m_features;

    /// The software the server is running.
    QString m_server_software;

    /**
     * @brief A QString of an URL that defines the content repository
     *        send by the server.
     *
     * @details Introduced in Version 2.9.2.
     *        Addresses the issue of contenturl devlivery for WebAO
     *        without relying on someone adding the link manually.
     */
    QString m_asset_url;
  };
} // namespace server

#endif // SERVERDATA_H
