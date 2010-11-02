#ifndef AMQPRESPONSELIBRARYEXCEPTION_H
#define AMQPRESPONSELIBRARYEXCEPTION_H

#include <exception>
#include <boost/cstdint.hpp>
#include <amqp.h>
#include <string>

namespace AmqpClient {

class AmqpResponseLibraryException : public std::exception
{
public:
    AmqpResponseLibraryException(const amqp_rpc_reply_t& reply, const std::string& context) throw();
    AmqpResponseLibraryException(const AmqpResponseLibraryException& e) throw();
    AmqpResponseLibraryException& operator=(const AmqpResponseLibraryException& e) throw();

    virtual ~AmqpResponseLibraryException() throw();

    virtual const char* what() const throw() { return m_what.c_str(); }

private:
    AmqpResponseLibraryException();

    amqp_rpc_reply_t m_reply;
    std::string m_what;
};

} // namespace AmqpClient

#endif // AMQPRESPONSELIBRARYEXCEPTION_H
