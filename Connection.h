#ifndef CONNECTION_H
#define CONNECTION_H

#include "Channel.h"

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <amqp.h>
#include <boost/utility.hpp>
#include <string>
#include <vector>

#define BROKER_HEARTBEAT 0

namespace AmqpClient {

class Connection : boost::noncopyable
{
public:
	typedef boost::shared_ptr<Connection> ptr_t;
	friend ptr_t boost::make_shared<Connection>(std::string const & a1, int const & a2,
			std::string const & a3, std::string const & a4,
			std::string const & a5, int const & a6, int const & a7);

	static ptr_t Create(const std::string& host = "127.0.0.1",
						int port = 5672,
						const std::string& username = "guest",
						const std::string& password = "guest",
						const std::string& vhost = "/",
						int channel_max = 0,
						int frame_max = 131072)
	{
		return boost::make_shared<Connection>(host, port, username, password, vhost, channel_max, frame_max);
	}
private:
    explicit Connection(const std::string& host,
			   int port,
			   const std::string& username,
			   const std::string& password,
			   const std::string& vhost,
			   int channel_max,
			   int frame_max);
public:
    virtual ~Connection();

    amqp_connection_state_t GetConnectionObject() const { return m_connection; }

    Channel::ptr_t CreateChannel();

protected:
    amqp_connection_state_t m_connection;
    amqp_channel_t m_nextChannel;
    std::vector<Channel::ptr_t> m_channels;
};

} // namespace AmqpClient

#endif // CONNECTION_H
