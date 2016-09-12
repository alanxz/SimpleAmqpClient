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
#include "connected_test.h"

using namespace AmqpClient;

TEST_F(connected_test, basic_consume) {
  std::string queue = channel->DeclareQueue("");
  std::string consumer = channel->BasicConsume(queue);
}

TEST_F(connected_test, basic_consume_badqueue) {
  EXPECT_THROW(channel->BasicConsume("test_consume_noexistqueue"),
               ChannelException);
}

TEST_F(connected_test, basic_consume_duplicatetag) {
  std::string queue = channel->DeclareQueue("");
  std::string consumer = channel->BasicConsume(queue);
  EXPECT_THROW(channel->BasicConsume(queue, consumer), ChannelException);
}

TEST_F(connected_test, basic_cancel_consumer) {
  std::string queue = channel->DeclareQueue("");
  std::string consumer = channel->BasicConsume(queue);
  channel->BasicCancel(consumer);
}

TEST_F(connected_test, basic_cancel_bad_consumer) {
  EXPECT_THROW(channel->BasicCancel("test_consume_noexistconsumer"),
               ConsumerTagNotFoundException);
}

TEST_F(connected_test, basic_cancel_cancelled_consumer) {
  std::string queue = channel->DeclareQueue("");
  std::string consumer = channel->BasicConsume(queue);
  channel->BasicCancel(consumer);
  EXPECT_THROW(channel->BasicCancel(consumer), ConsumerTagNotFoundException);
}

TEST_F(connected_test, basic_consume_message) {
  BasicMessage::ptr_t message = BasicMessage::Create("Message Body");
  std::string queue = channel->DeclareQueue("");
  std::string consumer = channel->BasicConsume(queue);
  channel->BasicPublish("", queue, message);

  Envelope::ptr_t delivered;
  EXPECT_TRUE(channel->BasicConsumeMessage(consumer, delivered, -1));
  EXPECT_EQ(consumer, delivered->ConsumerTag());
  EXPECT_EQ("", delivered->Exchange());
  EXPECT_EQ(queue, delivered->RoutingKey());
  EXPECT_EQ(message->Body(), delivered->Message()->Body());
}

TEST_F(connected_test, basic_consume_message_bad_consumer) {
  EXPECT_THROW(channel->BasicConsumeMessage("test_consume_noexistconsumer"),
               ConsumerTagNotFoundException);
}

TEST_F(connected_test, basic_consume_inital_qos) {
  BasicMessage::ptr_t message1 = BasicMessage::Create("Message1");
  BasicMessage::ptr_t message2 = BasicMessage::Create("Message2");
  BasicMessage::ptr_t message3 = BasicMessage::Create("Message3");

  std::string queue = channel->DeclareQueue("");
  channel->BasicPublish("", queue, message1, true);
  channel->BasicPublish("", queue, message2, true);
  channel->BasicPublish("", queue, message3, true);

  std::string consumer = channel->BasicConsume(queue, "", true, false);
  Envelope::ptr_t received1, received2;
  ASSERT_TRUE(channel->BasicConsumeMessage(consumer, received1, 100));

  EXPECT_FALSE(channel->BasicConsumeMessage(consumer, received2, 100));
  channel->BasicAck(received1);

  EXPECT_TRUE(channel->BasicConsumeMessage(consumer, received2, 100));
}

TEST_F(connected_test, basic_consume_2consumers) {
  BasicMessage::ptr_t message1 = BasicMessage::Create("Message1");
  BasicMessage::ptr_t message2 = BasicMessage::Create("Message2");
  BasicMessage::ptr_t message3 = BasicMessage::Create("Message3");

  std::string queue1 = channel->DeclareQueue("");
  std::string queue2 = channel->DeclareQueue("");
  std::string queue3 = channel->DeclareQueue("");

  channel->BasicPublish("", queue1, message1);
  channel->BasicPublish("", queue2, message2);
  channel->BasicPublish("", queue3, message3);

  std::string consumer1 = channel->BasicConsume(queue1, "", true, false);
  std::string consumer2 = channel->BasicConsume(queue2, "", true, false);

  Envelope::ptr_t envelope1;
  Envelope::ptr_t envelope2;
  Envelope::ptr_t envelope3;

  channel->BasicConsumeMessage(consumer1, envelope1);
  channel->BasicAck(envelope1);
  channel->BasicConsumeMessage(consumer2, envelope2);
  channel->BasicAck(envelope2);
  channel->BasicGet(envelope3, queue3);
  channel->BasicAck(envelope3);
}

TEST_F(connected_test, basic_consume_1000messages) {
  BasicMessage::ptr_t message1 = BasicMessage::Create("Message1");

  std::string queue = channel->DeclareQueue("");
  std::string consumer = channel->BasicConsume(queue, "");

  Envelope::ptr_t msg;
  for (int i = 0; i < 1000; ++i) {
    message1->Timestamp(i);
    channel->BasicPublish("", queue, message1, true);
    channel->BasicConsumeMessage(consumer, msg);
  }
}

TEST_F(connected_test, basic_recover) {
  BasicMessage::ptr_t message = BasicMessage::Create("Message1");

  std::string queue = channel->DeclareQueue("");
  std::string consumer = channel->BasicConsume(queue, "", true, false);
  channel->BasicPublish("", queue, message);

  Envelope::ptr_t message1;
  Envelope::ptr_t message2;

  EXPECT_TRUE(channel->BasicConsumeMessage(consumer, message1));
  channel->BasicRecover(consumer);
  EXPECT_TRUE(channel->BasicConsumeMessage(consumer, message2));

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, basic_recover_badconsumer) {
  EXPECT_THROW(channel->BasicRecover("consumer_notexist"),
               ConsumerTagNotFoundException);
}

TEST_F(connected_test, basic_qos) {
  std::string queue = channel->DeclareQueue("");
  std::string consumer = channel->BasicConsume(queue, "", true, false);
  channel->BasicPublish("", queue, BasicMessage::Create("Message1"));
  channel->BasicPublish("", queue, BasicMessage::Create("Message2"));

  Envelope::ptr_t incoming;
  EXPECT_TRUE(channel->BasicConsumeMessage(consumer, incoming, 100));
  EXPECT_FALSE(channel->BasicConsumeMessage(consumer, incoming, 100));

  channel->BasicQos(consumer, 2);
  EXPECT_TRUE(channel->BasicConsumeMessage(consumer, incoming, 100));

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, basic_qos_badconsumer) {
  EXPECT_THROW(channel->BasicQos("consumer_notexist", 1),
               ConsumerTagNotFoundException);
}

TEST_F(connected_test, consumer_cancelled) {
  std::string queue = channel->DeclareQueue("");
  std::string consumer = channel->BasicConsume(queue, "", true, false);
  channel->DeleteQueue(queue);

  EXPECT_THROW(channel->BasicConsumeMessage(consumer),
               ConsumerCancelledException);
}

TEST_F(connected_test, consumer_cancelled_one_message) {
  std::string queue = channel->DeclareQueue("");
  std::string consumer = channel->BasicConsume(queue, "", true, false);

  channel->BasicPublish("", queue, BasicMessage::Create("Message"));
  channel->BasicConsumeMessage(consumer);

  channel->DeleteQueue(queue);

  EXPECT_THROW(channel->BasicConsumeMessage(consumer),
               ConsumerCancelledException);
}

TEST_F(connected_test, consume_multiple) {
  std::string queue1 = channel->DeclareQueue("");
  std::string queue2 = channel->DeclareQueue("");

  std::string Body = "Message 1";
  channel->BasicPublish("", queue1, BasicMessage::Create(Body));

  channel->BasicConsume(queue1);
  channel->BasicConsume(queue2);

  Envelope::ptr_t env = channel->BasicConsumeMessage();

  EXPECT_EQ(Body, env->Message()->Body());
}
