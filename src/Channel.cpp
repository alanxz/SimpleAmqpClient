
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

#include "SimpleAmqpClient/Channel.h"

#include "SimpleAmqpClient/AmqpResponseLibraryException.h"
#include "SimpleAmqpClient/AmqpResponseServerException.h"
#include "SimpleAmqpClient/ConsumerTagNotFoundException.h"
#include "SimpleAmqpClient/MessageReturnedException.h"
#include "SimpleAmqpClient/Util.h"
#include "config.h"

#include <amqp.h>
#include <amqp_framing.h>

#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>

#include <boost/cstdint.hpp>
#include <boost/limits.hpp>

// This will get us the posix version of strerror_r() on linux
#ifndef _XOPEN_SOURCE
# define _XOPEN_SOURCE 600
#endif
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
# include <sys/select.h>
#endif

#ifdef HAVE_WINSOCK2_H
# include <WinSock2.h>
#endif

#include <time.h>

// Win32 headers seem to define this annoyingly...
#ifdef max
# undef max
#endif

namespace AmqpClient {

namespace Detail
{

class ChannelImpl : boost::noncopyable
{
public:
  ChannelImpl();
  virtual ~ChannelImpl();

  amqp_connection_state_t m_connection;

  amqp_channel_t GetChannel();
  void ReturnChannel(amqp_channel_t channel);

  void CheckLastRpcReply(amqp_channel_t channel, const std::string& context);
  void CheckRpcReply(amqp_channel_t channel, const amqp_rpc_reply_t& reply, const std::string& context);
  void CheckForError(int ret, const std::string& context);

  void CheckFrameForClose(amqp_frame_t& frame, amqp_channel_t channel);
  void FinishCloseChannel(amqp_channel_t channel);
  void FinishCloseConnection();

  MessageReturnedException CreateMessageReturnedException(amqp_basic_return_t& return_method, amqp_channel_t channel);
  AmqpClient::BasicMessage::ptr_t ReadContent(amqp_channel_t channel);

  void AddConsumer(const std::string& consumer_tag, amqp_channel_t channel);
  amqp_channel_t RemoveConsumer(const std::string& consumer_tag);
  amqp_channel_t GetConsumerChannel(const std::string& consumer_tag);

 static const amqp_table_t EMPTY_TABLE;

private:
  amqp_channel_t CreateNewChannel();
  amqp_channel_t GetNextChannelId();

  std::queue<amqp_channel_t> m_free_channels;
  std::set<amqp_channel_t> m_open_channels;

  uint16_t m_next_channel_id;
  std::map<std::string, amqp_channel_t> m_consumer_channel_map;
};

ChannelImpl::ChannelImpl() :
  m_next_channel_id(1)
{
  // Channel 0 is always open
  m_open_channels.insert(0);
}

ChannelImpl::~ChannelImpl()
{
}

amqp_channel_t ChannelImpl::GetNextChannelId()
{
  int max_channels = amqp_get_channel_max(m_connection);
  int channel_count = m_open_channels.size();
  if (0 == max_channels)
  {
    if (std::numeric_limits<uint16_t>::max() <= channel_count)
    {
      throw std::runtime_error("Too many channels open");
    }
  }
  else if (max_channels <= channel_count)
  {
    throw std::runtime_error("Too many channels open");
  }

  while (m_open_channels.end() != m_open_channels.find(++m_next_channel_id)) { /* Empty */ }

  m_open_channels.insert(m_next_channel_id);
  return m_next_channel_id;
}

amqp_channel_t ChannelImpl::CreateNewChannel()
{
  amqp_channel_t new_channel = GetNextChannelId();
  amqp_channel_open(m_connection, new_channel);
  CheckLastRpcReply(new_channel, "ChannelImpl::CreateNewChannel channel.open");

  amqp_confirm_select(m_connection, new_channel);
  CheckLastRpcReply(new_channel, "ChannelImpl::CreateNewChannel confirm.select");

  return new_channel;
}

amqp_channel_t ChannelImpl::GetChannel()
{
  if (m_free_channels.empty())
  {
    return CreateNewChannel();
  }
  else
  {
    amqp_channel_t ret = m_free_channels.front();
    m_free_channels.pop();
    return ret;
  }
}

void ChannelImpl::ReturnChannel(amqp_channel_t channel)
{
  m_free_channels.push(channel);
  amqp_maybe_release_buffers(m_connection);
}

void ChannelImpl::CheckLastRpcReply(amqp_channel_t channel, const std::string& context)
{
  CheckRpcReply(channel, amqp_get_rpc_reply(m_connection), context);
}

void ChannelImpl::CheckRpcReply(amqp_channel_t channel, const amqp_rpc_reply_t& reply, const std::string& context)
{
  switch (reply.reply_type)
  {
  case AMQP_RESPONSE_NORMAL:
    return;
    break;

  case AMQP_RESPONSE_NONE:
    throw std::logic_error("Got a amqp_rpc_reply_t with no reply_type!");
    break;

  case AMQP_RESPONSE_LIBRARY_EXCEPTION:
    // If we're getting this likely is the socket is already closed
    throw AmqpResponseLibraryException(reply, context);

    break;

  case AMQP_RESPONSE_SERVER_EXCEPTION:
    if (reply.reply.id == AMQP_CHANNEL_CLOSE_METHOD) 
    {
      FinishCloseChannel(channel);
    }
    else if (reply.reply.id == AMQP_CONNECTION_CLOSE_METHOD)
    {
      FinishCloseConnection();
    }

    throw AmqpResponseServerException(reply, context);
    break;
  default:
    throw std::runtime_error("amqp_rpc_reply_t that didn't match!");
  }
}

void ChannelImpl::FinishCloseChannel(amqp_channel_t channel)
{
  amqp_channel_close_ok_t close_ok;
  m_open_channels.erase(channel);
  CheckForError(amqp_send_method(m_connection, channel, AMQP_CHANNEL_CLOSE_OK_METHOD, &close_ok), "ChannelImpl::FinishCloseChannel channel.close");
}

void ChannelImpl::FinishCloseConnection()
{
  amqp_connection_close_ok_t close_ok;
  amqp_send_method(m_connection, 0, AMQP_CONNECTION_CLOSE_OK_METHOD, &close_ok);
}

void ChannelImpl::CheckForError(int ret, const std::string& context)
{
  if (ret < 0)
  {
    char* errstr = amqp_error_string(-ret);
    std::ostringstream oss;
    oss << context << ": " << errstr;
    free(errstr);
    throw std::runtime_error(oss.str().c_str());
  }
}

MessageReturnedException ChannelImpl::CreateMessageReturnedException(amqp_basic_return_t& return_method, amqp_channel_t channel)
{
  const int reply_code = return_method.reply_code;
  const std::string reply_text((char*)return_method.reply_text.bytes, return_method.reply_text.len);
  const std::string exchange((char*)return_method.exchange.bytes, return_method.exchange.len);
  const std::string routing_key((char*)return_method.routing_key.bytes, return_method.routing_key.len);
  BasicMessage::ptr_t content = ReadContent(channel);
  return MessageReturnedException(content, reply_code, reply_text, exchange, routing_key);
}

BasicMessage::ptr_t ChannelImpl::ReadContent(amqp_channel_t channel)
{
  amqp_frame_t frame;
  // Wait for frame #2, the header frame which contains body size
  CheckForError(amqp_simple_wait_frame_on_channel(m_connection, channel, &frame), "Consume Message: header frame");
  CheckFrameForClose(frame, channel);

  if (frame.frame_type != AMQP_FRAME_HEADER)
    throw std::runtime_error("Channel::BasicConsumeMessage: receieved unexpected frame type (was expected AMQP_FRAME_HEADER)");

  // The memory for this is allocated in a pool associated with the connection
  // Its freed in amqp_maybe_release_buffers above
  // The BasicMessage constructor does a deep copy of the properties structure
  amqp_basic_properties_t* properties = reinterpret_cast<amqp_basic_properties_t*>(frame.payload.properties.decoded);

  size_t body_size = frame.payload.properties.body_size;
  size_t received_size = 0;
  amqp_bytes_t body = amqp_bytes_malloc(body_size);

  // frame #3 and up:
  while (received_size < body_size)
  {
    CheckForError(amqp_simple_wait_frame(m_connection, &frame), "Consume Message: body frame");
    CheckFrameForClose(frame, channel);

    if (frame.frame_type != AMQP_FRAME_BODY)
      throw std::runtime_error("Channel::BasicConsumeMessge: received unexpected frame type (was expecting AMQP_FRAME_BODY)");

    void* body_ptr = reinterpret_cast<char*>(body.bytes) + received_size;
    memcpy(body_ptr, frame.payload.body_fragment.bytes, frame.payload.body_fragment.len);
    received_size += frame.payload.body_fragment.len;
  }
  return BasicMessage::Create(body, properties);
}

void ChannelImpl::CheckFrameForClose(amqp_frame_t& frame, amqp_channel_t channel)
{
  if (frame.frame_type == AMQP_FRAME_METHOD)
  {
    switch (frame.payload.method.id)
    {
    case AMQP_CHANNEL_CLOSE_METHOD:
      FinishCloseChannel(channel);
      throw AmqpResponseServerException(*reinterpret_cast<amqp_channel_close_t*>(frame.payload.method.decoded), "Consuming message");
      break;

    case AMQP_CONNECTION_CLOSE_METHOD:
      FinishCloseConnection();
      throw AmqpResponseServerException(*reinterpret_cast<amqp_connection_close_t*>(frame.payload.method.decoded), "Consuming message");
      break;
    }
  }
}

void ChannelImpl::AddConsumer(const std::string& consumer_tag, amqp_channel_t channel)
{
  m_consumer_channel_map.insert(std::make_pair(consumer_tag, channel));
}

amqp_channel_t ChannelImpl::RemoveConsumer(const std::string& consumer_tag)
{
  std::map<std::string, amqp_channel_t>::iterator it = m_consumer_channel_map.find(consumer_tag);
  if (it == m_consumer_channel_map.end())
  {
    throw ConsumerTagNotFoundException();
  }

  amqp_channel_t result = it->second;

  m_consumer_channel_map.erase(it);

  return result;
}

amqp_channel_t ChannelImpl::GetConsumerChannel(const std::string& consumer_tag)
{
  std::map<std::string, amqp_channel_t>::const_iterator it = m_consumer_channel_map.find(consumer_tag);
  if (it == m_consumer_channel_map.end())
  {
    throw ConsumerTagNotFoundException();
  }
  return it->second;
}

const amqp_table_t ChannelImpl::EMPTY_TABLE = { 0, NULL };

}

const std::string Channel::EXCHANGE_TYPE_DIRECT("amq.direct");
const std::string Channel::EXCHANGE_TYPE_FANOUT("fanout");
const std::string Channel::EXCHANGE_TYPE_TOPIC("topic");

Channel::Channel(const std::string& host,
                 int port,
                 const std::string& username,
                 const std::string& password,
                 const std::string& vhost,
                 int frame_max) :
m_impl(new Detail::ChannelImpl)
{
    m_impl->m_connection = amqp_new_connection();

    int sock = amqp_open_socket(host.c_str(), port);
    m_impl->CheckForError(sock, "Channel::Channel amqp_open_socket");

    amqp_set_sockfd(m_impl->m_connection, sock);

    m_impl->CheckRpcReply(0, amqp_login(m_impl->m_connection, vhost.c_str(), 0,
                                   frame_max, BROKER_HEARTBEAT, AMQP_SASL_METHOD_PLAIN,
                                   username.c_str(), password.c_str()), "Channel::Channel amqp_login");
}

Channel::~Channel()
{
    amqp_connection_close(m_impl->m_connection, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(m_impl->m_connection);
}

void Channel::DeclareExchange(const std::string& exchange_name,
                              const std::string& exchange_type,
                              bool passive,
                              bool durable,
                              bool auto_delete)
{
  amqp_channel_t channel = m_impl->GetChannel();

  amqp_exchange_declare(m_impl->m_connection, channel,
    amqp_cstring_bytes(exchange_name.c_str()),
    amqp_cstring_bytes(exchange_type.c_str()),
    passive,
    durable,
    Detail::ChannelImpl::EMPTY_TABLE);

	m_impl->CheckLastRpcReply(channel, "Channel::DeclareExchange exchange.declare");
  m_impl->ReturnChannel(channel);
}

void Channel::DeleteExchange(const std::string& exchange_name,
                             bool if_unused)
{
  amqp_channel_t channel = m_impl->GetChannel();

  amqp_exchange_delete(m_impl->m_connection, channel, 
    amqp_cstring_bytes(exchange_name.c_str()),
    if_unused);

  m_impl->CheckLastRpcReply(channel, "Channel::DeleteExchange exchange.delete");
  m_impl->ReturnChannel(channel);
}

void Channel::BindExchange(const std::string& destination,
                           const std::string& source,
                           const std::string& routing_key)
{
  amqp_channel_t channel = m_impl->GetChannel();

  amqp_exchange_bind(m_impl->m_connection, channel,
                    amqp_cstring_bytes(destination.c_str()),  
                    amqp_cstring_bytes(source.c_str()),
                    amqp_cstring_bytes(routing_key.c_str()),
                    Detail::ChannelImpl::EMPTY_TABLE);
  m_impl->CheckLastRpcReply(channel, "Channel::BindExchange exchange.bind");
  m_impl->ReturnChannel(channel);
}

void Channel::UnbindExchange(const std::string& destination,
                             const std::string& source,
                             const std::string& routing_key)
{
  amqp_channel_t channel = m_impl->GetChannel();

  amqp_exchange_bind(m_impl->m_connection, channel,
                    amqp_cstring_bytes(destination.c_str()),  
                    amqp_cstring_bytes(source.c_str()),
                    amqp_cstring_bytes(routing_key.c_str()),
                    Detail::ChannelImpl::EMPTY_TABLE);
  m_impl->CheckLastRpcReply(channel, "Channel::BindExchange exchange.bind");
  m_impl->ReturnChannel(channel);
}

std::string Channel::DeclareQueue(const std::string& queue_name,
          		                  bool passive,
								  bool durable,
								  bool exclusive,
								  bool auto_delete)
{
  amqp_channel_t channel = m_impl->GetChannel();

  amqp_queue_declare_ok_t* queue_declare = 
    amqp_queue_declare(m_impl->m_connection, channel,
    amqp_cstring_bytes(queue_name.c_str()),
    passive,
    durable,
    exclusive,
    auto_delete,
    Detail::ChannelImpl::EMPTY_TABLE);

  m_impl->CheckLastRpcReply(channel, "Channel::DeclareQueue queue.declare");
  m_impl->ReturnChannel(channel);

  return std::string((char*)queue_declare->queue.bytes,
    queue_declare->queue.len);
}

void Channel::DeleteQueue(const std::string& queue_name,
                          bool if_unused,
                          bool if_empty)
{
  amqp_channel_t channel = m_impl->GetChannel();
  amqp_queue_delete(m_impl->m_connection, channel,
    amqp_cstring_bytes(queue_name.c_str()),
    if_unused,
    if_empty);

	m_impl->CheckLastRpcReply(channel, "Channel::DeleteQueue queue.delete");
  m_impl->ReturnChannel(channel);
}

void Channel::BindQueue(const std::string& queue_name,
                        const std::string& exchange_name,
                        const std::string& routing_key)
{
  amqp_channel_t channel = m_impl->GetChannel();
  amqp_queue_bind(m_impl->m_connection, channel,
    amqp_cstring_bytes(queue_name.c_str()),
    amqp_cstring_bytes(exchange_name.c_str()),
    amqp_cstring_bytes(routing_key.c_str()),
    Detail::ChannelImpl::EMPTY_TABLE);

	m_impl->CheckLastRpcReply(channel, "Channel::BindQueue queue.bind");
  m_impl->ReturnChannel(channel);
}

void Channel::UnbindQueue(const std::string& queue_name,
                          const std::string& exchange_name,
                          const std::string& binding_key)
{
  amqp_channel_t channel = m_impl->GetChannel();
  amqp_queue_unbind(m_impl->m_connection, channel,
    amqp_cstring_bytes(queue_name.c_str()),
    amqp_cstring_bytes(exchange_name.c_str()),
    amqp_cstring_bytes(binding_key.c_str()),
    Detail::ChannelImpl::EMPTY_TABLE);

	m_impl->CheckLastRpcReply(channel, "Channel::UnbindQueue queue.unbind");
  m_impl->ReturnChannel(channel);
}

void Channel::BasicAck(const BasicMessage::ptr_t message)
{
	BasicAck(message->DeliveryTag());
}

void Channel::BasicAck(uint64_t delivery_tag)
{
  amqp_channel_t channel = m_impl->GetChannel();
	m_impl->CheckForError(amqp_basic_ack(m_impl->m_connection, channel,
			delivery_tag,
			false), "Channel::BasicAck basic.ack");

  m_impl->ReturnChannel(channel);
}

void Channel::BasicPublish(const std::string& exchange_name,
                           const std::string& routing_key,
                           const BasicMessage::ptr_t message,
                           bool mandatory,
                           bool immediate)
{
  amqp_channel_t channel = m_impl->GetChannel();

  m_impl->CheckForError(amqp_basic_publish(m_impl->m_connection, channel,
                       amqp_cstring_bytes(exchange_name.c_str()),
                       amqp_cstring_bytes(routing_key.c_str()),
                       mandatory,
                       immediate,
                       message->getAmqpProperties(),
                       message->getAmqpBody()), "Publishing to queue");
  // If we've done things correctly we can get one of 4 things back from the broker
  // - basic.ack - our channel is in confirm mode, messsage was 'dealt with' by the broker
  // - basic.return then basic.ack - the message wasn't delievered, but was dealt with
  // - channel.close - probably tried to publish to a non-existant exchange, in any case error!
  // - connection.clsoe - something really bad happened
  amqp_method_number_t methods[] = { AMQP_BASIC_ACK_METHOD, AMQP_BASIC_RETURN_METHOD, 0 };
  amqp_rpc_reply_t reply = amqp_simple_wait_methods(m_impl->m_connection, channel, methods);

  m_impl->CheckRpcReply(channel, reply, "Channel::BasicPublish waiting for basic.ack or basic.return");

  if (reply.reply.id == AMQP_BASIC_RETURN_METHOD)
  {
    MessageReturnedException message_returned = 
      m_impl->CreateMessageReturnedException(*(reinterpret_cast<amqp_basic_return_t*>(reply.reply.decoded)), channel);

    // Now listen for just an basic.ack
    methods[1] = 0;
    m_impl->CheckRpcReply(channel, 
      amqp_simple_wait_methods(m_impl->m_connection, channel, methods), 
      "Channel::BasicPublish waiting for basic.ack");

    m_impl->ReturnChannel(channel);
    throw message_returned;
  }

  m_impl->ReturnChannel(channel);
}

bool Channel::BasicGet(BasicMessage::ptr_t& message, const std::string& queue, bool no_ack)
{
  amqp_channel_t channel = m_impl->GetChannel();

  amqp_rpc_reply_t reply = amqp_basic_get(m_impl->m_connection, channel, amqp_cstring_bytes(queue.c_str()), no_ack);
  m_impl->CheckRpcReply(channel, reply, "Channel::BasicGet basic.get");

  if (AMQP_BASIC_GET_EMPTY_METHOD == reply.reply.id)
  {
    m_impl->ReturnChannel(channel);
    return false;
  }
  message = m_impl->ReadContent(channel);
  m_impl->ReturnChannel(channel);
  return true;
}

std::string Channel::BasicConsume(const std::string& queue,
						   const std::string& consumer_tag,
						   bool no_local,
						   bool no_ack,
						   bool exclusive)
{
  amqp_channel_t channel = m_impl->GetChannel();

	amqp_basic_consume_ok_t* consume_ok = amqp_basic_consume(m_impl->m_connection, channel,
    amqp_cstring_bytes(queue.c_str()),
    amqp_cstring_bytes(consumer_tag.c_str()),
    no_local,
    no_ack,
    exclusive,
    Detail::ChannelImpl::EMPTY_TABLE);

	m_impl->CheckLastRpcReply(channel, "Channel::BasicConsume basic.consume");
  std::string tag((char*)consume_ok->consumer_tag.bytes, consume_ok->consumer_tag.len);

  m_impl->AddConsumer(tag, channel);

  return tag;
}

void Channel::BasicCancel(const std::string& consumer_tag)
{
  amqp_channel_t channel = m_impl->GetConsumerChannel(consumer_tag);

  amqp_basic_cancel(m_impl->m_connection, channel, amqp_cstring_bytes(consumer_tag.c_str()));

  m_impl->RemoveConsumer(consumer_tag);
  m_impl->CheckLastRpcReply(channel, "Channel::BasicCancel basic.cancel");

  // Lets go hunting to make sure we don't have any queued frames lying around
  // Otherwise these frames will lie around forever
  while (amqp_frames_enqueued_for_channel(m_impl->m_connection, channel))
  {
    amqp_frame_t f;
    amqp_simple_wait_frame_on_channel(m_impl->m_connection, channel, &f);
  }
  m_impl->ReturnChannel(channel);
}


BasicMessage::ptr_t Channel::BasicConsumeMessage(const std::string& consumer_tag)
{
	BasicMessage::ptr_t returnval;
	BasicConsumeMessage(consumer_tag, returnval, 0);
	return returnval;
}

bool Channel::BasicConsumeMessage(const std::string& consumer_tag, BasicMessage::ptr_t& message, int timeout)
{
  Envelope::ptr_t envelope;
  bool ret = BasicConsumeMessage(consumer_tag, envelope, timeout);
  if (ret) 
  {
    message = envelope->Message();
  }
  return ret;
}

bool Channel::BasicConsumeMessage(const std::string& consumer_tag, Envelope::ptr_t& message, int timeout)
{
  amqp_channel_t channel = m_impl->GetConsumerChannel(consumer_tag);

  int socketno = amqp_get_sockfd(m_impl->m_connection);

	struct timeval tv_timeout;
	memset(&tv_timeout, 0, sizeof(tv_timeout));
	tv_timeout.tv_sec = timeout;

	while (true)
	{
		amqp_frame_t frame;
		amqp_maybe_release_buffers(m_impl->m_connection);
		
		// Possibly set a timeout on receiving
		// We only do this on the first frame otherwise we'd confuse
		// This function if it immediately turns around and gets called again
		if (timeout > 0)
    {
      fd_set fds;
      FD_ZERO(&fds);
      FD_SET(socketno, &fds);

      int select_return = select(socketno + 1, &fds, NULL, NULL, &tv_timeout);

      if (select_return == 0) // If it times out, return
      {
        return false;
      }
      else if (select_return == -1)
      {
        // If its an interupted system call just try again
        if (errno == EINTR)
        {
          continue;
        }
        else
        {
          std::string error_string("error calling select on socket: ");
#ifdef HAVE_STRERROR_S
          const int BUFFER_LENGTH = 256;
          char error_string_buffer[BUFFER_LENGTH] = {0};
          strerror_s(error_string_buffer, errno);
          error_string += error_string_buffer;
#elif defined(HAVE_STRERROR_R)
          const int BUFFER_LENGTH = 256;
          char error_string_buffer[BUFFER_LENGTH] = {0};
          strerror_r(errno, error_string_buffer, BUFFER_LENGTH);
          error_string += error_string_buffer;
#else
          error_string += strerror(errno);
#endif
          throw std::runtime_error(error_string.c_str());
        }
      }
    }

    m_impl->CheckForError(amqp_simple_wait_frame_on_channel(m_impl->m_connection, channel, &frame), "Channel::BasicConsumeMessage basic.deliver");

    if (frame.frame_type != AMQP_FRAME_METHOD)
      continue;

    m_impl->CheckFrameForClose(frame, channel);

    if (AMQP_BASIC_DELIVER_METHOD == frame.payload.method.id)
    {
      amqp_basic_deliver_t* deliver_method = reinterpret_cast<amqp_basic_deliver_t*>(frame.payload.method.decoded);

      const std::string exchange((char*)deliver_method->exchange.bytes, deliver_method->exchange.len);
      const std::string routing_key((char*)deliver_method->routing_key.bytes, deliver_method->routing_key.len);
      const std::string consumer_tag((char*)deliver_method->consumer_tag.bytes, deliver_method->consumer_tag.len);
      const uint64_t delivery_tag = deliver_method->delivery_tag;
      const bool redelivered = (deliver_method->redelivered == 0 ? false : true);
      BasicMessage::ptr_t content = m_impl->ReadContent(channel);
      content->DeliveryTag(delivery_tag);
      message = Envelope::Create(content, consumer_tag, delivery_tag, exchange, redelivered, routing_key);
      amqp_maybe_release_buffers(m_impl->m_connection);
      return true;
      break;
    }
	}
}

} // namespace AmqpClient
