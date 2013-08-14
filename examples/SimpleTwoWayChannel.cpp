/* vim:set ft=cpp ts=4 sw=4 sts=4 et cindent: */
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

#include "SimpleAmqpClient/SimpleTwoWayChannel.h"
#include "SimpleAmqpClient/BasicMessage.h"


namespace AmqpClient {

const std::string SimpleTwoWayChannel::ROUTING_KEY = "";

SimpleTwoWayChannel::SimpleTwoWayChannel(Channel::ptr_t channel, const std::string& incoming_name, 
			const std::string& outgoing_name) :
	m_channel(channel), m_incoming_tag(incoming_name), m_outgoing_tag(outgoing_name)
{
	// Receiver:
	m_channel->DeclareExchange(m_incoming_tag, "fanout");
	// Have the broker create a queue name for us
	std::string queue_name = m_channel->DeclareQueue("");
	m_channel->BindQueue(queue_name, m_incoming_tag);
	m_channel->BasicConsume(queue_name, m_incoming_tag);

	// Sender
}

SimpleTwoWayChannel::~SimpleTwoWayChannel() 
{
}

void SimpleTwoWayChannel::Send(const std::string& message)
{
	BasicMessage::ptr_t full_message = BasicMessage::Create();
	full_message->Body(message);
	Send(full_message);
}

void SimpleTwoWayChannel::Send(BasicMessage::ptr_t message)
{
	m_channel->BasicPublish(m_outgoing_tag, ROUTING_KEY, message);
}

std::string SimpleTwoWayChannel::Receive()
{
	BasicMessage::ptr_t message = ReceiveMessage();
	return message->Body();
}

BasicMessage::ptr_t SimpleTwoWayChannel::ReceiveMessage()
{
	return m_channel->BasicConsumeMessage();
}

std::string SimpleTwoWayChannel::SendAndReceive(const std::string& message)
{
	Send(message);
	return Receive();
}

BasicMessage::ptr_t SimpleTwoWayChannel::SendAndReceive(BasicMessage::ptr_t message)
{
	Send(message);
	return ReceiveMessage();
}

} // namespace AmqpClient
