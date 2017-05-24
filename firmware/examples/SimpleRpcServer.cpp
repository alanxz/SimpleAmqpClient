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

#include "SimpleRpcServer.h"


namespace AmqpClient
{

SimpleRpcServer::SimpleRpcServer(Channel::ptr_t channel, const std::string &rpc_name) :
    m_channel(channel)
    , m_incoming_tag(m_channel->DeclareQueue(rpc_name))

{
    m_channel->BindQueue(m_incoming_tag, "amq.direct", m_incoming_tag);
    m_channel->BasicConsume(m_incoming_tag, m_incoming_tag);
}

SimpleRpcServer::~SimpleRpcServer()
{
}

BasicMessage::ptr_t SimpleRpcServer::GetNextIncomingMessage()
{
    return m_channel->BasicConsumeMessage(m_incoming_tag)->Message();
}

bool SimpleRpcServer::GetNextIncomingMessage(BasicMessage::ptr_t &message, int timeout)
{
    Envelope::ptr_t env;
    if (m_channel->BasicConsumeMessage(m_incoming_tag, env, timeout))
    {
        message = env->Message();
        return true;
    }
    else
    {
        return false;
    }
}

void SimpleRpcServer::RespondToMessage(BasicMessage::ptr_t request, BasicMessage::ptr_t response)
{
    if (request->CorrelationIdIsSet() && !response->CorrelationIdIsSet())
    {
        response->CorrelationId(request->CorrelationId());
    }

    m_channel->BasicPublish("amq.direct", request->ReplyTo(), response);
}

void SimpleRpcServer::RespondToMessage(BasicMessage::ptr_t request, const std::string response)
{
    BasicMessage::ptr_t response_message = BasicMessage::Create();
    response_message->Body(response);

    RespondToMessage(request, response_message);
}

} // namespace AmqpClient
