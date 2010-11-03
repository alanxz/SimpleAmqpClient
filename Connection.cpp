#include "Connection.h"
#include "Util.h"

#include <amqp_framing.h>
namespace AmqpClient {

Connection::Connection(const std::string& host,
			   int port,
			   const std::string& username,
			   const std::string& password,
			   const std::string& vhost,
			   int channel_max,
			   int frame_max) :
    m_nextChannel(0)
{
    m_connection = amqp_new_connection();

    int sock = amqp_open_socket(host.c_str(), port);
    Util::CheckForError(sock, "Connection::Connection amqp_open_socket");

    amqp_set_sockfd(m_connection, sock);

    Util::CheckRpcReply(amqp_login(m_connection, vhost.c_str(), channel_max,
                                   frame_max, BROKER_HEARTBEAT, AMQP_SASL_METHOD_PLAIN,
                                   username.c_str(), password.c_str()), std::string("Amqp Login"));
}

Connection::~Connection()
{
    amqp_connection_close(m_connection, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(m_connection);
}

Channel::ptr_t Connection::CreateChannel()
{
    m_nextChannel++;
    m_channels.push_back(Channel::Create(m_connection, m_nextChannel));
    return m_channels.back();
}

} // namespace AmqpClient
