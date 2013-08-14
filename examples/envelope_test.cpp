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

#include <iostream>

#include "SimpleAmqpClient.h"

using namespace AmqpClient;
int main()
{
  const std::string EXCHANGE_NAME = "SimpleAmqpClientEnvelopeTest";
  const std::string ROUTING_KEY = "SACRoutingKey";
  const std::string CONSUMER_TAG = "SACConsumerTag";

  try
  {
    Channel::ptr_t channel = Channel::Create();

    channel->DeclareExchange(EXCHANGE_NAME, Channel::EXCHANGE_TYPE_FANOUT);
    std::string queue = channel->DeclareQueue("");
    channel->BindQueue(queue, EXCHANGE_NAME, ROUTING_KEY);

    try
    {
      channel->BasicPublish(EXCHANGE_NAME, ROUTING_KEY, BasicMessage::Create("MessageBody"));
      channel->BasicPublish(EXCHANGE_NAME, ROUTING_KEY, BasicMessage::Create("MessageBody2"));
      channel->BasicPublish(EXCHANGE_NAME, ROUTING_KEY, BasicMessage::Create("MessageBody3"));
    }
    catch (MessageReturnedException& e)
    {
      std::cout << "Message got returned: " << e.what();
      std::cout << "\nMessage body: " << e.message()->Body();
      return -1;
    }

    channel->BasicConsume(queue, CONSUMER_TAG);

    Envelope::ptr_t env;
    for (int i = 0; i < 3; ++i)
    {
      if (channel->BasicConsumeMessage(CONSUMER_TAG, env, 0)) 
      {
        std::cout << "Envelope received: \n" 
          << " Exchange: " << env->Exchange()
          << "\n Routing key: " << env->RoutingKey() 
          << "\n Consumer tag: " << env->ConsumerTag()
          << "\n Delivery tag: " << env->DeliveryTag()
          << "\n Redelivered: " << env->Redelivered() 
          << "\n Body: " << env->Message()->Body() << std::endl;
      }
      else
      {
        std::cout << "Basic Consume failed.\n";
      }
    }
  }
  catch (AmqpException& e)
  {
    std::cout << "Failure: " << e.what();
  }
  return 0;
}
