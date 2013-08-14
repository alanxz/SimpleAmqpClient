/* vim:set ft=cpp ts=4 sw=4 sts=4 et cindent: */
#ifndef SIMPLERPCSERVER_H
#define SIMPLERPCSERVER_H
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

#include "SimpleAmqpClient/BasicMessage.h"
#include "SimpleAmqpClient/Channel.h"
#include "SimpleAmqpClient/Util.h"

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <string>

#ifdef _MSC_VER
# pragma warning ( push )
# pragma warning ( disable: 4275 4251 )
#endif

namespace AmqpClient
{


class SIMPLEAMQPCLIENT_EXPORT SimpleRpcServer : boost::noncopyable
{
public:
    typedef boost::shared_ptr<SimpleRpcServer> ptr_t;

    friend ptr_t boost::make_shared<SimpleRpcServer>(AmqpClient::Channel::ptr_t const &a1, std::string const &a2);

    static ptr_t Create(Channel::ptr_t channel, const std::string &rpc_name = "")
    {
        return boost::make_shared<SimpleRpcServer>(channel, rpc_name);
    }

private:
    explicit SimpleRpcServer(Channel::ptr_t channel, const std::string &rpc_name);

public:
    virtual ~SimpleRpcServer();

    std::string GetRpcName() const
    {
        return m_incoming_tag;
    }

    BasicMessage::ptr_t GetNextIncomingMessage();

    bool GetNextIncomingMessage(BasicMessage::ptr_t &message, int timeout);

    void RespondToMessage(BasicMessage::ptr_t request,
                          BasicMessage::ptr_t response);

    void RespondToMessage(BasicMessage::ptr_t request,
                          const std::string response);

private:
    Channel::ptr_t m_channel;
    const std::string m_incoming_tag;




};

}

#ifdef _MSC_VER
# pragma warning ( pop )
#endif

#endif // SIMPLERPCSERVER_H
