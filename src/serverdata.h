#ifndef SERVERDATA_H
#define SERVERDATA_H

#include <QObject>
#include <QString>
#include <QStringList>

namespace server
{
Q_NAMESPACE

/**
 * @brief Arranges data about the server the client is connected to.
 */
class ServerData
{
public:
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
