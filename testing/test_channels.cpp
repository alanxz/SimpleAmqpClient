#include <SimpleAmqpClient.h>
#include <gtest/gtest.h>

using namespace AmqpClient;

TEST(test_channels, first_channel)
{
  Channel::ptr_t channel = Channel::Create("localhost");

  channel->DeclareExchange("test_channel_exchange", Channel::EXCHANGE_TYPE_FANOUT, false, false, true);
  channel->DeleteExchange("test_channel_exchange");
}

// Check to see that channels are reused properly
TEST(test_channels, channel_reuse)
{
  Channel::ptr_t channel = Channel::Create("localhost");

  channel->DeclareExchange("test_channel_exchange1", Channel::EXCHANGE_TYPE_FANOUT, false, false, true);
  channel->DeclareExchange("test_channel_exchange2", Channel::EXCHANGE_TYPE_FANOUT, false, false, true);
  channel->DeleteExchange("test_channel_exchange1");
  channel->DeleteExchange("test_channel_exchange2");
}

// Check to see that a new channel is created when a channel is put in an exception state
TEST(test_channels, channel_recover_from_error)
{
  Channel::ptr_t channel = Channel::Create("localhost");

  EXPECT_THROW(channel->DeclareExchange("test_channel_exchangedoesnotexist", Channel::EXCHANGE_TYPE_FANOUT, true, false, true), AmqpResponseServerException);

  channel->DeclareExchange("test_channel_exchange", Channel::EXCHANGE_TYPE_FANOUT, false, false, true);
  channel->DeleteExchange("test_channel_exchange");
}

TEST(test_channels, publish_success1)
{
  Channel::ptr_t channel = Channel::Create("localhost");
  BasicMessage::ptr_t message = BasicMessage::Create("Test message");

  channel->BasicPublish("", "test_channel_routingkey", message, false, false);
}

TEST(test_channels, publish_success2)
{
  Channel::ptr_t channel = Channel::Create("localhost");
  BasicMessage::ptr_t message = BasicMessage::Create("Test message");

  channel->BasicPublish("", "test_channel_routingkey", message, false, false);
  channel->BasicPublish("", "test_channel_routingkey", message, false, false);
}

TEST(test_channels, publish_returned_mandatory)
{
  Channel::ptr_t channel = Channel::Create("localhost");
  BasicMessage::ptr_t message = BasicMessage::Create("Test message");

  EXPECT_THROW(channel->BasicPublish("", "test_channel_noqueue", message, true, false), MessageReturnedException);
}

TEST(test_channels, publish_returned_immediate)
{
  Channel::ptr_t channel = Channel::Create("localhost");
  BasicMessage::ptr_t message = BasicMessage::Create("Test message");
  std::string queue_name = channel->DeclareQueue("");

  EXPECT_THROW(channel->BasicPublish("", queue_name, message, false, true), MessageReturnedException);
}

TEST(test_channels, publish_bad_exchange)
{
  Channel::ptr_t channel = Channel::Create("localhost");
  BasicMessage::ptr_t message = BasicMessage::Create("Test message");

  EXPECT_THROW(channel->BasicPublish("test_channel_badexchange", "test_channel_rk", message, false, false), AmqpResponseServerException);
}

TEST(test_channels, publish_bad_exchange_recover)
{
  Channel::ptr_t channel = Channel::Create("localhost");
  BasicMessage::ptr_t message = BasicMessage::Create("Test message");

  EXPECT_THROW(channel->BasicPublish("test_channel_badexchange", "test_channel_rk", message, false, false), AmqpResponseServerException);

  channel->BasicPublish("", "test_channel_rk", message, false, false);
}

TEST(test_channels, consume_success)
{
  Channel::ptr_t channel = Channel::Create("localhost");
  BasicMessage::ptr_t message = BasicMessage::Create("Test message");
  std::string queue = channel->DeclareQueue("");
  channel->BasicPublish("", queue, message);

  std::string consumer = channel->BasicConsume(queue);

  Envelope::ptr_t consumed_envelope;
  channel->BasicConsumeMessage(consumer, consumed_envelope, 0);
}

TEST(test_channels, consume_success2)
{
  Channel::ptr_t channel = Channel::Create("localhost");
  BasicMessage::ptr_t message = BasicMessage::Create("Test message");
  std::string queue = channel->DeclareQueue("");

  std::string consumer = channel->BasicConsume(queue);
  channel->BasicPublish("", queue, message);

  Envelope::ptr_t consumed_envelope;
  channel->BasicConsumeMessage(consumer, consumed_envelope, 0);
}

TEST(test_channels, big_message)
{
  Channel::ptr_t channel = Channel::Create("localhost", 5672, "guest", "guest", "/", 4096);
  BasicMessage::ptr_t message = BasicMessage::Create(std::string(4099, 'a'));

  std::string queue = channel->DeclareQueue("");

  std::string consumer = channel->BasicConsume(queue);
  channel->BasicPublish("", queue, message);

  Envelope::ptr_t consumed_envelope;
  channel->BasicConsumeMessage(consumer, consumed_envelope, 0);
}