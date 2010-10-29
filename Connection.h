#ifndef CONNECTION_H
#define CONNECTION_H

#include "Channel.h"

#include <boost/cstdint.hpp>
#include <amqp.h>
#include <boost/utility.hpp>
#include <string>
#include <vector>

#define BROKER_HOST "141.214.124.76"
#define BROKER_PORT 5672
#define BROKER_VHOST "/"
#define BROKER_CHANNEL_MAX 0
#define BROKER_FRAME_MAX 131072
#define BROKER_HEARTBEAT 0
#define BROKER_USERNAME "guest"
#define BROKER_PASSWORD "guest"
#define BROKER_CHANNEL 1

namespace AmqpClient {

class Connection : boost::noncopyable
{
public:
    Connection();
    virtual ~Connection();

    amqp_connection_state_t GetConnectionObject() const { return m_connection; }

    Channel::Ptr CreateChannel();

protected:
    amqp_connection_state_t m_connection;
    amqp_channel_t m_nextChannel;
    std::vector<Channel::Ptr> m_channels;
};

} // namespace AmqpClient

#endif // CONNECTION_H
