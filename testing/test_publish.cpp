#include <SimpleAmqpClient.h>
#include <gtest/gtest.h>

using namespace AmqpClient;

TEST(test_publish, publish_success)
{
  Channel::ptr_t channel = Channel::Create();
  BasicMessage::ptr_t message = BasicMessage::Create("message body");

  channel->BasicPublish("", "test_publish_rk", message);
}

TEST(test_publish, publish_large_message)
{
  // Smallest frame size allowed by AMQP
  Channel::ptr_t channel = Channel::Create("127.0.0.1", 5672, "guest", "guest", "/", 4096);
  // Create a message with a body larger than a single frame
  BasicMessage::ptr_t message = BasicMessage::Create(std::string(4099, 'a'));

  channel->BasicPublish("", "test_publish_rk", message);
}

TEST(test_publish, publish_badexchange)
{
  Channel::ptr_t channel = Channel::Create();
  BasicMessage::ptr_t message = BasicMessage::Create("message body");
  
  EXPECT_THROW(channel->BasicPublish("test_publish_notexist", "test_publish_rk", message), AmqpResponseServerException);
}

TEST(test_publish, publish_recover_from_error)
{
  Channel::ptr_t channel = Channel::Create();
  BasicMessage::ptr_t message = BasicMessage::Create("message body");
  
  EXPECT_THROW(channel->BasicPublish("test_publish_notexist", "test_publish_rk", message), AmqpResponseServerException);
  channel->BasicPublish("", "test_publish_rk", message);
}

TEST(test_publish, publish_mandatory_fail)
{
  Channel::ptr_t channel = Channel::Create();
  BasicMessage::ptr_t message = BasicMessage::Create("message body");

  EXPECT_THROW(channel->BasicPublish("", "test_publish_notexist", message, true), MessageReturnedException);
}

TEST(test_publish, publish_mandatory_success)
{
  Channel::ptr_t channel = Channel::Create();
  BasicMessage::ptr_t message = BasicMessage::Create("message body");
  std::string queue = channel->DeclareQueue("");

  channel->BasicPublish("", queue, message, true);
}

TEST(test_publish, publish_immediate_fail1)
{
  Channel::ptr_t channel = Channel::Create();
  BasicMessage::ptr_t message = BasicMessage::Create("message body");

  // No queue connected
  EXPECT_THROW(channel->BasicPublish("", "test_publish_notexist", message, false, true), MessageReturnedException);
}

TEST(test_publish, publish_immediate_fail2)
{
  Channel::ptr_t channel = Channel::Create();
  BasicMessage::ptr_t message = BasicMessage::Create("message body");
  std::string queue = channel->DeclareQueue("");

  // No consumer connected
  EXPECT_THROW(channel->BasicPublish("", queue, message, false, true), MessageReturnedException);
}

TEST(test_publish, publish_immediate_success)
{
  Channel::ptr_t channel = Channel::Create();
  BasicMessage::ptr_t message = BasicMessage::Create("message body");
  std::string queue = channel->DeclareQueue("");
  std::string consumer = channel->BasicConsume(queue, "");

  channel->BasicPublish("", queue, message, true);
}