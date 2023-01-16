#include "aochatqueue.h"

void AOChatQueue::enqueue(DataTypes::MSPacket packet)
{
    m_ms_queue.enqueue(packet);
}

DataTypes::MSPacket AOChatQueue::dequeue()
{
    return m_ms_queue.dequeue();
}

QVector<DataTypes::MSPacket> AOChatQueue::dequeueAll()
{
    QVector<DataTypes::MSPacket> l_packets;
    while (!isEmpty()) {
        l_packets.append(dequeue());
    }
    return l_packets;
}

bool AOChatQueue::isEmpty()
{
    return m_ms_queue.isEmpty();
}
