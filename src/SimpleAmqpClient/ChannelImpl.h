#ifndef SIMPLEAMQPCLIENT_CHANNELIMPL_H
#define SIMPLEAMQPCLIENT_CHANNELIMPL_H
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

// Put these first to avoid warnings about INT#_C macro redefinition
#include <amqp.h>
#include <amqp_framing.h>

#include "SimpleAmqpClient/AmqpException.h"
#include "SimpleAmqpClient/BasicMessage.h"
#include "SimpleAmqpClient/ConsumerCancelledException.h"
#include "SimpleAmqpClient/Envelope.h"
#include "SimpleAmqpClient/MessageReturnedException.h"

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/noncopyable.hpp>

#include <map>
#include <vector>

namespace AmqpClient {
namespace Detail {

class ChannelImpl : boost::noncopyable {
 public:
  ChannelImpl();
  virtual ~ChannelImpl();

  typedef std::vector<amqp_channel_t> channel_list_t;
  typedef std::vector<amqp_frame_t> frame_queue_t;
  typedef std::map<amqp_channel_t, frame_queue_t> channel_map_t;
  typedef channel_map_t::iterator channel_map_iterator_t;

  void DoLogin(const std::string &username, const std::string &password,
               const std::string &vhost, int frame_max);
  amqp_channel_t GetChannel();
  void ReturnChannel(amqp_channel_t channel);
  bool IsChannelOpen(amqp_channel_t channel);

  bool GetNextFrameFromBroker(amqp_frame_t &frame,
                              boost::chrono::microseconds timeout);

  bool CheckForQueuedMessageOnChannel(amqp_channel_t message_on_channel) const;
  void AddToFrameQueue(const amqp_frame_t &frame);

  template <class ChannelListType>
  bool GetNextFrameFromBrokerOnChannel(const ChannelListType channels,
                                       amqp_frame_t &frame_out,
                                       boost::chrono::microseconds timeout =
                                           boost::chrono::microseconds::max()) {
    boost::chrono::steady_clock::time_point end_point;
    boost::chrono::microseconds timeout_left = timeout;
    if (timeout != boost::chrono::microseconds::max()) {
      end_point = boost::chrono::steady_clock::now() + timeout;
    }

    amqp_frame_t frame;
    while (GetNextFrameFromBroker(frame, timeout_left)) {
      if (channels.end() !=
          std::find(channels.begin(), channels.end(), frame.channel)) {
        frame_out = frame;
        return true;
      }

      if (frame.channel == 0) {
        // Only thing we care to handle on the channel0 is the connection.close
        // method
        if (AMQP_FRAME_METHOD == frame.frame_type &&
            AMQP_CONNECTION_CLOSE_METHOD == frame.payload.method.id) {
          FinishCloseConnection();
          AmqpException::Throw(*reinterpret_cast<amqp_connection_close_t *>(
              frame.payload.method.decoded));
        }
      } else {
        AddToFrameQueue(frame);
      }

      if (timeout != boost::chrono::microseconds::max()) {
        boost::chrono::steady_clock::time_point now =
            boost::chrono::steady_clock::now();
        if (now >= end_point) {
          return false;
        }
        timeout_left =
            boost::chrono::duration_cast<boost::chrono::microseconds>(
                end_point - now);
      }
    }
    return false;
  }

  bool GetNextFrameOnChannel(
      amqp_channel_t channel, amqp_frame_t &frame,
      boost::chrono::microseconds timeout = boost::chrono::microseconds::max());

  static bool is_on_channel(const amqp_frame_t frame, amqp_channel_t channel) {
    return channel == frame.channel;
  }

  static bool is_frame_type_on_channel(const amqp_frame_t frame,
                                       uint8_t frame_type,
                                       amqp_channel_t channel) {
    return frame.frame_type == frame_type && frame.channel == channel;
  }

  static bool is_method_on_channel(const amqp_frame_t frame,
                                   amqp_method_number_t method,
                                   amqp_channel_t channel) {
    return frame.channel == channel && frame.frame_type == AMQP_FRAME_METHOD &&
           frame.payload.method.id == method;
  }

  template <class ChannelListType, class ResponseListType>
  static bool is_expected_method_on_channel(
      const amqp_frame_t &frame, const ChannelListType channels,
      const ResponseListType &expected_responses) {
    return channels.end() !=
               std::find(channels.begin(), channels.end(), frame.channel) &&
           AMQP_FRAME_METHOD == frame.frame_type &&
           expected_responses.end() != std::find(expected_responses.begin(),
                                                 expected_responses.end(),
                                                 frame.payload.method.id);
  }

  template <class ChannelListType, class ResponseListType>
  bool GetMethodOnChannel(const ChannelListType channels, amqp_frame_t &frame,
                          const ResponseListType &expected_responses,
                          boost::chrono::microseconds timeout =
                              boost::chrono::microseconds::max()) {
    frame_queue_t::iterator desired_frame = std::find_if(
        m_frame_queue.begin(), m_frame_queue.end(),
        boost::bind(
            &ChannelImpl::is_expected_method_on_channel<ChannelListType,
                                                        ResponseListType>,
            _1, channels, expected_responses));

    if (m_frame_queue.end() != desired_frame) {
      frame = *desired_frame;
      m_frame_queue.erase(desired_frame);
      return true;
    }

    boost::chrono::steady_clock::time_point end_point;
    boost::chrono::microseconds timeout_left = timeout;
    if (timeout != boost::chrono::microseconds::max()) {
      end_point = boost::chrono::steady_clock::now() + timeout;
    }

    amqp_frame_t incoming_frame;
    while (GetNextFrameFromBrokerOnChannel(channels, incoming_frame,
                                           timeout_left)) {
      if (is_expected_method_on_channel(incoming_frame, channels,
                                        expected_responses)) {
        frame = incoming_frame;
        return true;
      }
      if (AMQP_FRAME_METHOD == incoming_frame.frame_type &&
          AMQP_CHANNEL_CLOSE_METHOD == incoming_frame.payload.method.id) {
        FinishCloseChannel(incoming_frame.channel);
        try {
          AmqpException::Throw(*reinterpret_cast<amqp_channel_close_t *>(
              incoming_frame.payload.method.decoded));
        } catch (AmqpException &) {
          MaybeReleaseBuffersOnChannel(incoming_frame.channel);
          throw;
        }
      }
      m_frame_queue.push_back(incoming_frame);

      if (timeout != boost::chrono::microseconds::max()) {
        boost::chrono::steady_clock::time_point now =
            boost::chrono::steady_clock::now();
        if (now >= end_point) {
          return false;
        }
        timeout_left =
            boost::chrono::duration_cast<boost::chrono::microseconds>(
                end_point - now);
      }
    }
    return false;
  }

  template <class ResponseListType>
  amqp_frame_t DoRpcOnChannel(amqp_channel_t channel, boost::uint32_t method_id,
                              void *decoded,
                              const ResponseListType &expected_responses) {
    CheckForError(amqp_send_method(m_connection, channel, method_id, decoded));

    amqp_frame_t response;
    boost::array<amqp_channel_t, 1> channels = {{channel}};

    GetMethodOnChannel(channels, response, expected_responses);
    return response;
  }

  template <class ResponseListType>
  amqp_frame_t DoRpc(boost::uint32_t method_id, void *decoded,
                     const ResponseListType &expected_responses) {
    amqp_channel_t channel = GetChannel();
    amqp_frame_t ret =
        DoRpcOnChannel(channel, method_id, decoded, expected_responses);
    ReturnChannel(channel);
    return ret;
  }

  template <class ChannelListType>
  static bool envelope_on_channel(const Envelope::ptr_t &envelope,
                                  const ChannelListType channels) {
    return channels.end() != std::find(channels.begin(), channels.end(),
                                       envelope->DeliveryChannel());
  }

  template <class ChannelListType>
  bool ConsumeMessageOnChannel(const ChannelListType channels,
                               Envelope::ptr_t &message, int timeout) {
    envelope_list_t::iterator it = std::find_if(
        m_delivered_messages.begin(), m_delivered_messages.end(),
        boost::bind(ChannelImpl::envelope_on_channel<ChannelListType>, _1,
                    channels));

    if (it != m_delivered_messages.end()) {
      message = *it;
      m_delivered_messages.erase(it);
      return true;
    }

    return ConsumeMessageOnChannelInner(channels, message, timeout);
  }

  template <class ChannelListType>
  bool ConsumeMessageOnChannelInner(const ChannelListType channels,
                                    Envelope::ptr_t &message, int timeout) {
    const boost::array<boost::uint32_t, 2> DELIVER_OR_CANCEL = {
        {AMQP_BASIC_DELIVER_METHOD, AMQP_BASIC_CANCEL_METHOD}};

    boost::chrono::microseconds real_timeout =
        (timeout >= 0 ? boost::chrono::milliseconds(timeout)
                      : boost::chrono::microseconds::max());

    amqp_frame_t deliver;
    if (!GetMethodOnChannel(channels, deliver, DELIVER_OR_CANCEL,
                            real_timeout)) {
      return false;
    }

    if (AMQP_BASIC_CANCEL_METHOD == deliver.payload.method.id) {
      amqp_basic_cancel_t *cancel_method =
          reinterpret_cast<amqp_basic_cancel_t *>(
              deliver.payload.method.decoded);
      std::string consumer_tag((char *)cancel_method->consumer_tag.bytes,
                               cancel_method->consumer_tag.len);

      RemoveConsumer(consumer_tag);
      ReturnChannel(deliver.channel);
      MaybeReleaseBuffersOnChannel(deliver.channel);

      throw ConsumerCancelledException(consumer_tag);
    }

    amqp_basic_deliver_t *deliver_method =
        reinterpret_cast<amqp_basic_deliver_t *>(
            deliver.payload.method.decoded);

    const std::string exchange((char *)deliver_method->exchange.bytes,
                               deliver_method->exchange.len);
    const std::string routing_key((char *)deliver_method->routing_key.bytes,
                                  deliver_method->routing_key.len);
    const std::string in_consumer_tag(
        (char *)deliver_method->consumer_tag.bytes,
        deliver_method->consumer_tag.len);
    const boost::uint64_t delivery_tag = deliver_method->delivery_tag;
    const bool redelivered = (deliver_method->redelivered == 0 ? false : true);
    MaybeReleaseBuffersOnChannel(deliver.channel);

    BasicMessage::ptr_t content = ReadContent(deliver.channel);
    MaybeReleaseBuffersOnChannel(deliver.channel);

    message = Envelope::Create(content, in_consumer_tag, delivery_tag, exchange,
                               redelivered, routing_key, deliver.channel);
    return true;
  }

  amqp_channel_t CreateNewChannel();
  amqp_channel_t GetNextChannelId();

  void CheckRpcReply(amqp_channel_t channel, const amqp_rpc_reply_t &reply);
  void CheckForError(int ret);

  void CheckFrameForClose(amqp_frame_t &frame, amqp_channel_t channel);
  void FinishCloseChannel(amqp_channel_t channel);
  void FinishCloseConnection();

  MessageReturnedException CreateMessageReturnedException(
      amqp_basic_return_t &return_method, amqp_channel_t channel);
  AmqpClient::BasicMessage::ptr_t ReadContent(amqp_channel_t channel);

  void AddConsumer(const std::string &consumer_tag, amqp_channel_t channel);
  amqp_channel_t RemoveConsumer(const std::string &consumer_tag);
  amqp_channel_t GetConsumerChannel(const std::string &consumer_tag);
  std::vector<amqp_channel_t> GetAllConsumerChannels() const;

  void MaybeReleaseBuffersOnChannel(amqp_channel_t channel);
  void CheckIsConnected();
  void SetIsConnected(bool state) { m_is_connected = state; }

  // The RabbitMQ broker changed the way that basic.qos worked as of v3.3.0.
  // See: http://www.rabbitmq.com/consumer-prefetch.html
  // Newer versions of RabbitMQ basic.qos.global set to false applies to new
  // consumers made on the channel, and true applies to all consumers on the
  // channel (not connection).
  bool BrokerHasNewQosBehavior() const { return 0x030300 <= m_brokerVersion; }

  amqp_connection_state_t m_connection;

 private:
  static boost::uint32_t ComputeBrokerVersion(
      const amqp_connection_state_t state);

  frame_queue_t m_frame_queue;

  typedef std::vector<Envelope::ptr_t> envelope_list_t;
  envelope_list_t m_delivered_messages;

  typedef std::map<std::string, amqp_channel_t> consumer_map_t;
  consumer_map_t m_consumer_channel_map;

  enum channel_state_t { CS_Closed = 0, CS_Open, CS_Used };
  typedef std::vector<channel_state_t> channel_state_list_t;

  channel_state_list_t m_channels;
  boost::uint32_t m_brokerVersion;
  // A channel that is likely to be an CS_Open state
  amqp_channel_t m_last_used_channel;

  bool m_is_connected;
};

}  // namespace Detail
}  // namespace AmqpClient
#endif  // SIMPLEAMQPCLIENT_CHANNELIMPL_H
