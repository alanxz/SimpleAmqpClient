#include "connected_test.h"

using namespace AmqpClient;

TEST_F(connected_test, get_ok)
{
  BasicMessage::ptr_t message = BasicMessage::Create("Message Body");
  std::string queue = channel->DeclareQueue("");
  channel->BasicPublish("", queue, message, true);

  Envelope::ptr_t new_message;
  EXPECT_TRUE(channel->BasicGet(new_message, queue));
  EXPECT_EQ(message->Body(), new_message->Message()->Body());
}

TEST_F(connected_test, get_empty)
{
  BasicMessage::ptr_t message = BasicMessage::Create("Message Body");
  std::string queue = channel->DeclareQueue("");

  Envelope::ptr_t new_message;
  EXPECT_FALSE(channel->BasicGet(new_message, queue));
}

TEST(test_get, get_big)
{
  // Smallest frame size allowed by AMQP
  Channel::ptr_t channel = Channel::Create("127.0.0.1", 5672, "guest", "guest", "/", 4096);
  // Create a message with a body larger than a single frame
  BasicMessage::ptr_t message = BasicMessage::Create(std::string(4099, 'a'));
  std::string queue = channel->DeclareQueue("");

  channel->BasicPublish("", queue, message);
  Envelope::ptr_t new_message;
  EXPECT_TRUE(channel->BasicGet(new_message, queue));
  EXPECT_EQ(message->Body(), new_message->Message()->Body());
}

TEST_F(connected_test, bad_queue)
{
  Envelope::ptr_t new_message;
  EXPECT_THROW(channel->BasicGet(new_message, "test_get_nonexistantqueue"), ChannelException);
}

TEST_F(connected_test, ack_message)
{
  BasicMessage::ptr_t message = BasicMessage::Create("Message Body");
  std::string queue = channel->DeclareQueue("");
  channel->BasicPublish("", queue, message, true);

  Envelope::ptr_t new_message;
  EXPECT_TRUE(channel->BasicGet(new_message, queue, false));
  channel->BasicAck(new_message);
  EXPECT_FALSE(channel->BasicGet(new_message, queue, false));
}