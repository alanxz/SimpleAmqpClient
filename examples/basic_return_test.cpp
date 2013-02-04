/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MIT
 *
 * Portions created by VMware are Copyright (c) 2007-2012 VMware, Inc.
 * All Rights Reserved.
 *
 * Portions created by Tony Garnock-Jones are Copyright (c) 2009-2010
 * VMware, Inc. and Tony Garnock-Jones. All Rights Reserved.
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

using namespace AmqpClient;

int main()
{
  char* szBroker = getenv("AMQP_BROKER");
  Channel::ptr_t channel;
  if (szBroker != NULL)
    channel = Channel::Create(szBroker);
  else
    channel = Channel::Create();

  BasicMessage::ptr_t the_message = BasicMessage::Create("Body Content");
  try {  
  channel->DeclareQueue("BasicReturnTestQueue");
  channel->BasicConsume("BasicReturnTestQueue", "consumer_tag1");

  channel->BasicPublish("", "BasicReturnTestQueue", the_message, true, false);
  channel->BasicPublish("", "ThisDoesntExist", the_message, true, false);
  

    for (int i = 0; i < 2; ++i)
    {
      Envelope::ptr_t env;
      if (channel->BasicConsumeMessage("consumer_tag1", env, 0))
      {
        std::cout << "Received message with body: " << env->Message()->Body() << std::endl;
      }
    }
  }
  catch (MessageReturnedException& e)
  {
    std::cout << "Message got returned: " << e.what();
    std::cout << "\nMessage body: " << e.message()->Body();
  }

}
