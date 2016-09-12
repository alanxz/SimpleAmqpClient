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

#include <gtest/gtest.h>
#include "SimpleAmqpClient/SimpleAmqpClient.h"

#include "connected_test.h"

using namespace AmqpClient;

TEST(connecting_test, connect_default) {
  Channel::ptr_t channel = Channel::Create(connected_test::GetBrokerHost());
}

TEST(connecting_test, connect_badhost) {
  EXPECT_THROW(Channel::ptr_t channel = Channel::Create("HostDoesntExist"),
               std::runtime_error);
}

TEST(connecting_test, connect_badauth) {
  EXPECT_THROW(Channel::ptr_t channel = Channel::Create(
                   connected_test::GetBrokerHost(), 5672, "baduser", "badpass"),
               AccessRefusedException);
}

TEST(connecting_test, connect_badframesize) {
  // AMQP Spec says we have a minimum frame size of 4096
  EXPECT_THROW(
      Channel::ptr_t channel = Channel::Create(
          connected_test::GetBrokerHost(), 5672, "guest", "guest", "/", 400),
      AmqpResponseLibraryException);
}

TEST(connecting_test, connect_badvhost) {
  EXPECT_THROW(Channel::ptr_t channel =
                   Channel::Create(connected_test::GetBrokerHost(), 5672,
                                   "guest", "guest", "nonexitant_vhost"),
               NotAllowedException);
}

TEST(connecting_test, connect_using_uri) {
  std::string host_uri = "amqp://" + connected_test::GetBrokerHost();
  Channel::ptr_t channel = Channel::CreateFromUri(host_uri);
}
