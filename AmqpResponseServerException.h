#ifndef AMQPRESPONSESERVEREXCEPTION_H
#define AMQPRESPONSESERVEREXCEPTION_H

#include <boost/cstdint.hpp>
#include <amqp.h>
#include <string>
#include <exception>

namespace AmqpClient {

class AmqpResponseServerException : public std::exception
{
public:
	AmqpResponseServerException(const amqp_rpc_reply_t& reply, const std::string& context) throw();
	AmqpResponseServerException(const AmqpResponseServerException& e) throw();
	AmqpResponseServerException& operator=(const AmqpResponseServerException& e) throw();

	virtual ~AmqpResponseServerException() throw();

	virtual const char* what() const throw() { return m_what.c_str(); }

private:
	AmqpResponseServerException();

    amqp_rpc_reply_t m_reply;
    std::string m_what;
};

} // namespace AmqpClient

#endif // AMQPRESPONSESERVEREXCEPTION_H
