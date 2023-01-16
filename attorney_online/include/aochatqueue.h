#ifndef AOCHATQUEUE_H
#define AOCHATQUEUE_H
#include <QQueue>

#include "datatypes.h"

using namespace AttorneyOnline;

/**
 * @brief Small extension of QQueue.
 */
class AOChatQueue : public QQueue<DataTypes::MSPacket> {
public:
  AOChatQueue() = default;
  ~AOChatQueue() = default;
  /**
   * @brief Dequeues all available queue entries.
   *
   * @details This function is only called if objection skip the queue.
   *
   * @return QVector of all pending MS Packets.
   */
  const QVector<DataTypes::MSPacket> dequeueAll()
  {
    QVector<DataTypes::MSPacket> l_packets;
    while (!isEmpty()) {
      l_packets.append(dequeue());
    }
    return l_packets;
  }
};

#endif // AOCHATQUEUE_H)
