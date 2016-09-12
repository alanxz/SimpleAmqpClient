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

#ifdef _WIN32
#define NOMINMAX
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Winsock2.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#endif

#include "SimpleAmqpClient/AmqpException.h"
#include "SimpleAmqpClient/AmqpLibraryException.h"
#include "SimpleAmqpClient/AmqpResponseLibraryException.h"
#include "SimpleAmqpClient/ChannelImpl.h"
#include "SimpleAmqpClient/ConnectionClosedException.h"
#include "SimpleAmqpClient/ConsumerTagNotFoundException.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/array.hpp>
#include <boost/lexical_cast.hpp>

#include <string.h>

#define BROKER_HEARTBEAT 0

namespace AmqpClient {
namespace Detail {

ChannelImpl::ChannelImpl() : m_last_used_channel(0), m_is_connected(false) {
  m_channels.push_back(CS_Used);
}

ChannelImpl::~ChannelImpl() {}

void ChannelImpl::DoLogin(const std::string &username,
                          const std::string &password, const std::string &vhost,
                          int frame_max) {
  amqp_table_entry_t capabilties[1];
  amqp_table_entry_t capability_entry;
  amqp_table_t client_properties;

  capabilties[0].key = amqp_cstring_bytes("consumer_cancel_notify");
  capabilties[0].value.kind = AMQP_FIELD_KIND_BOOLEAN;
  capabilties[0].value.value.boolean = 1;

  capability_entry.key = amqp_cstring_bytes("capabilities");
  capability_entry.value.kind = AMQP_FIELD_KIND_TABLE;
  capability_entry.value.value.table.num_entries =
      sizeof(capabilties) / sizeof(amqp_table_entry_t);
  capability_entry.value.value.table.entries = capabilties;

  client_properties.num_entries = 1;
  client_properties.entries = &capability_entry;

  CheckRpcReply(
      0, amqp_login_with_properties(m_connection, vhost.c_str(), 0, frame_max,
                                    BROKER_HEARTBEAT, &client_properties,
                                    AMQP_SASL_METHOD_PLAIN, username.c_str(),
                                    password.c_str()));

  m_brokerVersion = ComputeBrokerVersion(m_connection);
}

amqp_channel_t ChannelImpl::GetNextChannelId() {
  channel_state_list_t::iterator unused_channel =
      std::find(m_channels.begin(), m_channels.end(), CS_Closed);

  if (m_channels.end() == unused_channel) {
    int max_channels = amqp_get_channel_max(m_connection);
    if (0 == max_channels) {
      max_channels = std::numeric_limits<uint16_t>::max();
    }
    if (static_cast<size_t>(max_channels) < m_channels.size()) {
      throw std::runtime_error("Too many channels open");
    }

    m_channels.push_back(CS_Closed);
    unused_channel = m_channels.end() - 1;
  }

  return unused_channel - m_channels.begin();
}

amqp_channel_t ChannelImpl::CreateNewChannel() {
  amqp_channel_t new_channel = GetNextChannelId();

  static const boost::array<boost::uint32_t, 1> OPEN_OK = {
      {AMQP_CHANNEL_OPEN_OK_METHOD}};
  amqp_channel_open_t channel_open = {};
  DoRpcOnChannel<boost::array<boost::uint32_t, 1> >(
      new_channel, AMQP_CHANNEL_OPEN_METHOD, &channel_open, OPEN_OK);

  static const boost::array<boost::uint32_t, 1> CONFIRM_OK = {
      {AMQP_CONFIRM_SELECT_OK_METHOD}};
  amqp_confirm_select_t confirm_select = {};
  DoRpcOnChannel<boost::array<boost::uint32_t, 1> >(
      new_channel, AMQP_CONFIRM_SELECT_METHOD, &confirm_select, CONFIRM_OK);

  m_channels.at(new_channel) = CS_Open;

  return new_channel;
}

amqp_channel_t ChannelImpl::GetChannel() {
  if (CS_Open == m_channels.at(m_last_used_channel)) {
    m_channels[m_last_used_channel] = CS_Used;
    return m_last_used_channel;
  }

  channel_state_list_t::iterator it =
      std::find(m_channels.begin(), m_channels.end(), CS_Open);

  if (m_channels.end() == it) {
    amqp_channel_t new_channel = CreateNewChannel();
    m_channels.at(new_channel) = CS_Used;
    return new_channel;
  }

  *it = CS_Used;
  return it - m_channels.begin();
}

void ChannelImpl::ReturnChannel(amqp_channel_t channel) {
  m_channels.at(channel) = CS_Open;
  m_last_used_channel = channel;
}

bool ChannelImpl::IsChannelOpen(amqp_channel_t channel) {
  return CS_Closed != m_channels.at(channel);
}

void ChannelImpl::FinishCloseChannel(amqp_channel_t channel) {
  m_channels.at(channel) = CS_Closed;

  amqp_channel_close_ok_t close_ok;
  CheckForError(amqp_send_method(m_connection, channel,
                                 AMQP_CHANNEL_CLOSE_OK_METHOD, &close_ok));
}

void ChannelImpl::FinishCloseConnection() {
  SetIsConnected(false);
  amqp_connection_close_ok_t close_ok;
  amqp_send_method(m_connection, 0, AMQP_CONNECTION_CLOSE_OK_METHOD, &close_ok);
}

void ChannelImpl::CheckRpcReply(amqp_channel_t channel,
                                const amqp_rpc_reply_t &reply) {
  switch (reply.reply_type) {
    case AMQP_RESPONSE_NORMAL:
      return;
      break;

    case AMQP_RESPONSE_LIBRARY_EXCEPTION:
      // If we're getting this likely is the socket is already closed
      throw AmqpResponseLibraryException::CreateException(reply, "");
      break;

    case AMQP_RESPONSE_SERVER_EXCEPTION:
      if (reply.reply.id == AMQP_CHANNEL_CLOSE_METHOD) {
        FinishCloseChannel(channel);
      } else if (reply.reply.id == AMQP_CONNECTION_CLOSE_METHOD) {
        FinishCloseConnection();
      }
      AmqpException::Throw(reply);
      break;

    default:
      AmqpException::Throw(reply);
  }
}

void ChannelImpl::CheckForError(int ret) {
  if (ret < 0) {
    throw AmqpLibraryException::CreateException(ret);
  }
}

MessageReturnedException ChannelImpl::CreateMessageReturnedException(
    amqp_basic_return_t &return_method, amqp_channel_t channel) {
  const int reply_code = return_method.reply_code;
  const std::string reply_text((char *)return_method.reply_text.bytes,
                               return_method.reply_text.len);
  const std::string exchange((char *)return_method.exchange.bytes,
                             return_method.exchange.len);
  const std::string routing_key((char *)return_method.routing_key.bytes,
                                return_method.routing_key.len);
  BasicMessage::ptr_t content = ReadContent(channel);
  return MessageReturnedException(content, reply_code, reply_text, exchange,
                                  routing_key);
}

BasicMessage::ptr_t ChannelImpl::ReadContent(amqp_channel_t channel) {
  amqp_frame_t frame;

  GetNextFrameOnChannel(channel, frame);

  if (frame.frame_type != AMQP_FRAME_HEADER)
    // TODO: We should connection.close here
    throw std::runtime_error(
        "Channel::BasicConsumeMessage: received unexpected frame type (was "
        "expected AMQP_FRAME_HEADER)");

  // The memory for this is allocated in a pool associated with the connection
  // The BasicMessage constructor does a deep copy of the properties structure
  amqp_basic_properties_t *properties =
      reinterpret_cast<amqp_basic_properties_t *>(
          frame.payload.properties.decoded);

  // size_t could possibly be 32-bit, body_size is always 64-bit
  assert(frame.payload.properties.body_size <
         static_cast<uint64_t>(std::numeric_limits<size_t>::max()));

  size_t body_size = static_cast<size_t>(frame.payload.properties.body_size);
  size_t received_size = 0;

  amqp_bytes_t body = amqp_bytes_malloc(body_size);

  // frame #3 and up:
  while (received_size < body_size) {
    GetNextFrameOnChannel(channel, frame);

    if (frame.frame_type != AMQP_FRAME_BODY)
      // TODO: we should connection.close here
      throw std::runtime_error(
          "Channel::BasicConsumeMessage: received unexpected frame type (was "
          "expecting AMQP_FRAME_BODY)");

    void *body_ptr = reinterpret_cast<char *>(body.bytes) + received_size;
    memcpy(body_ptr, frame.payload.body_fragment.bytes,
           frame.payload.body_fragment.len);
    received_size += frame.payload.body_fragment.len;
  }
  return BasicMessage::Create(body, properties);
}

void ChannelImpl::CheckFrameForClose(amqp_frame_t &frame,
                                     amqp_channel_t channel) {
  if (frame.frame_type == AMQP_FRAME_METHOD) {
    switch (frame.payload.method.id) {
      case AMQP_CHANNEL_CLOSE_METHOD:
        FinishCloseChannel(channel);
        AmqpException::Throw(*reinterpret_cast<amqp_channel_close_t *>(
            frame.payload.method.decoded));
        break;

      case AMQP_CONNECTION_CLOSE_METHOD:
        FinishCloseConnection();
        AmqpException::Throw(*reinterpret_cast<amqp_connection_close_t *>(
            frame.payload.method.decoded));
        break;
    }
  }
}

void ChannelImpl::AddConsumer(const std::string &consumer_tag,
                              amqp_channel_t channel) {
  m_consumer_channel_map.insert(std::make_pair(consumer_tag, channel));
}

amqp_channel_t ChannelImpl::RemoveConsumer(const std::string &consumer_tag) {
  std::map<std::string, amqp_channel_t>::iterator it =
      m_consumer_channel_map.find(consumer_tag);
  if (it == m_consumer_channel_map.end()) {
    throw ConsumerTagNotFoundException();
  }

  amqp_channel_t result = it->second;

  m_consumer_channel_map.erase(it);

  return result;
}

amqp_channel_t ChannelImpl::GetConsumerChannel(
    const std::string &consumer_tag) {
  std::map<std::string, amqp_channel_t>::const_iterator it =
      m_consumer_channel_map.find(consumer_tag);
  if (it == m_consumer_channel_map.end()) {
    throw ConsumerTagNotFoundException();
  }
  return it->second;
}

std::vector<amqp_channel_t> ChannelImpl::GetAllConsumerChannels() const {
  std::vector<amqp_channel_t> ret;
  for (consumer_map_t::const_iterator it = m_consumer_channel_map.begin();
       it != m_consumer_channel_map.end(); ++it) {
    ret.push_back(it->second);
  }

  return ret;
}

bool ChannelImpl::CheckForQueuedMessageOnChannel(amqp_channel_t channel) const {
  frame_queue_t::const_iterator it =
      std::find_if(m_frame_queue.begin(), m_frame_queue.end(),
                   boost::bind(&ChannelImpl::is_method_on_channel, _1,
                               AMQP_BASIC_DELIVER_METHOD, channel));

  if (it == m_frame_queue.end()) {
    return false;
  }

  it = std::find_if(it + 1, m_frame_queue.end(),
                    boost::bind(&ChannelImpl::is_on_channel, _1, channel));

  if (it == m_frame_queue.end()) {
    return false;
  }
  if (it->frame_type != AMQP_FRAME_HEADER) {
    throw std::runtime_error("Protocol error");
  }

  uint64_t body_length = it->payload.properties.body_size;
  uint64_t body_received = 0;

  while (body_received < body_length) {
    it = std::find_if(it + 1, m_frame_queue.end(),
                      boost::bind(&ChannelImpl::is_on_channel, _1, channel));

    if (it == m_frame_queue.end()) {
      return false;
    }
    if (it->frame_type != AMQP_FRAME_BODY) {
      throw std::runtime_error("Protocol error");
    }
    body_received += it->payload.body_fragment.len;
  }

  return true;
}

void ChannelImpl::AddToFrameQueue(const amqp_frame_t &frame) {
  m_frame_queue.push_back(frame);

  if (CheckForQueuedMessageOnChannel(frame.channel)) {
    boost::array<amqp_channel_t, 1> channel = {{frame.channel}};
    Envelope::ptr_t envelope;
    if (!ConsumeMessageOnChannelInner(channel, envelope, -1)) {
      throw std::logic_error(
          "ConsumeMessageOnChannelInner returned false unexpectedly");
    }

    m_delivered_messages.push_back(envelope);
  }
}

bool ChannelImpl::GetNextFrameFromBroker(amqp_frame_t &frame,
                                         boost::chrono::microseconds timeout) {
  struct timeval *tvp = NULL;
  struct timeval tv_timeout;
  memset(&tv_timeout, 0, sizeof(tv_timeout));

  if (timeout != boost::chrono::microseconds::max()) {
    // boost::chrono::seconds.count() returns boost::int_atleast64_t,
    // long can be 32 or 64 bit depending on the platform/arch
    // unless the timeout is something absurd cast to long will be ok, but
    // lets guard against the case where someone does something silly
    assert(
        boost::chrono::duration_cast<boost::chrono::seconds>(timeout).count() <
        static_cast<boost::chrono::seconds::rep>(
            std::numeric_limits<long>::max()));

    tv_timeout.tv_sec = static_cast<long>(
        boost::chrono::duration_cast<boost::chrono::seconds>(timeout).count());
    tv_timeout.tv_usec = static_cast<long>(
        (timeout - boost::chrono::seconds(tv_timeout.tv_sec)).count());

    tvp = &tv_timeout;
  }

  int ret = amqp_simple_wait_frame_noblock(m_connection, &frame, tvp);

  if (AMQP_STATUS_TIMEOUT == ret) {
    return false;
  }
  CheckForError(ret);
  return true;
}

bool ChannelImpl::GetNextFrameOnChannel(amqp_channel_t channel,
                                        amqp_frame_t &frame,
                                        boost::chrono::microseconds timeout) {
  frame_queue_t::iterator it =
      std::find_if(m_frame_queue.begin(), m_frame_queue.end(),
                   boost::bind(&ChannelImpl::is_on_channel, _1, channel));

  if (m_frame_queue.end() != it) {
    frame = *it;
    m_frame_queue.erase(it);

    if (AMQP_FRAME_METHOD == frame.frame_type &&
        AMQP_CHANNEL_CLOSE_METHOD == frame.payload.method.id) {
      FinishCloseChannel(channel);
      AmqpException::Throw(*reinterpret_cast<amqp_channel_close_t *>(
          frame.payload.method.decoded));
    }
    return true;
  }

  boost::array<amqp_channel_t, 1> channels = {{channel}};
  return GetNextFrameFromBrokerOnChannel(channels, frame, timeout);
}

void ChannelImpl::MaybeReleaseBuffersOnChannel(amqp_channel_t channel) {
  if (m_frame_queue.end() ==
      std::find_if(m_frame_queue.begin(), m_frame_queue.end(),
                   boost::bind(&ChannelImpl::is_on_channel, _1, channel))) {
    amqp_maybe_release_buffers_on_channel(m_connection, channel);
  }
}

void ChannelImpl::CheckIsConnected() {
  if (!m_is_connected) {
    throw ConnectionClosedException();
  }
}

namespace {
bool bytesEqual(amqp_bytes_t r, amqp_bytes_t l) {
  if (r.len == l.len) {
    if (0 == memcmp(r.bytes, l.bytes, r.len)) {
      return true;
    }
  }
  return false;
}
}

boost::uint32_t ChannelImpl::ComputeBrokerVersion(
    amqp_connection_state_t state) {
  const amqp_table_t *properties = amqp_get_server_properties(state);
  const amqp_bytes_t version = amqp_cstring_bytes("version");
  amqp_table_entry_t *version_entry = NULL;

  for (int i = 0; i < properties->num_entries; ++i) {
    if (bytesEqual(properties->entries[i].key, version)) {
      version_entry = &properties->entries[i];
      break;
    }
  }
  if (NULL == version_entry) {
    return 0;
  }

  std::string version_string(
      static_cast<char *>(version_entry->value.value.bytes.bytes),
      version_entry->value.value.bytes.len);
  std::vector<std::string> version_components;
  boost::split(version_components, version_string, boost::is_any_of("."));
  if (version_components.size() != 3) {
    return 0;
  }
  boost::uint32_t version_major =
      boost::lexical_cast<boost::uint32_t>(version_components[0]);
  boost::uint32_t version_minor =
      boost::lexical_cast<boost::uint32_t>(version_components[1]);
  boost::uint32_t version_patch =
      boost::lexical_cast<boost::uint32_t>(version_components[2]);
  return (version_major & 0xFF) << 16 | (version_minor & 0xFF) << 8 |
         (version_patch & 0xFF);
}

}  // namespace Detail
}  // namespace AmqpClient
