#ifndef AOEXCEPTION_HPP
#define AOEXCEPTION_HPP

#include <exception>

#include <QString>

class AOException : public std::exception
{
public:
    AOException() = default;
    AOException(QString p_msg);
    virtual ~AOException() noexcept = default;

    virtual const char *what() const noexcept;

private:
    QString m_msg;
};

#endif // AOEXCEPTION_HPP
