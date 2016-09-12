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

#include <boost/array.hpp>

#include <algorithm>
#include <iostream>

using namespace AmqpClient;

TEST(basic_message, empty_message) {
  BasicMessage::ptr_t empty_message = BasicMessage::Create();

  EXPECT_EQ(std::string(), empty_message->Body());

  // Allow the message to destruct
}

TEST(basic_message, empty_message_add_body) {
  BasicMessage::ptr_t empty_message = BasicMessage::Create();

  EXPECT_EQ(std::string(), empty_message->Body());

  const std::string body("Message Body");
  empty_message->Body(body);

  EXPECT_EQ(body, empty_message->Body());

  // Allow the message to destruct
}

TEST(basic_message, empty_message_add_body2) {
  BasicMessage::ptr_t empty_message = BasicMessage::Create();

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
  BasicMessage::ptr_t message = BasicMessage::Create(first_body);

  EXPECT_EQ(first_body, message->Body());

  const std::string second_body("Second message Body");
  message->Body(second_body);

  EXPECT_EQ(second_body, message->Body());

  // Allow the message to destruct
}

TEST(basic_message, initial_message_replace2) {
  const std::string first_body("First message body");
  BasicMessage::ptr_t message = BasicMessage::Create(first_body);
  EXPECT_EQ(first_body, message->Body());

  const std::string second_body("second message body");
  message->Body(second_body);
  EXPECT_EQ(second_body, message->Body());

  const std::string third_body("3rd Body");
  message->Body(third_body);
  EXPECT_EQ(third_body, message->Body());
}

TEST(basic_message, embedded_nulls) {
  const boost::array<char, 7> message_data = {
      {'a', 'b', 'c', 0, '1', '2', '3'}};
  const std::string body(message_data.data(), message_data.size());
  BasicMessage::ptr_t message = BasicMessage::Create(body);
  EXPECT_EQ(body, message->Body());

  amqp_bytes_t amqp_body = message->getAmqpBody();
  EXPECT_EQ(body.length(), amqp_body.len);
  EXPECT_TRUE(std::equal(message_data.begin(), message_data.end(),
                         reinterpret_cast<char *>(amqp_body.bytes)));

  const boost::array<char, 7> message_data2 = {
      {'1', '2', '3', 0, 'a', 'b', 'c'}};
  const std::string body2(message_data2.data(), message_data2.size());
  message->Body(body2);
  EXPECT_EQ(body2, message->Body());

  amqp_bytes_t amqp_body2 = message->getAmqpBody();
  EXPECT_EQ(body2.length(), amqp_body2.len);
  EXPECT_TRUE(std::equal(message_data2.begin(), message_data2.end(),
                         reinterpret_cast<char *>(amqp_body2.bytes)));
}

TEST_F(connected_test, replaced_received_body) {
  const std::string queue = channel->DeclareQueue("");
  const std::string consumer = channel->BasicConsume(queue);

  const std::string body("First Message Body");
  BasicMessage::ptr_t out_message = BasicMessage::Create(body);
  channel->BasicPublish("", queue, out_message);

  Envelope::ptr_t envelope = channel->BasicConsumeMessage(consumer);
  BasicMessage::ptr_t in_message = envelope->Message();
  EXPECT_EQ(out_message->Body(), in_message->Body());

  const std::string body2("Second message body");
  in_message->Body(body2);
  EXPECT_EQ(body2, in_message->Body());
}
