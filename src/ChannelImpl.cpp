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

#include "SimpleAmqpClient/ChannelImpl.h"
#include "SimpleAmqpClient/AmqpResponseLibraryException.h"
#include "SimpleAmqpClient/AmqpException.h"
#include "SimpleAmqpClient/ConnectionClosedException.h"
#include "SimpleAmqpClient/ConsumerTagNotFoundException.h"

#include "config.h"

#include <boost/array.hpp>

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
# define NOMINMAX
# include <WinSock2.h>
#endif

namespace AmqpClient
{
namespace Detail
{

ChannelImpl::ChannelImpl() :
    m_is_connected(false)
    , m_next_channel_id(1)
{
    // Channel 0 is always open
    m_open_channels.insert(std::make_pair(0, frame_queue_t()));
}

ChannelImpl::~ChannelImpl()
{
}

amqp_channel_t ChannelImpl::GetNextChannelId()
{
    int max_channels = amqp_get_channel_max(m_connection);
    int channel_count = static_cast<int>(m_open_channels.size());
    if (0 == max_channels)
    {
        if (std::numeric_limits<boost::uint16_t>::max() <= channel_count)
        {
            throw std::runtime_error("Too many channels open");
        }
    }
    else if (max_channels <= channel_count)
    {
        throw std::runtime_error("Too many channels open");
    }

    while (m_open_channels.end() != m_open_channels.find(++m_next_channel_id))
    {
        /* Empty */
    }

    m_open_channels.insert(std::make_pair(m_next_channel_id, frame_queue_t()));
    return m_next_channel_id;
}

amqp_channel_t ChannelImpl::CreateNewChannel()
{
    amqp_channel_t new_channel = GetNextChannelId();

    static const boost::array<boost::uint32_t, 1> OPEN_OK = { { AMQP_CHANNEL_OPEN_OK_METHOD } };
    amqp_channel_open_t channel_open = {};
    DoRpcOnChannel<boost::array<boost::uint32_t, 1> >(new_channel, AMQP_CHANNEL_OPEN_METHOD, &channel_open, OPEN_OK);

    static const boost::array<boost::uint32_t, 1> CONFIRM_OK = { { AMQP_CONFIRM_SELECT_OK_METHOD } };
    amqp_confirm_select_t confirm_select = {};
    DoRpcOnChannel<boost::array<boost::uint32_t, 1> >(new_channel, AMQP_CONFIRM_SELECT_METHOD, &confirm_select, CONFIRM_OK);

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
}


bool ChannelImpl::IsChannelOpen(amqp_channel_t channel)
{
    return m_open_channels.find(channel) != m_open_channels.end();
}


void ChannelImpl::FinishCloseChannel(amqp_channel_t channel)
{
    amqp_channel_close_ok_t close_ok;
    m_open_channels.erase(channel);
    CheckForError(amqp_send_method(m_connection, channel, AMQP_CHANNEL_CLOSE_OK_METHOD, &close_ok));
}

void ChannelImpl::FinishCloseConnection()
{
    SetIsConnected(false);
    amqp_connection_close_ok_t close_ok;
    amqp_send_method(m_connection, 0, AMQP_CONNECTION_CLOSE_OK_METHOD, &close_ok);
}

void ChannelImpl::CheckRpcReply(amqp_channel_t channel, const amqp_rpc_reply_t &reply)
{
    switch (reply.reply_type)
    {
    case AMQP_RESPONSE_NORMAL:
        return;
        break;

    case AMQP_RESPONSE_LIBRARY_EXCEPTION:
        // If we're getting this likely is the socket is already closed
        throw AmqpResponseLibraryException::CreateException(reply, "");
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
        AmqpException::Throw(reply);
        break;

    default:
        AmqpException::Throw(reply);
    }
}

void ChannelImpl::CheckForError(int ret)
{
    if (ret < 0)
    {
        std::runtime_error error(amqp_error_string2(ret));
        throw error;
    }
}

MessageReturnedException ChannelImpl::CreateMessageReturnedException(amqp_basic_return_t &return_method, amqp_channel_t channel)
{
    const int reply_code = return_method.reply_code;
    const std::string reply_text((char *)return_method.reply_text.bytes, return_method.reply_text.len);
    const std::string exchange((char *)return_method.exchange.bytes, return_method.exchange.len);
    const std::string routing_key((char *)return_method.routing_key.bytes, return_method.routing_key.len);
    BasicMessage::ptr_t content = ReadContent(channel);
    return MessageReturnedException(content, reply_code, reply_text, exchange, routing_key);
}

BasicMessage::ptr_t ChannelImpl::ReadContent(amqp_channel_t channel)
{
    amqp_frame_t frame;

    GetNextFrameOnChannel(channel, frame);

    if (frame.frame_type != AMQP_FRAME_HEADER)
        // TODO: We should connection.close here
        throw std::runtime_error("Channel::BasicConsumeMessage: receieved unexpected frame type (was expected AMQP_FRAME_HEADER)");

    // The memory for this is allocated in a pool associated with the connection
    // The BasicMessage constructor does a deep copy of the properties structure
    amqp_basic_properties_t *properties = reinterpret_cast<amqp_basic_properties_t *>(frame.payload.properties.decoded);

    // size_t could possibly be 32-bit, body_size is always 64-bit
    assert(frame.payload.properties.body_size < static_cast<uint64_t>(std::numeric_limits<size_t>::max()));

    size_t body_size = static_cast<size_t>(frame.payload.properties.body_size);
    size_t received_size = 0;

    amqp_bytes_t body = amqp_bytes_malloc(body_size);

    // frame #3 and up:
    while (received_size < body_size)
    {
        GetNextFrameOnChannel(channel, frame);

        if (frame.frame_type != AMQP_FRAME_BODY)
            // TODO: we should connection.close here
            throw std::runtime_error("Channel::BasicConsumeMessge: received unexpected frame type (was expecting AMQP_FRAME_BODY)");

        void *body_ptr = reinterpret_cast<char *>(body.bytes) + received_size;
        memcpy(body_ptr, frame.payload.body_fragment.bytes, frame.payload.body_fragment.len);
        received_size += frame.payload.body_fragment.len;
    }
    return BasicMessage::Create(body, properties);
}

void ChannelImpl::CheckFrameForClose(amqp_frame_t &frame, amqp_channel_t channel)
{
    if (frame.frame_type == AMQP_FRAME_METHOD)
    {
        switch (frame.payload.method.id)
        {
        case AMQP_CHANNEL_CLOSE_METHOD:
            FinishCloseChannel(channel);
            AmqpException::Throw(*reinterpret_cast<amqp_channel_close_t *>(frame.payload.method.decoded));
            break;

        case AMQP_CONNECTION_CLOSE_METHOD:
            FinishCloseConnection();
            AmqpException::Throw(*reinterpret_cast<amqp_connection_close_t *>(frame.payload.method.decoded));
            break;
        }
    }
}

void ChannelImpl::AddConsumer(const std::string &consumer_tag, amqp_channel_t channel)
{
    m_consumer_channel_map.insert(std::make_pair(consumer_tag, channel));
}

amqp_channel_t ChannelImpl::RemoveConsumer(const std::string &consumer_tag)
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

amqp_channel_t ChannelImpl::GetConsumerChannel(const std::string &consumer_tag)
{
    std::map<std::string, amqp_channel_t>::const_iterator it = m_consumer_channel_map.find(consumer_tag);
    if (it == m_consumer_channel_map.end())
    {
        throw ConsumerTagNotFoundException();
    }
    return it->second;
}

ChannelImpl::channel_map_iterator_t ChannelImpl::GetChannelQueueOrThrow(amqp_channel_t channel)
{
    channel_map_iterator_t ret = m_open_channels.find(channel);
    if (m_open_channels.end() == ret)
    {
        throw std::runtime_error("Channel not found");
    }
    return ret;
}

bool ChannelImpl::GetNextFrameFromBroker(amqp_frame_t &frame, boost::chrono::microseconds timeout)
{
    struct timeval *tvp = NULL;
    struct timeval tv_timeout;
    memset(&tv_timeout, 0, sizeof(tv_timeout));

    if (timeout != boost::chrono::microseconds::max())
    {
        // boost::chrono::seconds.count() returns boost::int_atleast64_t,
        // long can be 32 or 64 bit depending on the platform/arch
        // unless the timeout is something absurd cast to long will be ok, but
        // lets guard against the case where someone does something silly
        assert(boost::chrono::duration_cast<boost::chrono::seconds>(timeout).count() <
                static_cast<boost::chrono::seconds::rep>(std::numeric_limits<long>::max()));

        tv_timeout.tv_sec = static_cast<long>(boost::chrono::duration_cast<boost::chrono::seconds>(timeout).count());
        tv_timeout.tv_usec = static_cast<long>((timeout - boost::chrono::seconds(tv_timeout.tv_sec)).count());

        tvp = &tv_timeout;
    }

    int ret = amqp_simple_wait_frame_noblock(m_connection, &frame, tvp);

    if (AMQP_STATUS_TIMEOUT == ret)
    {
        return false;
    }
    CheckForError(ret);
    return true;
}

bool ChannelImpl::GetNextFrameFromBrokerOnChannel(amqp_channel_t channel, amqp_frame_t &frame_out, boost::chrono::microseconds timeout)
{
    boost::chrono::steady_clock::time_point end_point;
    boost::chrono::microseconds timeout_left = timeout;
    if (timeout != boost::chrono::microseconds::max())
    {
        end_point = boost::chrono::steady_clock::now() + timeout;
    }

    amqp_frame_t frame;
    while (GetNextFrameFromBroker(frame, timeout_left))
    {
        if (frame.channel == channel)
        {
            frame_out = frame;
            return true;
        }

        if (frame.channel == 0)
        {
            // Only thing we care to handle on the channel0 is the connection.close method
            if (AMQP_FRAME_METHOD == frame.frame_type &&
                    AMQP_CONNECTION_CLOSE_METHOD == frame.payload.method.id)
            {
                FinishCloseConnection();
                AmqpException::Throw(*reinterpret_cast<amqp_connection_close_t *>(frame.payload.method.decoded));
            }
        }
        else
        {
            GetChannelQueueOrThrow(frame.channel)->second.push_back(frame);
        }

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

bool ChannelImpl::GetNextFrameOnChannel(amqp_channel_t channel, amqp_frame_t &frame, boost::chrono::microseconds timeout)
{
    frame_queue_t &channel_queue = GetChannelQueueOrThrow(channel)->second;

    if (!channel_queue.empty())
    {
        frame_queue_t::iterator it = channel_queue.begin();
        frame = *it;
        channel_queue.erase(it);

        if (AMQP_FRAME_METHOD == frame.frame_type &&
                AMQP_CHANNEL_CLOSE_METHOD == frame.payload.method.id)
        {
            FinishCloseChannel(channel);
            AmqpException::Throw(*reinterpret_cast<amqp_channel_close_t *>(frame.payload.method.decoded));
        }
        return true;
    }

    return GetNextFrameFromBrokerOnChannel(channel, frame, timeout);
}

void ChannelImpl::MaybeReleaseBuffers()
{
    // Check to see if we have any amqp_frame_t's lying around, if not we can tell the library to recycle its buffers
    bool buffers_empty = true;
    for (channel_map_iterator_t it = m_open_channels.begin(); it != m_open_channels.end(); ++it)
    {
        if (!it->second.empty())
        {
            buffers_empty = false;
            break;
        }
    }

    if (buffers_empty)
    {
        amqp_maybe_release_buffers(m_connection);
    }
}

void ChannelImpl::CheckIsConnected()
{
    if (!m_is_connected)
    {
        throw ConnectionClosedException();
    }
}

} // namespace Detail
} // namespace AmqpClient
