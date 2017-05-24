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

#include <SimpleAmqpClient.h>

#include <iostream>
#include <stdlib.h>

using namespace AmqpClient;
int main()
{
    char *szBroker = getenv("AMQP_BROKER");
    Channel::ptr_t channel;
    if (szBroker != NULL)
        channel = Channel::Create(szBroker);
    else
        channel = Channel::Create();

    channel->DeclareQueue("alanqueue");
    channel->BindQueue("alanqueue", "amq.direct", "alankey");

    BasicMessage::ptr_t msg_in = BasicMessage::Create();

    msg_in->Body("This is a small message.");

    channel->BasicPublish("amq.direct", "alankey", msg_in);

    channel->BasicConsume("alanqueue", "consumertag");

    BasicMessage::ptr_t msg_out = channel->BasicConsumeMessage("consumertag")->Message();

    std::cout << "Message text: " << msg_out->Body() << std::endl;

}

