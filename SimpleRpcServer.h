#ifndef SIMPLERPCSERVER_H
#define SIMPLERPCSERVER_H

#include "Channel.h"
#include "Message.h"

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <string>

namespace AmqpClient {


class SimpleRpcServer : boost::noncopyable
{
public:
	typedef boost::shared_ptr<SimpleRpcServer> ptr_t;

	friend ptr_t boost::make_shared<SimpleRpcServer>(Channel::ptr_t const & a1, std::string const & a2);

	static ptr_t Create(Channel::ptr_t channel, const std::string& rpc_name) 
		{ return boost::make_shared<SimpleRpcServer>(channel, rpc_name); }

	static std::string CreateUniqueRpcName(const std::string prefix = "");

private:
	explicit SimpleRpcServer(Channel::ptr_t channel, const std::string& rpc_name);

public:
	virtual ~SimpleRpcServer();

	std::string GetRpcName() const { return m_incoming_tag; }

	Message::ptr_t GetNextIncomingMessage();

	void RespondToMessage(Message::ptr_t request, 
						  Message::ptr_t response);

	void RespondToMessage(Message::ptr_t request,
						  const std::string response);

private:
	Channel::ptr_t m_channel;
	const std::string& m_incoming_tag;




};

}
#endif // SIMPLERPCSERVER_H
