/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1
 *
 * ``The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is SimpleAmqpClient for RabbitMQ.
 *
 * The Initial Developer of the Original Code is Alan Antonuk.
 * Original code is Copyright (C) Alan Antonuk.
 *
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the GNU General Public License Version 2 or later (the "GPL"), in
 * which case the provisions of the GPL are applicable instead of those
 * above. If you wish to allow use of your version of this file only
 * under the terms of the GPL, and not to allow others to use your
 * version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the
 * notice and other provisions required by the GPL. If you do not
 * delete the provisions above, a recipient may use your version of
 * this file under the terms of any one of the MPL or the GPL.
 *
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
