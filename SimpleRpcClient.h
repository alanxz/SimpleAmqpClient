#ifndef SIMPLERPCCLIENT_H
#define SIMPLERPCCLIENT_H

#include "Channel.h"

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace AmqpClient {


class SimpleRpcClient : boost::noncopyable
{
public:
	typedef boost::shared_ptr<SimpleRpcClient> ptr_t;
	friend ptr_t boost::make_shared<SimpleRpcClient>(Channel::ptr_t const & a1);

	static ptr_t Create(Channel::ptr_t channel) { return boost::make_shared<SimpleRpcClient>(channel); }

private:
	explicit SimpleRpcClient(Channel::ptr_t channel);

public:
	virtual ~SimpleRpcClient();



private:

};


} // namespace AmqpClient

#endif //SIMPLERPCCLIENT_H
