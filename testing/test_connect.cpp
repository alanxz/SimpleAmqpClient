#include "SimpleAmqpClient/SimpleAmqpClient.h"
#include <gtest/gtest.h>

#include "connected_test.h"

using namespace AmqpClient;

TEST(connecting_test, connect_default)
{
  Channel::ptr_t channel = Channel::Create(connected_test::GetBrokerHost());
}

TEST(connecting_test, connect_badhost)
{
  EXPECT_THROW(Channel::ptr_t channel = Channel::Create("HostDoesntExist"), std::runtime_error);
}

TEST(connecting_test, connect_badauth)
{
  EXPECT_THROW(Channel::ptr_t channel = Channel::Create(connected_test::GetBrokerHost(), 5672, "baduser", "badpass"), AmqpResponseLibraryException);
}

TEST(connecting_test, connect_badframesize)
{
  // AMQP Spec says we have a minimum frame size of 4096
  EXPECT_THROW(Channel::ptr_t channel = Channel::Create(connected_test::GetBrokerHost(), 5672, "guest", "guest", "/", 400), AmqpResponseLibraryException);
}

TEST(connecting_test, connect_badvhost)
{
  EXPECT_THROW(Channel::ptr_t channel = Channel::Create(connected_test::GetBrokerHost(), 5672, "guest", "guest", "nonexitant_vhost"), AmqpResponseLibraryException);
}
