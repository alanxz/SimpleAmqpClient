#ifndef SIMPLERPCCLIENT_H
#define SIMPLERPCCLIENT_H

#include "Channel.h"

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <string>

namespace AmqpClient {


class SimpleRpcClient : boost::noncopyable
{
public:
	typedef boost::shared_ptr<SimpleRpcClient> ptr_t;
	friend ptr_t boost::make_shared<SimpleRpcClient>(Channel::ptr_t const & a1, std::string const & a2);

	static ptr_t Create(Channel::ptr_t channel, const std::string& rpc_name) 
	{ return boost::make_shared<SimpleRpcClient>(channel, rpc_name); }

private:
	explicit SimpleRpcClient(Channel::ptr_t channel, const std::string& rpc_name);

public:
	virtual ~SimpleRpcClient();

	std::string Call(const std::string& message);


private:
	Channel::ptr_t m_channel;
	const std::string m_outgoing_tag;
	const std::string m_incoming_tag;
};


} // namespace AmqpClient

#endif //SIMPLERPCCLIENT_H
