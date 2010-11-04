
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

#include "SimpleRpcClient.h"
#include "Message.h"

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
	Message::ptr_t outgoing_msg;
	outgoing_msg->Body(message);
	outgoing_msg->ReplyTo(m_incoming_tag);

	m_channel->BasicPublish("amq.direct", m_outgoing_tag, outgoing_msg);

	m_channel->BasicConsume(m_incoming_tag, m_incoming_tag);

	Message::ptr_t incoming_msg = m_channel->BasicConsumeMessage();
	return incoming_msg->Body();
}

} // namespace AmqpClient
