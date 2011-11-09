#include <SimpleAmqpClient.h>
#include <gtest/gtest.h>
#include <iostream>

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
  EXPECT_TRUE(channel->BasicConsumeMessage(consumer, delivered, -1));
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

TEST(test_consume, test_initial_qos)
{
  Channel::ptr_t channel = Channel::Create();
  BasicMessage::ptr_t message1 = BasicMessage::Create("Message1");
  BasicMessage::ptr_t message2 = BasicMessage::Create("Message2");
  BasicMessage::ptr_t message3 = BasicMessage::Create("Message3");

  std::string queue = channel->DeclareQueue("");
  channel->BasicPublish("", queue, message1, true);
  channel->BasicPublish("", queue, message2, true);
  channel->BasicPublish("", queue, message3, true);

  std::string consumer = channel->BasicConsume(queue, "", true, false, true, 1);
  Envelope::ptr_t received1, received2;
  EXPECT_TRUE(channel->BasicConsumeMessage(consumer, received1, 1));

  EXPECT_FALSE(channel->BasicConsumeMessage(consumer, received2, 0));
  channel->BasicAck(received1);

  EXPECT_TRUE(channel->BasicConsumeMessage(consumer, received2, 1));
}

TEST(test_consume, test_2consumers)
{
  Channel::ptr_t channel = Channel::Create();
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

TEST(test_consume, test_1000messages)
{
  Channel::ptr_t channel = Channel::Create();
  BasicMessage::ptr_t message1 = BasicMessage::Create("Message1");

  std::string queue = channel->DeclareQueue("");
  std::string consumer = channel->BasicConsume(queue, "", true, false, true, 2);

  Envelope::ptr_t msg;
  for (int i = 0; i < 1000; ++i)
  {
    message1->Timestamp(i);
    channel->BasicPublish("", queue, message1, true);
    channel->BasicConsumeMessage(consumer, msg);
  }

}