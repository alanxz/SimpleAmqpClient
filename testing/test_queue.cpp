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

TEST_F(connected_test, queue_declare) {
  std::string queue = channel->DeclareQueue("");
  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_declare_named) {
  std::string queue = channel->DeclareQueue("declare_queue_test");
  EXPECT_EQ("declare_queue_test", queue);

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_declare_passive) {
  std::string queue = channel->DeclareQueue("declare_queue_passive");
  channel->DeclareQueue("declare_queue_passive", true);

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_declare_passive_fail) {
  EXPECT_THROW(channel->DeclareQueue("declare_queue_notexist", true),
               ChannelException);
}

TEST_F(connected_test, queue_declare_durable) {
  std::string queue =
      channel->DeclareQueue("declare_queue_durable", false, true, false);

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_declare_notexclusive) {
  std::string queue =
      channel->DeclareQueue("declare_queue_notexclusive", false, false, false);

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_declare_notautodelete) {
  std::string queue = channel->DeclareQueue("declare_queue_notautodelete",
                                            false, false, false, false);

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_declare_counts) {
  boost::uint32_t message_count = 123;
  boost::uint32_t consumer_count = 123;

  std::string queue = channel->DeclareQueueWithCounts(
      "queue_declare_counts", message_count, consumer_count);

  EXPECT_NE("", queue);
  EXPECT_EQ(0, message_count);
  EXPECT_EQ(0, consumer_count);

  const std::string body("Test Message");
  BasicMessage::ptr_t out_message = BasicMessage::Create(body);
  channel->BasicPublish("", queue, out_message);
  channel->BasicPublish("", queue, out_message);
  channel->BasicPublish("", queue, out_message);

  std::string queue2 = channel->DeclareQueueWithCounts(
      "queue_declare_counts", message_count, consumer_count);

  EXPECT_NE("", queue2);
  EXPECT_EQ(3, message_count);
  EXPECT_EQ(0, consumer_count);

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_declare_counts_table) {
  boost::uint32_t message_count = 123;
  boost::uint32_t consumer_count = 123;

  Table qTable;

  qTable.insert(TableEntry(TableKey("IsATest"), TableValue(true)));

  std::string queue = channel->DeclareQueueWithCounts(
      "queue_declare_counts_table", message_count, consumer_count, false, false,
      true, true, qTable);

  EXPECT_NE("", queue);
  EXPECT_EQ(0, message_count);
  EXPECT_EQ(0, consumer_count);

  const std::string body("Test Message");
  BasicMessage::ptr_t out_message = BasicMessage::Create(body);
  channel->BasicPublish("", queue, out_message);
  channel->BasicPublish("", queue, out_message);
  channel->BasicPublish("", queue, out_message);

  std::string queue2 = channel->DeclareQueueWithCounts(
      "queue_declare_counts_table", message_count, consumer_count, false, false,
      true, true, qTable);

  EXPECT_NE("", queue2);
  EXPECT_EQ(3, message_count);
  EXPECT_EQ(0, consumer_count);

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_delete) {
  std::string queue = channel->DeclareQueue("delete_queue");
  channel->DeleteQueue(queue);
  EXPECT_THROW(channel->DeclareQueue(queue, true), ChannelException);
}

TEST_F(connected_test, queue_delete_badqueue) {
  EXPECT_THROW(channel->DeleteQueue("delete_queue_notexist"), ChannelException);
}

TEST_F(connected_test, queue_delete_ifunused) {
  std::string queue = channel->DeclareQueue("delete_queue_ifunused");
  channel->DeleteQueue(queue, true);
  EXPECT_THROW(channel->DeclareQueue(queue, true), ChannelException);
}

TEST_F(connected_test, queue_delete_ifused) {
  std::string queue = channel->DeclareQueue("delete_queue_ifused");
  channel->BasicConsume(queue);
  EXPECT_THROW(channel->DeleteQueue(queue, true), ChannelException);

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_delete_ifempty) {
  std::string queue = channel->DeclareQueue("delete_queue_ifempty");
  channel->DeleteQueue(queue, false, true);

  EXPECT_THROW(channel->DeclareQueue(queue, true), ChannelException);
}

TEST_F(connected_test, queue_delete_ifnotempty) {
  std::string queue = channel->DeclareQueue("delete_queue_ifnotempty");
  BasicMessage::ptr_t message = BasicMessage::Create("Message body");
  channel->BasicPublish("", queue, message, true);

  EXPECT_THROW(channel->DeleteQueue(queue, false, true), ChannelException);

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_bind) {
  channel->DeclareExchange("queue_bind_exchange");
  std::string queue = channel->DeclareQueue("queue_bind_queue");

  channel->BindQueue(queue, "queue_bind_exchange", "rk");

  channel->DeleteExchange("queue_bind_exchange");
  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_bind_badexchange) {
  std::string queue = channel->DeclareQueue("queue_bind_badexchange");

  EXPECT_THROW(channel->BindQueue(queue, "queue_bind_exchangenotexist", "rk"),
               ChannelException);

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_bind_badqueue) {
  channel->DeclareExchange("queue_bind_badqueue");

  EXPECT_THROW(channel->BindQueue("queue_bind_queuenotexist",
                                  "queue_bind_badqueue", "rk"),
               ChannelException);

  channel->DeleteExchange("queue_bind_badqueue");
}

TEST_F(connected_test, queue_bind_nokey) {
  channel->DeclareExchange("queue_bind_exchange");
  std::string queue = channel->DeclareQueue("queue_bind_queue");

  channel->BindQueue(queue, "queue_bind_exchange");

  channel->DeleteExchange("queue_bind_exchange");
  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_unbind) {
  channel->DeclareExchange("queue_unbind_exchange");
  std::string queue = channel->DeclareQueue("queue_unbind_queue");
  channel->BindQueue(queue, "queue_unbind_exchange", "rk");

  channel->UnbindQueue(queue, "queue_unbind_exchange", "rk");

  channel->DeleteExchange("queue_unbind_exchange");
  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_unbind_badbinding) {
  EXPECT_THROW(channel->UnbindQueue("queue_unbind_queuenotexist",
                                    "queue_unbind_exchangenotexist", "rk"),
               ChannelException);
}

TEST_F(connected_test, queue_purge) {
  std::string queue = channel->DeclareQueue("queue_purge");
  BasicMessage::ptr_t message = BasicMessage::Create("Message Body");
  channel->BasicPublish("", queue, message, true);

  channel->PurgeQueue(queue);
  Envelope::ptr_t envelope;
  EXPECT_FALSE(channel->BasicGet(envelope, queue));

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_purge_badqueue) {
  EXPECT_THROW(channel->PurgeQueue("purge_queue_queuenotexist"),
               ChannelException);
}
