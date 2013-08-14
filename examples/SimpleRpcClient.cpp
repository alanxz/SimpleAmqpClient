/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MIT
 *
 * Copyright (c) 2010-2013 Alan Antonuk
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * ***** END LICENSE BLOCK *****
 */

#include "SimpleRpcClient.h"
#include "BasicMessage.h"

namespace AmqpClient {

SimpleRpcClient::SimpleRpcClient(Channel::ptr_t channel, const std::string& rpc_name) :
	m_channel(channel), m_outgoing_tag(rpc_name),
	// Declare the reply queue, by passing an empty string, the broker will
	// give us a name
	m_incoming_tag(m_channel->DeclareQueue(""))
{
	m_channel->BindQueue(m_incoming_tag, "amq.direct", m_incoming_tag);
	m_channel->BasicConsume(m_incoming_tag, m_incoming_tag);
}

SimpleRpcClient::~SimpleRpcClient()
{
}

std::string SimpleRpcClient::Call(const std::string& message)
{
	BasicMessage::ptr_t outgoing_msg = BasicMessage::Create();
	outgoing_msg->Body(message);

	BasicMessage::ptr_t reply = Call(outgoing_msg);
	return reply->Body();
}

BasicMessage::ptr_t SimpleRpcClient::Call(BasicMessage::ptr_t message)
{
	message->ReplyTo(m_incoming_tag);
	m_channel->BasicPublish("amq.direct", m_outgoing_tag, message);

	BasicMessage::ptr_t incoming_msg = m_channel->BasicConsumeMessage(m_incoming_tag)->Message();

	return incoming_msg;
}

} // namespace AmqpClient
