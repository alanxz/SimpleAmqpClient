#include "SimpleRpcClient.h"
#include "Message.h"

namespace AmqpClient {

SimpleRpcClient::SimpleRpcClient(Channel::ptr_t channel, const std::string&
		rpc_name) :
	m_channel(channel)
{
	// Declare the reply queue, by passing an empty string, the broker will
	// give us a name
	m_incoming_tag = m_channel->DeclareQueue("");
	m_channel->BindQueue(m_incoming_tag, "amq.direct", m_incoming_tag);
	m_channel->BasicConsume(m_incoming_tag, m_incoming_tag);
}

SimpleRpcClient::~SimpleRpcClient()
{
}

std::string SimpleRpcClient::Call(const std::string& message)
{
	Message::ptr_t outgoing_msg;
	outgoing_msg->Body(message);
	outgoing_msg->ReplyTo(m_incoming_tag);

	m_channel->BasicPublish("amq.direct", m_outgoing_tag, outgoing_msg);

	m_channel->BasicConsume(m_incoming_tag, m_incoming_tag);

	Message::ptr_t incoming_msg = m_channel->BasicConsumeMessage();
	return incoming_msg->Body();
}

} // namespace AmqpClient
