#ifndef AOCHATQUEUE_H
#define AOCHATQUEUE_H
#include <QQueue>

#include "datatypes.h"

using namespace AttorneyOnline;

/**
 * @brief Simple wrapper for QQeueue. Just adds a dequeueAll and might become useful in the future.
 */
class AOChatQueue {
public :
    AOChatQueue() = default;
    ~AOChatQueue() = default;

    void enqueue(DataTypes::MSPacket packet);

    /**
     * @brief Dequeues the oldest entry in the queue for viewport processing.
     *
     * @return A single MSPacket that was queued.
     */
    DataTypes::MSPacket dequeue();

    /**
     * @brief Dequeues all available queue entries.
     *
     * @details This function is only called if objection skip the queue.
     *
     * @return
     */
    QVector<DataTypes::MSPacket> dequeueAll();

    /**
     * @brief Checks if we have any pending entries in the queue.
     * @return
     */
    bool isEmpty();

private:
    QQueue<DataTypes::MSPacket> m_ms_queue;
};

#endif // AOCHATQUEUE_H)
