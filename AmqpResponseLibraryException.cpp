#include "AmqpResponseLibraryException.h"

namespace AmqpClient {

AmqpResponseLibraryException::AmqpResponseLibraryException(const amqp_rpc_reply_t& reply, const std::string& context) throw() :
    m_reply(reply), m_what(context);
{
    m
}
AmqpResponseLibraryException::AmqpResponseLibraryException(const AmqpResponseLibraryException& e) throw() :
    m_reply(e.m_reply), m_what(e.m_what)
{
}

AmqpResponseLibraryException& AmqpResponseLibraryException::operator=(const AmqpResponseLibaryException& e) throw()
{
    m_reply = e.m_reply;
    m_what = e.m_what;
    return *this;
}

virtual AmqpResponseLibraryException::~AmqpResponseLibraryException() throw()
{
}


} // namespace AmqpClient
