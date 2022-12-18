#ifndef HARDWARE_FUNCTIONS_H
#define HARDWARE_FUNCTIONS_H

#include <QString>

namespace AttorneyOnline {

/*!
 * \brief Returns the hard drive ID of the client from the operating system.
 *
 * The HDID system remains one of the most controversial features of AO. Used
 * by Fanat in the closed-source AO1 to keep a tight grip on his community,
 * OmniTroid was forced to continue the "tradition" when he wanted the AO2
 * client to connect to AO1 servers (as AO1, at the time, was the norm, and
 * AO2 was completely unheard of).
 *
 * On one hand, it is a simple deterrent against ban evaders who think they can
 * get away with simply switching IP addresses; on the other hand, the scheme
 * has been broken in various ways since the creation of webAO. Browsers do not
 * allow users to be easily uniquely identified, making it either too easy
 * to ban an unintended range of users, or defeating the effectiveness of
 * HDID bans altogether (since users can simply hop on incognito mode).
 *
 * While the scheme may enjoy some practical security, it has absolutely no
 * technical security and is extremely easy to circumvent. As such, the code
 * is preserved here for backward compatibility purposes.
 *
 * Look here for new ideas:
 * http://n00bworld.com/blog/2019/04/using-openpgp-for-video-games/
 */
QString hdid();

} // namespace AttorneyOnline

#endif // HARDWARE_FUNCTIONS_H
