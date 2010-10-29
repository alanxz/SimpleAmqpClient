#include "Connection.h"

#include <amqp_framing.h>
namespace AmqpClient {

Connection::Connection() :
    m_nextChannel(0)
{
    m_connection = amqp_new_connection();

    int sock = amqp_open_socket(BROKER_HOST, BROKER_PORT);

    amqp_set_sockfd(m_connection, sock);

    amqp_login(m_connection, BROKER_VHOST, BROKER_CHANNEL_MAX,
               BROKER_FRAME_MAX, BROKER_HEARTBEAT, AMQP_SASL_METHOD_PLAIN,
               BROKER_USERNAME, BROKER_PASSWORD);
}

Connection::~Connection()
{
    amqp_connection_close(m_connection, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(m_connection);
}

Channel Connection::CreateChannel()
{
    m_nextChannel++;
    return Channel(m_connection, m_nextChannel);
}

} // namespace AmqpClient
