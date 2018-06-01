#include "aoexception.hpp"

AOException::AOException(QString p_msg)
    : m_msg(p_msg)
{}

const char *AOException::what() const noexcept
{
    return m_msg.toStdString().c_str();
}
