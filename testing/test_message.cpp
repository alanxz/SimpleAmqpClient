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

#include <amqp.h>

#include <algorithm>
#include <array>
#include <iostream>

using namespace AmqpClient;

TEST(basic_message, empty_message) {
  std::shared_ptr<BasicMessage> empty_message = BasicMessage::Create();

  EXPECT_EQ(std::string(), empty_message->Body());

  // Allow the message to destruct
}

TEST(basic_message, empty_message_add_body) {
  std::shared_ptr<BasicMessage> empty_message = BasicMessage::Create();

  EXPECT_EQ(std::string(), empty_message->Body());

  const std::string body("Message Body");
  empty_message->Body(body);

  EXPECT_EQ(body, empty_message->Body());

  // Allow the message to destruct
}

TEST(basic_message, empty_message_add_body2) {
  std::shared_ptr<BasicMessage> empty_message = BasicMessage::Create();

  EXPECT_EQ(std::string(), empty_message->Body());

  const std::string body("Message Body");
  empty_message->Body(body);

  EXPECT_EQ(body, empty_message->Body());

  const std::string body2("Second body");
  empty_message->Body(body2);
  EXPECT_EQ(body2, empty_message->Body());

  // Allow the message to destruct
}

TEST(basic_message, initial_message_replace) {
  const std::string first_body("First message Body");
  std::shared_ptr<BasicMessage> message = BasicMessage::Create(first_body);

  EXPECT_EQ(first_body, message->Body());

  const std::string second_body("Second message Body");
  message->Body(second_body);

  EXPECT_EQ(second_body, message->Body());

  // Allow the message to destruct
}

TEST(basic_message, initial_message_replace2) {
  const std::string first_body("First message body");
  std::shared_ptr<BasicMessage> message = BasicMessage::Create(first_body);
  EXPECT_EQ(first_body, message->Body());

  const std::string second_body("second message body");
  message->Body(second_body);
  EXPECT_EQ(second_body, message->Body());

  const std::string third_body("3rd Body");
  message->Body(third_body);
  EXPECT_EQ(third_body, message->Body());
}

TEST_F(connected_test, replaced_received_body) {
  const std::string queue = channel->DeclareQueue("");
  const std::string consumer = channel->BasicConsume(queue);

  const std::string body("First Message Body");
  std::shared_ptr<BasicMessage> out_message = BasicMessage::Create(body);
  channel->BasicPublish("", queue, out_message);

  std::shared_ptr<Envelope> envelope = channel->BasicConsumeMessage(consumer);
  std::shared_ptr<BasicMessage> in_message = envelope->Message();
  EXPECT_EQ(out_message->Body(), in_message->Body());

  const std::string body2("Second message body");
  in_message->Body(body2);
  EXPECT_EQ(body2, in_message->Body());
}
