
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

#include "Channel.h"

#include "Util.h"

#include <amqp_framing.h>

#include <stdexcept>
#include <cstring>

namespace AmqpClient {

const amqp_table_t Channel::EMPTY_TABLE = { 0, NULL };

Channel::Channel(const std::string& host,
                 int port,
                 const std::string& username,
                 const std::string& password,
                 const std::string& vhost,
                 int frame_max) :
    m_channel(DEFAULT_CHANNEL)
{
    m_connection = amqp_new_connection();

    int sock = amqp_open_socket(host.c_str(), port);
    Util::CheckForError(sock, "Channel::Channel amqp_open_socket");

    amqp_set_sockfd(m_connection, sock);

    Util::CheckRpcReply(amqp_login(m_connection, vhost.c_str(), 2,
                                   frame_max, BROKER_HEARTBEAT, AMQP_SASL_METHOD_PLAIN,
                                   username.c_str(), password.c_str()), "Amqp Login");

    amqp_channel_open(m_connection, m_channel);
    Util::CheckLastRpcReply(m_connection, "Channel::Channel creating default channel");
}

Channel::~Channel()
{
    amqp_channel_close(m_connection, m_channel, AMQP_REPLY_SUCCESS);
    amqp_connection_close(m_connection, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(m_connection);
}

void Channel::DeclareExchange(const std::string& exchange_name,
                              const std::string& exchange_type,
                              bool passive,
                              bool durable,
                              bool auto_delete)
{
    amqp_exchange_declare(m_connection, m_channel,
                          amqp_cstring_bytes(exchange_name.c_str()),
                          amqp_cstring_bytes(exchange_type.c_str()),
                          passive,
                          durable,
                          auto_delete,
                          EMPTY_TABLE);
	Util::CheckLastRpcReply(m_connection, "Declaring exchange");
}


std::string Channel::DeclareQueue(const std::string& queue_name,
          		                  bool passive,
								  bool durable,
								  bool exclusive,
								  bool auto_delete)
{

    amqp_queue_declare_ok_t* queue_declare = 
			amqp_queue_declare(m_connection, m_channel,
								amqp_cstring_bytes(queue_name.c_str()),
								passive,
								durable,
								exclusive,
								auto_delete,
								EMPTY_TABLE);

	Util::CheckLastRpcReply(m_connection, "Declaring queue");

	return std::string((char*)queue_declare->queue.bytes,
			queue_declare->queue.len);
}

void Channel::DeleteQueue(const std::string& queue_name,
                          bool if_unused,
                          bool if_empty)
{
    amqp_queue_delete(m_connection, m_channel,
                      amqp_cstring_bytes(queue_name.c_str()),
                      if_unused,
                      if_empty);

	Util::CheckLastRpcReply(m_connection, "Deleting Queue");
}

void Channel::BindQueue(const std::string& queue_name,
                        const std::string& exchange_name,
                        const std::string& routing_key)
{
    amqp_queue_bind(m_connection, m_channel,
                    amqp_cstring_bytes(queue_name.c_str()),
                    amqp_cstring_bytes(exchange_name.c_str()),
                    amqp_cstring_bytes(routing_key.c_str()),
                    EMPTY_TABLE);

	Util::CheckLastRpcReply(m_connection, "Binding queue");
}

void Channel::UnbindQueue(const std::string& queue_name,
                          const std::string& exchange_name,
                          const std::string& binding_key)
{
    amqp_queue_unbind(m_connection, m_channel,
                      amqp_cstring_bytes(queue_name.c_str()),
                      amqp_cstring_bytes(exchange_name.c_str()),
                      amqp_cstring_bytes(binding_key.c_str()),
                      EMPTY_TABLE);

	Util::CheckLastRpcReply(m_connection, "Unbinding queue");
}

void Channel::BasicAck(const BasicMessage::ptr_t message)
{
	BasicAck(message->DeliveryTag());
}

void Channel::BasicAck(uint64_t delivery_tag)
{
	Util::CheckForError(amqp_basic_ack(m_connection, m_channel,
			delivery_tag,
			false), "Ack");

}
void Channel::BasicPublish(const std::string& exchange_name,
                           const std::string& routing_key,
                           const BasicMessage::ptr_t message,
                           bool mandatory,
                           bool immediate)
{
    amqp_basic_publish(m_connection, m_channel,
                       amqp_cstring_bytes(exchange_name.c_str()),
                       amqp_cstring_bytes(routing_key.c_str()),
                       mandatory,
                       immediate,
                       message->getAmqpProperties(),
                       message->getAmqpBody());

	Util::CheckLastRpcReply(m_connection, "Publishing to queue");
}

void Channel::BasicConsume(const std::string& queue,
						   const std::string& consumer_tag,
						   bool no_local,
						   bool no_ack,
						   bool exclusive)
{
	amqp_basic_consume(m_connection, m_channel,
			amqp_cstring_bytes(queue.c_str()),
			amqp_cstring_bytes(consumer_tag.c_str()),
			no_local,
			no_ack,
			exclusive);

	Util::CheckLastRpcReply(m_connection, "Basic Consume");
}

void Channel::BasicCancel(const std::string& consumer_tag)
{
	amqp_method_number_t replies[2] = { AMQP_BASIC_CANCEL_OK_METHOD, 0 };
	amqp_basic_cancel_t req;
	req.consumer_tag = amqp_cstring_bytes(consumer_tag.c_str());
	req.nowait = 0;

	Util::CheckRpcReply(amqp_simple_rpc(m_connection, m_channel,
				AMQP_BASIC_CANCEL_METHOD,
				replies, &req), "Basic Cancel");
}


BasicMessage::ptr_t Channel::BasicConsumeMessage()
{
	while (true)
	{
		amqp_frame_t frame;
		amqp_maybe_release_buffers(m_connection);
		
		Util::CheckForError(amqp_simple_wait_frame(m_connection, &frame), "Consume Message: method frame");

		if (frame.frame_type != AMQP_FRAME_METHOD || frame.payload.method.id != AMQP_BASIC_DELIVER_METHOD)
			continue;

		amqp_basic_deliver_t* deliver_method = reinterpret_cast<amqp_basic_deliver_t*>(frame.payload.method.decoded);

		// Wait for frame #2, the header frame which contains body size
		Util::CheckForError(amqp_simple_wait_frame(m_connection, &frame), "Consume Message: header frame");

		if (frame.frame_type != AMQP_FRAME_HEADER)
			throw std::runtime_error("Channel::BasicConsumeMessage: receieved unexpected frame type (was expected AMQP_FRAME_HEADER)");

		amqp_basic_properties_t* properties = reinterpret_cast<amqp_basic_properties_t*>(frame.payload.properties.decoded);

		size_t body_size = frame.payload.properties.body_size;
		size_t received_size = 0;
		amqp_bytes_t body = amqp_bytes_malloc(body_size);

		// frame #3 and up:
		while (received_size < body_size)
		{
			Util::CheckForError(amqp_simple_wait_frame(m_connection, &frame), "Consume Message: body frame");

			if (frame.frame_type != AMQP_FRAME_BODY)
				throw std::runtime_error("Channel::BasicConsumeMessge: received unexpected frame type (was expecting AMQP_FRAME_BODY)");

			void* body_ptr = reinterpret_cast<char*>(body.bytes) + received_size;
			memcpy(body_ptr, frame.payload.body_fragment.bytes, frame.payload.body_fragment.len);
			received_size += frame.payload.body_fragment.len;
		}
		return BasicMessage::Create(body, properties, deliver_method->delivery_tag);
	}
}

} // namespace AmqpClient
