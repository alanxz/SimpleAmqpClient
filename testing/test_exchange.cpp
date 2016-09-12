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

#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <gtest/gtest.h>

TEST_F(connected_test, declare_exchange_defaults) {
  channel->DeclareExchange("declare_defaults");
  channel->DeleteExchange("declare_defaults");
}

TEST_F(connected_test, declare_exchange_direct) {
  channel->DeclareExchange("declare_direct", Channel::EXCHANGE_TYPE_DIRECT);
  channel->DeleteExchange("declare_direct");
}

TEST_F(connected_test, declare_exchange_fanout) {
  channel->DeclareExchange("declare_fanout", Channel::EXCHANGE_TYPE_FANOUT);
  channel->DeleteExchange("declare_fanout");
}

TEST_F(connected_test, declare_exchange_topic) {
  channel->DeclareExchange("declare_topic", Channel::EXCHANGE_TYPE_TOPIC);
  channel->DeleteExchange("declare_topic");
}

TEST_F(connected_test, declare_exchange_passive_good) {
  channel->DeclareExchange("declare_passive", Channel::EXCHANGE_TYPE_DIRECT);
  channel->DeclareExchange("declare_passive", Channel::EXCHANGE_TYPE_DIRECT,
                           true);

  channel->DeleteExchange("declare_passive");
}

TEST_F(connected_test, declare_exchange_passive_notexist) {
  EXPECT_THROW(channel->DeclareExchange("declare_passive_notexist",
                                        Channel::EXCHANGE_TYPE_DIRECT, true),
               ChannelException);
}

TEST_F(connected_test, declare_exchange_typemismatch) {
  channel->DeclareExchange("declare_typemismatch",
                           Channel::EXCHANGE_TYPE_DIRECT);
  EXPECT_THROW(channel->DeclareExchange("declare_typemismatch",
                                        Channel::EXCHANGE_TYPE_FANOUT),
               ChannelException);

  channel->DeleteExchange("declare_typemismatch");
}

TEST_F(connected_test, declare_exchange_typemismatch2) {
  channel->DeclareExchange("declare_typemismatch",
                           Channel::EXCHANGE_TYPE_DIRECT);
  EXPECT_THROW(
      channel->DeclareExchange("declare_typemismatch",
                               Channel::EXCHANGE_TYPE_DIRECT, false, true),
      ChannelException);

  channel->DeleteExchange("declare_typemismatch");
}

TEST_F(connected_test, declare_exchange_durable) {
  channel->DeclareExchange("declare_durable", Channel::EXCHANGE_TYPE_DIRECT,
                           false, true);

  channel->DeleteExchange("declare_durable");
}

TEST_F(connected_test, declare_exchange_autodelete) {
  channel->DeclareExchange("declare_autodelete", Channel::EXCHANGE_TYPE_DIRECT,
                           false, false, true);

  channel->DeleteExchange("declare_autodelete");
}

TEST_F(connected_test, delete_exchange) {
  channel->DeclareExchange("delete_exchange");
  channel->DeleteExchange("delete_exchange");
}

TEST_F(connected_test, delete_exchange_notexist) {
  EXPECT_THROW(channel->DeleteExchange("exchange_notexist"), ChannelException);
}

TEST_F(connected_test, delete_exhange_ifunused) {
  channel->DeclareExchange("exchange_used", Channel::EXCHANGE_TYPE_DIRECT);

  channel->DeleteExchange("exchange_used", true);
}

TEST_F(connected_test, delete_exhange_ifused) {
  channel->DeclareExchange("exchange_used", Channel::EXCHANGE_TYPE_DIRECT);
  std::string queue = channel->DeclareQueue("");
  channel->BindQueue(queue, "exchange_used", "whatever");

  EXPECT_THROW(channel->DeleteExchange("exchange_used", true),
               ChannelException);

  channel->DeleteQueue(queue);
  channel->DeleteExchange("exchange_used");
}

TEST_F(connected_test, bind_exchange) {
  channel->DeclareExchange("exchange_bind_dest");
  channel->DeclareExchange("exchange_bind_src");

  channel->BindExchange("exchange_bind_dest", "exchange_bind_src", "rk");

  channel->DeleteExchange("exchange_bind_dest");
  channel->DeleteExchange("exchange_bind_src");
}

TEST_F(connected_test, bind_exchange_badexchange) {
  channel->DeclareExchange("exchange_bind_dest");

  EXPECT_THROW(channel->BindExchange("exchange_bind_dest",
                                     "exchange_bind_notexist", "rk"),
               ChannelException);

  channel->DeleteExchange("exchange_bind_dest");
}

TEST_F(connected_test, unbind_exchange) {
  channel->DeclareExchange("exchange_bind_dest");
  channel->DeclareExchange("exchange_bind_src");

  channel->BindExchange("exchange_bind_dest", "exchange_bind_src", "rk");
  channel->UnbindExchange("exchange_bind_dest", "exchange_bind_src", "rk");

  channel->DeleteExchange("exchange_bind_dest");
  channel->DeleteExchange("exchange_bind_src");
}

TEST_F(connected_test, unbind_exchange_badbinding) {
  EXPECT_THROW(channel->UnbindExchange("exchange_notexist", "exchange_notexist",
                                       "notexist"),
               ChannelException);
}
