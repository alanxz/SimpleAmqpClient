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

TEST_F(connected_test, publish_success) {
  BasicMessage::ptr_t message = BasicMessage::Create("message body");

  channel->BasicPublish("", "test_publish_rk", message);
}

TEST(test_publish, publish_large_message) {
  // Smallest frame size allowed by AMQP
  Channel::ptr_t channel = Channel::Create(connected_test::GetBrokerHost(),
                                           5672, "guest", "guest", "/", 4096);
  // Create a message with a body larger than a single frame
  BasicMessage::ptr_t message = BasicMessage::Create(std::string(4099, 'a'));

  channel->BasicPublish("", "test_publish_rk", message);
}

TEST_F(connected_test, publish_badexchange) {
  BasicMessage::ptr_t message = BasicMessage::Create("message body");

  EXPECT_THROW(channel->BasicPublish("test_publish_notexist", "test_publish_rk",
                                     message),
               ChannelException);
}

TEST_F(connected_test, publish_recover_from_error) {
  BasicMessage::ptr_t message = BasicMessage::Create("message body");

  EXPECT_THROW(channel->BasicPublish("test_publish_notexist", "test_publish_rk",
                                     message),
               ChannelException);
  channel->BasicPublish("", "test_publish_rk", message);
}

TEST_F(connected_test, publish_mandatory_fail) {
  BasicMessage::ptr_t message = BasicMessage::Create("message body");

  EXPECT_THROW(
      channel->BasicPublish("", "test_publish_notexist", message, true),
      MessageReturnedException);
}

TEST_F(connected_test, publish_mandatory_success) {
  BasicMessage::ptr_t message = BasicMessage::Create("message body");
  std::string queue = channel->DeclareQueue("");

  channel->BasicPublish("", queue, message, true);
}

TEST_F(connected_test, DISABLED_publish_immediate_fail1) {
  BasicMessage::ptr_t message = BasicMessage::Create("message body");

  // No queue connected
  EXPECT_THROW(
      channel->BasicPublish("", "test_publish_notexist", message, false, true),
      MessageReturnedException);
}

TEST_F(connected_test, DISABLED_publish_immediate_fail2) {
  BasicMessage::ptr_t message = BasicMessage::Create("message body");
  std::string queue = channel->DeclareQueue("");

  // No consumer connected
  EXPECT_THROW(channel->BasicPublish("", queue, message, false, true),
               MessageReturnedException);
}

TEST_F(connected_test, publish_immediate_success) {
  BasicMessage::ptr_t message = BasicMessage::Create("message body");
  std::string queue = channel->DeclareQueue("");
  std::string consumer = channel->BasicConsume(queue, "");

  channel->BasicPublish("", queue, message, true);
}
