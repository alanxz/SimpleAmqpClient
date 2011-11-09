#ifndef CHANNELIMPL_H_
#define CHANNELIMPL_H_

#include "SimpleAmqpClient/BasicMessage.h"
#include "SimpleAmqpClient/Envelope.h"
#include "SimpleAmqpClient/MessageReturnedException.h"

#include <amqp.h>
#include <amqp_framing.h>

#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/noncopyable.hpp>

#include <map>
#include <queue>
#include <vector>

namespace AmqpClient
{
namespace Detail
{


class ChannelImpl : boost::noncopyable
{
public:
  ChannelImpl();
  virtual ~ChannelImpl();

  typedef std::vector<amqp_frame_t> frame_queue_t;
  typedef std::map<amqp_channel_t, frame_queue_t> channel_map_t;
  typedef channel_map_t::iterator channel_map_iterator_t;

  amqp_channel_t GetChannel();
  void ReturnChannel(amqp_channel_t channel);
  bool IsChannelOpen(amqp_channel_t channel);
  channel_map_iterator_t GetChannelQueueOrThrow(amqp_channel_t channel);

  bool GetNextFrameFromBroker(amqp_frame_t& frame, boost::chrono::microseconds timeout);
  bool GetNextFrameFromBrokerOnChannel(amqp_channel_t channel, amqp_frame_t& frame, boost::chrono::microseconds timeout = boost::chrono::microseconds::max());
  bool GetNextFrameOnChannel(amqp_channel_t channel, amqp_frame_t& frame, boost::chrono::microseconds timeout = boost::chrono::microseconds::max());

  template <class ResponseListType>
  static bool is_expected_method(const amqp_frame_t& frame, const ResponseListType& expected_responses)
  {
    return AMQP_FRAME_METHOD == frame.frame_type &&
      expected_responses.end() != std::find(expected_responses.begin(), expected_responses.end(), frame.payload.method.id);
  }

  template <class ResponseListType>
  bool GetMethodOnChannel(amqp_channel_t channel, amqp_frame_t& frame, 
    const ResponseListType& expected_responses, 
    boost::chrono::microseconds timeout = boost::chrono::microseconds::max())
  {
    // Check to see a desired frame is in the frame queue for the channel
    frame_queue_t channel_queue = GetChannelQueueOrThrow(channel)->second;

    frame_queue_t::iterator desired_frame = 
      std::find_if(channel_queue.begin(), channel_queue.end(), 
      boost::bind(&ChannelImpl::is_expected_method<ResponseListType>, _1, expected_responses));

    if (channel_queue.end() != desired_frame)
    {
      frame = *desired_frame;
      channel_queue.erase(desired_frame);
      return true;
    }

    boost::chrono::steady_clock::time_point end_point;
    boost::chrono::microseconds timeout_left = timeout;
    if (timeout != boost::chrono::microseconds::max())
    {
      end_point = boost::chrono::steady_clock::now() + timeout;
    }

    amqp_frame_t incoming_frame;
    while (GetNextFrameFromBrokerOnChannel(channel, incoming_frame, timeout_left))
    {
      if (is_expected_method(incoming_frame, expected_responses))
      {
        frame = incoming_frame;
        return true;
      }
      if (AMQP_FRAME_METHOD == incoming_frame.frame_type &&
        AMQP_CHANNEL_CLOSE_METHOD == incoming_frame.payload.method.id)
      {
        FinishCloseChannel(channel);
        throw AmqpResponseServerException(*reinterpret_cast<amqp_channel_close_t*>(incoming_frame.payload.method.decoded), "ChannelImpl::GetMethodOnChannel");
      }
      GetChannelQueueOrThrow(channel)->second.push_back(incoming_frame);

      if (timeout != boost::chrono::microseconds::max())
      {
        boost::chrono::steady_clock::time_point now = boost::chrono::steady_clock::now();
        if (now >= end_point)
        {
          return false;
        }
        timeout_left = boost::chrono::duration_cast<boost::chrono::microseconds>(end_point - now);
      }
    }
    return false;
  }

  template <class ResponseListType>
  amqp_frame_t DoRpcOnChannel(amqp_channel_t channel, uint32_t method_id, void* decoded, const ResponseListType& expected_responses)
  {
    CheckForError(amqp_send_method(m_connection, channel, method_id, decoded), "AmqpImpl::DoImpl amqp_send_method");

    amqp_frame_t response;
    GetMethodOnChannel(channel, response, expected_responses);
    return response;
  }

  template <class ResponseListType>
  amqp_frame_t DoRpc(uint32_t method_id, void* decoded, const ResponseListType& expected_responses)
  {
    amqp_channel_t channel = GetChannel();
    return DoRpcOnChannel(channel, method_id, decoded, expected_responses);
  }

  amqp_channel_t CreateNewChannel();
  amqp_channel_t GetNextChannelId();

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

  void MaybeReleaseBuffers();

  amqp_connection_state_t m_connection;

private:
  std::queue<amqp_channel_t> m_free_channels;
  channel_map_t m_open_channels;
  uint16_t m_next_channel_id;
  std::map<std::string, amqp_channel_t> m_consumer_channel_map;
};

} // namespace Detail
} // namespace AmqpClient
#endif // CHANNELIMPL_H_