#include "SimpleAmqpClient.h"
#include <gtest/gtest.h>

using namespace AmqpClient;

TEST(connect_success, connect)
{
  Channel::ptr_t channel = Channel::Create("localhost");
}

TEST(connect_bad_host, connect)
{
  EXPECT_THROW(Channel::ptr_t channel = Channel::Create("HostDoesntExist"), std::runtime_error);
}

TEST(connect_bad_auth, connect)
{
  EXPECT_THROW(Channel::ptr_t channel = Channel::Create("localhost", 5672, "baduser", "badpass"), AmqpResponseServerException);
}

TEST(connect_bad_frame_size, connect)
{
  // AMQP Spec says we have a minimum frame size of 4096
  EXPECT_THROW(Channel::ptr_t channel = Channel::Create("localhost", 5672, "guest", "guest", "/", 400), AmqpResponseServerException);
}

TEST(connect_bad_vhost, connect)
{
  EXPECT_THROW(Channel::ptr_t channel = Channel::Create("localhost", 5672, "guest", "guest", "nonexitant_vhost"), AmqpResponseServerException);
}