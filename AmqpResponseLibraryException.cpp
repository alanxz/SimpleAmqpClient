#include "AmqpResponseLibraryException.h"

namespace AmqpClient {

AmqpResponseLibraryException::AmqpResponseLibraryException(const amqp_rpc_reply_t& reply, const std::string& context) throw() :
    m_reply(reply), m_what(context)
{
	m_what += ": ";

	char* error_string = amqp_error_string(reply.library_error);

	m_what += error_string;

	free(error_string);
}
AmqpResponseLibraryException::AmqpResponseLibraryException(const AmqpResponseLibraryException& e) throw() :
    m_reply(e.m_reply), m_what(e.m_what)
{
}

AmqpResponseLibraryException& AmqpResponseLibraryException::operator=(const AmqpResponseLibraryException& e) throw()
{
	if (this == &e)
	{
		return *this;
	}

    m_reply = e.m_reply;
    m_what = e.m_what;
    return *this;
}

AmqpResponseLibraryException::~AmqpResponseLibraryException() throw()
{
}


} // namespace AmqpClient
