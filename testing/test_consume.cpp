#include <SimpleAmqpClient.h>
#include <gtest/gtest.h>

using namespace AmqpClient;
TEST(test_consume, create_consumer)
{
  Channel::ptr_t channel = Channel::Create();
  std::string queue = channel->DeclareQueue("");
  std::string consumer = channel->BasicConsume(queue);
}

TEST(test_consume, bad_queue)
{
  Channel::ptr_t channel = Channel::Create();
  EXPECT_THROW(channel->BasicConsume("test_consume_noexistqueue"), AmqpResponseServerException);
}

TEST(test_consume, duplicate_consumer_tag)
{
  Channel::ptr_t channel = Channel::Create();
  std::string queue = channel->DeclareQueue("");
  std::string consumer = channel->BasicConsume(queue);
  EXPECT_THROW(channel->BasicConsume(queue, consumer), AmqpResponseServerException);
}

TEST(test_consume, cancel_consumer)
{
  Channel::ptr_t channel = Channel::Create();
  std::string queue = channel->DeclareQueue("");
  std::string consumer = channel->BasicConsume(queue);
  channel->BasicCancel(consumer);
}

TEST(test_consume, cancel_bad_consumer)
{
  Channel::ptr_t channel = Channel::Create();
  EXPECT_THROW(channel->BasicCancel("test_consume_noexistconsumer"), ConsumerTagNotFoundException);
}

TEST(test_consume, cancel_cancelled_consumer)
{
  Channel::ptr_t channel = Channel::Create();
  std::string queue = channel->DeclareQueue("");
  std::string consumer = channel->BasicConsume(queue);
  channel->BasicCancel(consumer);
  EXPECT_THROW(channel->BasicCancel(consumer), ConsumerTagNotFoundException);
}

TEST(test_consume, consume_message)
{
  Channel::ptr_t channel = Channel::Create();
  BasicMessage::ptr_t message = BasicMessage::Create("Message Body");
  std::string queue = channel->DeclareQueue("");
  std::string consumer = channel->BasicConsume(queue);
  channel->BasicPublish("", queue, message);

  Envelope::ptr_t delivered;
  EXPECT_TRUE(channel->BasicConsumeMessage(consumer, delivered, 0));
  EXPECT_EQ(consumer, delivered->ConsumerTag());
  EXPECT_EQ("", delivered->Exchange());
  EXPECT_EQ(queue, delivered->RoutingKey());
  EXPECT_EQ(message->Body(), delivered->Message()->Body());
}

TEST(test_consume, consume_bad_consumer)
{
  Channel::ptr_t channel = Channel::Create();
  EXPECT_THROW(channel->BasicConsumeMessage("test_consume_noexistconsumer"), ConsumerTagNotFoundException);
}