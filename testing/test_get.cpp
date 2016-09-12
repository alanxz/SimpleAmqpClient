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

#include "connected_test.h"

using namespace AmqpClient;

TEST_F(connected_test, get_ok) {
  BasicMessage::ptr_t message = BasicMessage::Create("Message Body");
  std::string queue = channel->DeclareQueue("");
  channel->BasicPublish("", queue, message, true);

  Envelope::ptr_t new_message;
  EXPECT_TRUE(channel->BasicGet(new_message, queue));
  EXPECT_EQ(message->Body(), new_message->Message()->Body());
}

TEST_F(connected_test, get_empty) {
  BasicMessage::ptr_t message = BasicMessage::Create("Message Body");
  std::string queue = channel->DeclareQueue("");

  Envelope::ptr_t new_message;
  EXPECT_FALSE(channel->BasicGet(new_message, queue));
}

TEST(test_get, get_big) {
  // Smallest frame size allowed by AMQP
  Channel::ptr_t channel = Channel::Create(connected_test::GetBrokerHost(),
                                           5672, "guest", "guest", "/", 4096);
  // Create a message with a body larger than a single frame
  BasicMessage::ptr_t message = BasicMessage::Create(std::string(4099, 'a'));
  std::string queue = channel->DeclareQueue("");

  channel->BasicPublish("", queue, message);
  Envelope::ptr_t new_message;
  EXPECT_TRUE(channel->BasicGet(new_message, queue));
  EXPECT_EQ(message->Body(), new_message->Message()->Body());
}

TEST_F(connected_test, bad_queue) {
  Envelope::ptr_t new_message;
  EXPECT_THROW(channel->BasicGet(new_message, "test_get_nonexistantqueue"),
               ChannelException);
}

TEST_F(connected_test, ack_message) {
  BasicMessage::ptr_t message = BasicMessage::Create("Message Body");
  std::string queue = channel->DeclareQueue("");
  channel->BasicPublish("", queue, message, true);

  Envelope::ptr_t new_message;
  EXPECT_TRUE(channel->BasicGet(new_message, queue, false));
  channel->BasicAck(new_message);
  EXPECT_FALSE(channel->BasicGet(new_message, queue, false));
}
