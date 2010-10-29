#include "Connection.h"
#include "Util.h"

#include <amqp_framing.h>
namespace AmqpClient {

Connection::Connection() :
    m_nextChannel(0)
{
    m_connection = amqp_new_connection();

    int sock = amqp_open_socket(BROKER_HOST, BROKER_PORT);
    Util::CheckForError(sock, "Connection::Connection amqp_open_socket");

    amqp_set_sockfd(m_connection, sock);

    Util::CheckRpcReply(amqp_login(m_connection, BROKER_VHOST, BROKER_CHANNEL_MAX,
                                   BROKER_FRAME_MAX, BROKER_HEARTBEAT, AMQP_SASL_METHOD_PLAIN,
                                   BROKER_USERNAME, BROKER_PASSWORD), std::string("Amqp Login"));
}

Connection::~Connection()
{
    amqp_connection_close(m_connection, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(m_connection);
}

Channel::Ptr Connection::CreateChannel()
{
    m_nextChannel++;
    m_channels.push_back(Channel::New(m_connection, m_nextChannel));
    return m_channels.back();
}

} // namespace AmqpClient
