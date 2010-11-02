#include "AmqpResponseServerException.h"

#include <boost/cstdint.hpp>
#include <amqp.h>
#include <amqp_framing.h>
#include <sstream>

namespace AmqpClient {

AmqpResponseServerException::AmqpResponseServerException(const amqp_rpc_reply_t& reply, const std::string& context) throw() :
	m_reply(reply)
{
	std::ostringstream oss;
	oss << context;

	switch (reply.reply.id)
	{
		case AMQP_CONNECTION_CLOSE_METHOD:
			{
				amqp_connection_close_t* msg = reinterpret_cast<amqp_connection_close_t*>(reply.reply.decoded);
				oss << ": Server connection error: " << msg->reply_code << " status: " 
					<< std::string((char*)msg->reply_text.bytes, msg->reply_text.len);
			}
			break;

		case AMQP_CHANNEL_CLOSE_METHOD:
			{
				amqp_channel_close_t* msg = reinterpret_cast<amqp_channel_close_t*>(reply.reply.decoded);
				oss << ": Server channel error: " << msg->reply_code << " status: " 
					<< std::string((char*)msg->reply_text.bytes, msg->reply_text.len);

			}
			break;

		default:
			oss << ": Unknown server error, method: " << reply.reply.id;

	}
	m_what = oss.str();
}

AmqpResponseServerException::AmqpResponseServerException(const AmqpResponseServerException& e) throw() :
	m_reply(e.m_reply), m_what(e.m_what)
{
}
AmqpResponseServerException& AmqpResponseServerException::operator=(const AmqpResponseServerException& e) throw()
{
	if (this == &e)
	{
		return *this;
	}

	m_reply = e.m_reply;
	m_what = e.m_what;
	return *this;
}

AmqpResponseServerException::~AmqpResponseServerException() throw()
{
}

} // namespace AmqpClient
