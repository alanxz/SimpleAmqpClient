#ifndef SIMPLERPCSERVER_H
#define SIMPLERPCSERVER_H

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace AmqpClient {


class SimpleRpcServer : boost::noncopyable
{
public:
	typedef boost::shared_ptr<SimpleRpcServer> ptr_t;

	friend ptr_t boost::make_shared<SimpleRpcServer>();

	static ptr_t Create() { return boost::make_shared<SimpleRpcServer>(); }

private:
	explicit SimpleRpcServer();

public:
	virtual ~SimpleRpcServer();


private:


};

}
#endif // SIMPLERPCSERVER_H
