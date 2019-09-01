#include "network/legacyclient.h"

#include "network/hdid.h"

#include <QStringBuilder>
#include <QApplication>

namespace AttorneyOnline {

/*!
 * Composes and sends a message to the server.
 *
 * \param header  the header of the message
 * \param args  a list of arguments
 */
void LegacyClient::send(const QString &header, QStringList args = {})
{
  args.replaceInStrings("#", "<num>")
      .replaceInStrings("%", "<percent>")
      .replaceInStrings("$", "<dollar>")
      .replaceInStrings("&", "<and>");

  auto bytes = (header % args.join('#') % "#%").toUtf8();
  socket.write(bytes, bytes.length());
}

/*!
 * Waits until a message with the specified header is received.
 *
 * \param header  the header to wait for
 * \return a list of parameters sent with the message
 */
QPromise<QStringList> LegacyClient::waitForMessage(const QString &header)
{
  return QPromise<QStringList>(
        [&](const QPromiseResolve<QStringList>& resolve) {

    // Create a wrapper object we can place our connection inside. We don't
    // want to leak memory by having connections that are not needed anymore.
    std::unique_ptr<QObject> wrapper { new QObject };

    auto func = [&](const QString &recvHeader, const QStringList &args) {
      if (recvHeader == header)
      {
        resolve(args);

        // Destroy the wrapper QObject, automatically disconnecting this slot
        wrapper.reset();
      }
    };

    QObject::connect(this, &LegacyClient::messageReceived, wrapper.get(), func);
  });
}

#define ENFORCE_MIN_LENGTH(minimum) \
  if (args.size() < minimum) \
  { \
    qWarning() << "Incoming message" << header << "too short"; \
    return; \
  }

/*!
 * Maps incoming messages to signals.
 */
void LegacyClient::mapSignals()
{
  QObject::connect(this, &LegacyClient::messageReceived,
                   [&](const QString &header, const QStringList &args) {
    if (header == "FL")
    {
      if (args.contains("cccc_ic_support"))
        allowPairing = true;
    }
    else if (header == "SC")
    {
      charsList.clear();
      for (const QString &character : args)
      {
        QStringList charInfo = character.split("&");
        char_type charEntry;
        charEntry.name = charInfo[0];
        charEntry.description = charInfo[1];
        charsList.append(std::move(charEntry));
      }
    }
    else if (header == "CharsCheck")
    {
      for (int i = 0; i < charsList.length() && i < args.length(); i++)
      {
        charsList[i].taken = args[i].toInt() == -1;
      }

      emit takenCharsChanged();
    }
    else if (header == "PV")
    {
      ENFORCE_MIN_LENGTH(3);

      // The only useful thing from this packet is the index of the character
      // assigned. The rest is meaningless.
      bool ok = false;
      currentCharId = args[2].toInt(&ok);

      if (!ok)
      {
        qWarning() << "Server issued us an unknown character ID: " << args[2];
        currentCharId = -1;
      }

      emit characterChanged(currentCharId);
    }
    else if (header == "SM")
    {
      tracksList.clear();
      areasList.clear();

      int musicStart = tracksList.indexOf(QRegExp("(\\=.+|.*\\.(wav|mp3|ogg|opus))"));
      tracksList.append(args.mid(musicStart));

      for (int i = 0; i < musicStart; i++)
      {
        area_type area;
        area.id = i;
        area.name = args[i];
        area.players = 0;
        area.status = "Unknown";
        area.cm = "Unknown";
        area.locked = "Unknown";
        areasList.append(area);
      }

      emit tracksChanged();
    }
    else if (header == "LE")
    {
      evidenceList.clear();
      for (const QString &evi : args)
      {
        QStringList eviInfo = evi.split("&");
        evi_type eviEntry;
        eviEntry.name = eviInfo[0];
        eviEntry.image = eviInfo[1];
        eviEntry.description = eviInfo[2];
        evidenceList.append(eviEntry);
      }

      emit evidenceChanged();
    }
    else if (header == "MS")
    {
      ENFORCE_MIN_LENGTH(15);
      chat_message_type message(args);
      emit icReceived(message);
    }
    else if (header == "CT")
    {
      ENFORCE_MIN_LENGTH(1);

      QString name, message;
      if (args.size() == 1)
      {
        message = args[0];
      }
      else if (args.size() >= 2)
      {
        name = args[0];
        message = args[1];
      }

      emit oocReceived(name, message);
    }
    else if (header == "MC")
    {
      ENFORCE_MIN_LENGTH(1);

      bool ok;
      int charId = args[0].toInt(&ok);

      QString character;
      if (args.size() >= 2)
      {
        character = args[1];
      }
      else if (ok && charId >= 0 && charId < charsList.size())
      {
        character = charsList[charId].name;
      }

      // Trim extension from filename
      QString trackName = args[0].left(args[0].lastIndexOf('.'));

      emit trackChanged(trackName, character);
    }
    else if (header == "HP")
    {
      ENFORCE_MIN_LENGTH(2);
      emit healthChanged(static_cast<HEALTH_TYPE>(args[0].toInt()), args[1].toInt());
    }
    else if (header == "BN")
    {
      ENFORCE_MIN_LENGTH(1);
      emit backgroundChanged(args[0]);
    }
    else if (header == "RT")
    {
      ENFORCE_MIN_LENGTH(1);

      WTCE_TYPE wtce;
      if (args[0] == "testimony1")
        wtce = WITNESS_TESTIMONY;
      else if (args[0] == "testimony2")
        wtce = CROSS_EXAMINATION;
      else if (args[0] == "judgeruling" && args[1] == "0")
        wtce = NOT_GUILTY;
      else if (args[0] == "judgeruling" && args[1] == "1")
        wtce = GUILTY;
      else
      {
        qWarning() << QStringLiteral("unknown WTCE type %1").arg(args[0]);
        return;
      }

      emit wtceReceived(wtce);
    }
    else if (header == "ARUP")
    {
      ENFORCE_MIN_LENGTH(1);

      int updateType = args[0].toInt();

      for (int i = 0; i < areasList.length() && i < args.length() - 1; i++)
      {
        area_type &area = areasList[i];
        const QString &value = args[i + 1];

        switch (updateType)
        {
        case ARUP_PARAM::PLAYERS:
          area.players = value.toInt();
          break;
        case ARUP_PARAM::STATUS:
          area.status = value;
          break;
        case ARUP_PARAM::CM:
          area.cm = value;
          break;
        case ARUP_PARAM::LOCKED:
          area.locked = value;
          break;
        default:
          qWarning() << tr("Unknown ARUP parameter %1").arg(updateType);
          return;
        }
      }

      emit areasUpdated();
    }
    else if (header == "CASEA")
    {
      ENFORCE_MIN_LENGTH(6);

      // (The purpose of this packet is somewhat inane for its complexity.)
      auto casingFlags = casing_flags_to_bitset(
            args[1] == "1",
            args[2] == "1",
            args[3] == "1",
            args[4] == "1",
            args[5] == "1",
            0);
      emit caseCalled(args[0], casingFlags);
    }
    else if (header == "KK")
    {
      const QString msg = args.isEmpty() ? QStringLiteral() : args[0];
      emit kicked(msg, false);
    }
    else if (header == "KB" || header == "BD")
    {
      const QString msg = args.isEmpty() ? QStringLiteral() : args[0];
      emit kicked(msg, true);
    }
    else if (header == "ZZ")
    {
      ENFORCE_MIN_LENGTH(1);
      emit modCalled(args[0]);
    }
  });
}

/*!
 * Called when a TCP packet has been received.
 */
void LegacyClient::packetReceived()
{
  buffer.append(socket.readAll());

  if (buffer.size() >= BUFFER_SOFT_LIMIT)
  {
    qWarning() << QStringLiteral("Buffer reached over %1 bytes (%2 bytes)! "
                                 "Discarding buffer.")
                  .arg(BUFFER_SOFT_LIMIT)
                  .arg(buffer.size());
    buffer.clear();
    return;
  }

  int end;
  while ((end = buffer.indexOf('%')) != -1)
  {
    if (end == 0)
    {
      // Special case of a garbage message - kill with fire
      buffer = buffer.right(end + 1);
      continue;
    }

    QByteArray msg = buffer.left(end - 1); // Ignore trailing "#%"
    QStringList args = QString::fromUtf8(msg).split("#")
        .replaceInStrings("<num>", "#")
        .replaceInStrings("<percent>", "%")
        .replaceInStrings("<dollar>", "$")
        .replaceInStrings("<and>", "&");
    QString header = args.takeFirst();

    emit messageReceived(header, args);

    // (Unfortunately QByteArray is not a circular buffer. You wish it were,
    // though.)
    buffer = buffer.right(end + 1);
  }
}

/*!
 * Establishes a TCP connection to an AO2 server, and performs the usual
 * handshake by sending an HDID to the server and retrieving character
 * and music information.
 *
 * \return a promise that is resolved when the handshake is complete
 */
QPromise<void> LegacyClient::connect()
{
  emit connectProgress(0, 100, tr("Connecting to server..."));

  QObject::connect(&socket, &QTcpSocket::readyRead,
                   this, &LegacyClient::packetReceived);

  // (QTcpSocket::error is overloaded, so we have to select the right one)
  auto errorFunc = static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>
      (&QTcpSocket::error);

  auto promise = QtPromise::connect(&socket, &QTcpSocket::connected, errorFunc)
      .then([&](void) {
    emit connectProgress(10, 100, tr("Getting player information..."));
    // Send HDID. Ignore fantacrypt - no AO server requires communicating
    // over fantacrypt anymore.
    send("HI", { hdid() });
    // TODO: fix application version
    send("ID", { "AO2", QCoreApplication::applicationVersion() });
    return waitForMessage("PN");
  }).then([&] {
    emit connectProgress(15, 100, tr("Getting characters..."));
    send("askchaa");
    return waitForMessage("SI");
  }).then([&] {
    // Once we map the signals, we don't need to parse any more
    // messages explicitly here.
    mapSignals();
    send("RC");
    return waitForMessage("SC");
  }).then([&] {
    emit connectProgress(50, 100, tr("Getting music..."));
    send("RM");
    return waitForMessage("SM");
  }).then([&] {
    emit connectProgress(80, 100, tr("Loading client..."));

    // I am not going to put up with server-side preferences.
    // Filter case announcements on the client side.
    send("SETCASE", {"1", "1", "1", "1", "1", "1"});

    send("RD");
    return waitForMessage("DONE");
  }).then([&] {
    QObject::connect(&keepaliveTimer, &QTimer::timeout,
                     this, &LegacyClient::sendKeepalive);
    keepaliveTimer.setInterval(KEEPALIVE_INTERVAL);
    keepaliveTimer.start();
  });

  // Connect TCP socket, bringing the promise chain above into motion.
  socket.connectToHost(address, port);

  return promise;
}

/*!
 * Sends a keepalive packet to the server to prevent the client from being
 * disconnected automatically.
 */
void LegacyClient::sendKeepalive()
{
  send("CHECK");
}

/*!
 * Joins a room with the specific index. By default, the default room has
 * already been joined. Since the AO1 protocol has no notion of rooms,
 * the currently selected room cannot be tracked.
 *
 * The new room will be joined with the previously selected character.
 */
void LegacyClient::joinRoom(int index)
{
  send("MC", { QString::number(index), "0" });
}

/*!
 * Sets the character being used in the current room.
 *
 * \param character ID; a value of -1 indicates "spectator"
 */
void LegacyClient::setCharacter(int charId)
{
  send("CC", { "0", QString::number(charId), hdid() });
}

/*!
 * Requests a mod with a message.
 *
 * \param message  optional message. Some servers do not support messages
 * in mod calls; in this case, the message will be ignored by the server.
 */
void LegacyClient::callMod(const QString &message)
{
  send("ZZ", { message });
}

/*!
 * Sends a composed in-character chat message.
 *
 * \param message  a well-formed IC message
 * \return promise that resolves when the server confirms that the IC message
 * was received
 */
QPromise<void> LegacyClient::sendIC(const chat_message_type &message)
{
  auto msgCopy = message;
  msgCopy.char_id = currentCharId;

  send("MS", msgCopy);

  return QPromise<void>(
        [&](const QPromiseResolve<void>& resolve) {
    std::unique_ptr<QMetaObject::Connection> connection {
      new QMetaObject::Connection
    };

    *connection = QObject::connect(this, &LegacyClient::icReceived,
                                   [&](const chat_message_type &message) {
      // If you ever design a protocol, don't do this - this is a really bad
      // heuristic. There could be various players in the room with the same
      // character, or your messages might be being intentionally delayed.
      if (message.char_id == currentCharId) {
        QObject::disconnect(*connection);
        resolve();
      }
    });
  }).timeout(IC_ECHO_TIMEOUT);
}

/*!
 * Sends an out-of-character chat message.
 *
 * \param oocName  name to be shown on the OOC chat log
 * \param message  plain-text message to be sent
 */
void LegacyClient::sendOOC(const QString &oocName, const QString &message)
{
  send("CT", { oocName, message });
}

/*!
 * Sends a request to play the Witness Testimony/Cross Examination overlay
 * animation.
 *
 * \param type  the overlay to be played
 */
void LegacyClient::sendWTCE(WTCE_TYPE type)
{
  const QMap<WTCE_TYPE, QStringList> packets = {
    {WITNESS_TESTIMONY, {"testimony1"}},
    {CROSS_EXAMINATION, {"testimony2"}},
    {NOT_GUILTY, {"judgeruling", "0"}},
    {GUILTY, {"judgeruling", "1"}}
  };

  QStringList packet = packets.value(type);
  if (packet.isEmpty())
  {
    qWarning() << "out-of-range WTCE_TYPE" << type;
    return;
  }

  send("RT", packet);
}

/*!
 * Sends a request to modify a health (penalty) bar with a new
 * value.
 *
 * \param type  the health bar to be modified
 * \param value  the new value, from 0 to 10
 */
void LegacyClient::sendHealth(HEALTH_TYPE type, int value)
{
  send("HP", { QString::number(type), QString::number(value) });
}

/*!
 * Adds a new evidence item to the evidence list.
 * \param evidence  a well-formed evidence item
 */
void LegacyClient::addEvidence(const evi_type &evidence)
{
  send("PE", evidence);
}

/*!
 * Replaces an evidence item with a new item.
 *
 * \param index  the index of the evidence item to be replaced
 * \param evidence  the new evidence item
 */
void LegacyClient::editEvidence(const int index, const evi_type &evidence)
{
  QStringList packet = { QString::number(index) };
  packet.append(evidence);

  send("EE", packet);
}

/*!
 * Removes an evidence item.
 *
 * \param index  the index of the evidence item to be removed
 */
void LegacyClient::removeEvidence(const int index)
{
  send("DE", { QString::number(index) });
}

/*!
 * Requests to play a track.
 *
 * \param trackName  the name of the track to be played, including extension
 * \param showname  the name that should appear on the IC log
 */
void LegacyClient::playTrack(const QString &trackName, const QString &showname)
{
  send("MC", { trackName, "0", showname });
}

/*!
 * Announces a case with a list of roles needed to be filled.
 *
 * \param caseTitle  the title of the case
 * \param rolesNeeded  a bitset that controls what recipients will receive
 * the casing announcement, depending on the roles that each player has
 * chosen to receive casing announcements for
 */
void LegacyClient::announceCase(const QString &caseTitle,
                                const std::bitset<CASING_FLAGS_COUNT> &rolesNeeded)
{
  send("CASEA", {
    caseTitle,
    QString::number(rolesNeeded.test(CASING_DEF)),
    QString::number(rolesNeeded.test(CASING_PRO)),
    QString::number(rolesNeeded.test(CASING_JUD)),
    QString::number(rolesNeeded.test(CASING_JUR)),
    QString::number(rolesNeeded.test(CASING_STENO))
  });
}

} // namespace AttorneyOnline
