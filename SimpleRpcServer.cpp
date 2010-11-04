#include "SimpleRpcServer.h"


namespace AmqpClient {

SimpleRpcServer::SimpleRpcServer(Channel::ptr_t channel, const std::string& rpc_name) :
	m_channel(channel), m_incoming_tag(rpc_name)

{
	m_channel->DeclareQueue(m_incoming_tag);
	m_channel->BindQueue(m_incoming_tag, "amq.direct", m_incoming_tag);
	m_channel->BasicConsume(m_incoming_tag, m_incoming_tag);
}

SimpleRpcServer::~SimpleRpcServer()
{
}

Message::ptr_t SimpleRpcServer::GetNextIncomingMessage()
{
	return m_channel->BasicConsumeMessage();
}

void SimpleRpcServer::RespondToMessage(Message::ptr_t request, Message::ptr_t response)
{
	if (request->CorrelationIdIsSet() && !response->CorrelationIdIsSet())
	{
		response->CorrelationId(request->CorrelationId());
	}

	m_channel->BasicPublish("amq.direct", request->ReplyTo(), response);
}

void SimpleRpcServer::RespondToMessage(Message::ptr_t request, const std::string response)
{
	Message::ptr_t response_message = Message::Create();
	response_message->Body(response);

	RespondToMessage(request, response_message);
}

} // namespace AmqpClient
