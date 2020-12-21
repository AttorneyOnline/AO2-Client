#include "network/legacyclient.h"

#include "network/hdid.h"

#include <QStringBuilder>
#include <QApplication>

namespace AttorneyOnline {

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
  QObject::connect(&socket, &LegacySocket::messageReceived,
                   [&](const QString &header, const QStringList &args) {
    if (header == "FL")
    {
      if (args.contains("cccc_ic_support"))
        caseCafeFeatures = true;
    }
    else if (header == "PN")
    {
      ENFORCE_MIN_LENGTH(2)

      curPlayers = args[0].toInt();
      maxPlayers = args[1].toInt();
    }
    else if (header == "SC")
    {
      charsList.clear();
      for (const QString &character : args)
      {
        QStringList charInfo = character.split("&");
        char_type charEntry;
        charEntry.name = charInfo[0];
        if (charInfo.size() > 1)
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
      ENFORCE_MIN_LENGTH(3)

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
    else if (header == "DONE")
    {
      // Usually, this packet is used outside of the handshake process
      // to kick people off the current character.
      currentCharId = -1;
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
        eviEntry.description = eviInfo[1];
        eviEntry.image = eviInfo[2];
        evidenceList.append(eviEntry);
      }

      emit evidenceChanged();
    }
    else if (header == "MS")
    {
      ENFORCE_MIN_LENGTH(15)
      chat_message_type message(args);
      emit icReceived(message);
    }
    else if (header == "CT")
    {
      ENFORCE_MIN_LENGTH(1)

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
      ENFORCE_MIN_LENGTH(2)

      bool ok;
      int charId = args[1].toInt(&ok);

      QString character;
      if (args.size() >= 3)
      {
        // Use custom showname
        character = args[2];
      }
      else if (ok && charId >= 0 && charId < charsList.size())
      {
        // Use character's default name
        character = charsList[charId].name;
      }

      // Trim extension from filename
      QString trackName = args[0].left(args[0].lastIndexOf('.'));

      emit trackChanged(trackName, character);
    }
    else if (header == "HP")
    {
      ENFORCE_MIN_LENGTH(2)
      emit healthChanged(static_cast<HEALTH_TYPE>(args[0].toInt()), args[1].toInt());
    }
    else if (header == "BN")
    {
      ENFORCE_MIN_LENGTH(1)
      emit backgroundChanged(args[0]);
    }
    else if (header == "RT")
    {
      ENFORCE_MIN_LENGTH(1)

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
      ENFORCE_MIN_LENGTH(1)

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
      ENFORCE_MIN_LENGTH(6)

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
      kicked = true;
      emit connectionLost(KICKED, msg);
    }
    else if (header == "KB" || header == "BD")
    {
      const QString msg = args.isEmpty() ? QStringLiteral() : args[0];
      kicked = true;
      emit connectionLost(BANNED, msg);
    }
    else if (header == "ZZ")
    {
      ENFORCE_MIN_LENGTH(1)
      emit modCalled(args[0]);
    }
  });
}

/*!
 * Establishes a TCP connection to an AO2 server, and performs the usual
 * handshake by sending an HDID to the server and retrieving character
 * and music information.
 *
 * \return a promise that is resolved when the handshake is complete
 */
QPromise<void> LegacyClient::connect(const QString &address,
                                     const uint16_t &port,
                                     const bool &probeOnly)
{
  qInfo().noquote() << "Connecting to"
                    << QStringLiteral("%1:%2").arg(address).arg(port)
                    << QStringLiteral("(probe: %1)").arg(probeOnly ? "yes" : "no");

  emit connectProgress(0, 100, tr("Connecting to server..."));

  QObject::connect(&socket, &LegacySocket::connectionLost, this, [&] {
    if (kicked)
      return;
    emit connectionLost(CONNECTION_RESET, QStringLiteral());
  });

  auto promise = socket.connect(address, port)
      .then([&](void) {
    // Once we map the signals, we don't need to parse any more
    // messages explicitly here.
    mapSignals();
    emit connectProgress(10, 100, tr("Getting player information..."));
    // Send HDID. Ignore fantacrypt - no AO server requires communicating
    // over fantacrypt anymore.
    socket.send("HI", { hdid() });
    // TODO: fix application version
    socket.send("ID", { "AO2", QCoreApplication::applicationVersion() });
    return socket.waitForMessage("PN");
  });

  if (probeOnly)
    return promise.then(&QPromise<void>::resolve);

  return promise.then([&] {
    emit connectProgress(15, 100, tr("Getting characters..."));
    socket.send("askchaa");
    return socket.waitForMessage("SI");
  }).then([&] {
    socket.send("RC");
    return socket.waitForMessage("SC");
  }).then([&] {
    emit connectProgress(50, 100, tr("Getting music..."));
    socket.send("RM");
    return socket.waitForMessage("SM");
  }).then([&] {
    emit connectProgress(80, 100, tr("Loading client..."));

    // I am not going to put up with server-side preferences.
    // Filter case announcements on the client side.
    socket.send("SETCASE", {"", "1", "1", "1", "1", "1", "1"});

    socket.send("RD");
    return socket.waitForMessage("DONE");
  }).then([&] {
    QObject::connect(&keepaliveTimer, &QTimer::timeout,
                     this, &LegacyClient::sendKeepalive);
    keepaliveTimer.setInterval(KEEPALIVE_INTERVAL);
    keepaliveTimer.start();
  });
}

/*!
 * Sends a keepalive packet to the server to prevent the client from being
 * disconnected automatically.
 */
void LegacyClient::sendKeepalive()
{
  socket.send("CH");
}

char_type LegacyClient::character()
{
  if (currentCharId >= 0)
  {
    return charsList[currentCharId];
  }
  else
  {
    return { QStringLiteral(), QStringLiteral(), QStringLiteral(), false };
  }
}

/*!
 * Joins a room with the specified name. By default, the default room has
 * already been joined. Since the AO1 protocol has no notion of rooms,
 * the currently selected room cannot be tracked.
 *
 * The new room will be joined with the previously selected character; however,
 * it cannot be guaranteed that the character has not already been taken. In
 * this case, the client's character will be automatically reassigned by the
 * server.
 */
void LegacyClient::joinRoom(QString &name)
{
  socket.send("MC", { name, "0" });
}

/*!
 * Sets the character being used in the current room.
 *
 * \param character ID; a value of -1 indicates "spectator"
 */
void LegacyClient::setCharacter(int charId)
{
  socket.send("CC", { "0", QString::number(charId), hdid() });

  if (charId == -1)
  {
    // Don't wait for response. Server usually does not respond to spectate
    // requests, so just emulate a response.
    // Note that there is a common server-side bug where the character
    // slot is not freed when switching directly to spectator mode, even
    // when the character ID is requested to be switched to -1.
    currentCharId = -1;
    emit characterChanged(currentCharId);
  }
}

/*!
 * Requests a mod with a message.
 *
 * \param message  optional message. Some servers do not support messages
 * in mod calls; in this case, the message will be ignored by the server.
 */
void LegacyClient::callMod(const QString &message)
{
  if (!message.isEmpty()) {
    socket.send("ZZ", { message });
  } else {
    socket.send("ZZ");
  }
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

  socket.send("MS", msgCopy.serialize(caseCafeFeatures));

  return QPromise<void>([&](const QPromiseResolve<void>& resolve) {
    std::shared_ptr<QMetaObject::Connection> connection {
      new QMetaObject::Connection
    };

    *connection = QObject::connect(this, &LegacyClient::icReceived,
                                   [=](const chat_message_type &message) {
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
  socket.send("CT", { oocName, message });
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

  socket.send("RT", packet);
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
  if (value > 10 || value < 0)
    return;

  socket.send("HP", { QString::number(type), QString::number(value) });
}

/*!
 * Adds a new evidence item to the evidence list.
 * \param evidence  a well-formed evidence item
 */
void LegacyClient::addEvidence(const evi_type &evidence)
{
  socket.send("PE", evidence);
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

  socket.send("EE", packet);
}

/*!
 * Removes an evidence item.
 *
 * \param index  the index of the evidence item to be removed
 */
void LegacyClient::removeEvidence(const int index)
{
  socket.send("DE", { QString::number(index) });
}

/*!
 * Requests to play a track.
 *
 * \param trackName  the name of the track to be played, including extension
 * \param showname  the name that should appear on the IC log
 */
void LegacyClient::playTrack(const QString &trackName, const QString &showname)
{
  socket.send("MC", { trackName, "0", showname });
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
  socket.send("CASEA", {
    caseTitle,
    QString::number(rolesNeeded.test(CASING_DEF)),
    QString::number(rolesNeeded.test(CASING_PRO)),
    QString::number(rolesNeeded.test(CASING_JUD)),
    QString::number(rolesNeeded.test(CASING_JUR)),
    QString::number(rolesNeeded.test(CASING_STENO))
  });
}

/*!
 * Gets a player count. This count is usually only updated on initial server
 * connect; additional requests for server info may be required for an
 * up-to-date count.
 */
std::pair<int, int> LegacyClient::playerCount() const
{
  return { curPlayers, maxPlayers };
}

} // namespace AttorneyOnline
