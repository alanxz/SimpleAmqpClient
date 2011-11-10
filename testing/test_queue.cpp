#include "connected_test.h"

TEST_F(connected_test, queue_declare)
{
  std::string queue = channel->DeclareQueue("");
  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_declare_named)
{
  std::string queue = channel->DeclareQueue("declare_queue_test");
  EXPECT_EQ("declare_queue_test", queue);

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_declare_passive)
{
  std::string queue = channel->DeclareQueue("declare_queue_passive");
  channel->DeclareQueue("declare_queue_passive", true);

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_declare_passive_fail)
{
  EXPECT_THROW(channel->DeclareQueue("declare_queue_notexist", true), AmqpResponseServerException);
}

TEST_F(connected_test, queue_declare_durable)
{
  std::string queue = channel->DeclareQueue("declare_queue_durable", false, true, false);

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_declare_notexclusive)
{
  std::string queue = channel->DeclareQueue("declare_queue_notexclusive", false, false, false);

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_declare_notautodelete)
{
  std::string queue = channel->DeclareQueue("declare_queue_notautodelete", false, false, false, false);

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_delete)
{
  std::string queue = channel->DeclareQueue("delete_queue");
  channel->DeleteQueue(queue);
  EXPECT_THROW(channel->DeclareQueue(queue, true), AmqpResponseServerException);
}

TEST_F(connected_test, queue_delete_badqueue)
{
  EXPECT_THROW(channel->DeleteQueue("delete_queue_notexist"), AmqpResponseServerException);
}

TEST_F(connected_test, queue_delete_ifunused)
{
  std::string queue = channel->DeclareQueue("delete_queue_ifunused");
  channel->DeleteQueue(queue, true);
  EXPECT_THROW(channel->DeclareQueue(queue, true), AmqpResponseServerException);
}

TEST_F(connected_test, queue_delete_ifused)
{
  std::string queue = channel->DeclareQueue("delete_queue_ifused");
  channel->BasicConsume(queue);
  EXPECT_THROW(channel->DeleteQueue(queue, true), AmqpResponseServerException);

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_delete_ifempty)
{
  std::string queue = channel->DeclareQueue("delete_queue_ifempty");
  channel->DeleteQueue(queue, false, true);

  EXPECT_THROW(channel->DeclareQueue(queue, true), AmqpResponseServerException);
}

TEST_F(connected_test, queue_delete_ifnotempty)
{
  std::string queue = channel->DeclareQueue("delete_queue_ifnotempty");
  BasicMessage::ptr_t message = BasicMessage::Create("Message body");
  channel->BasicPublish("", queue, message, true);

  EXPECT_THROW(channel->DeleteQueue(queue, false, true), AmqpResponseServerException);

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_bind)
{
  channel->DeclareExchange("queue_bind_exchange");
  std::string queue = channel->DeclareQueue("queue_bind_queue");

  channel->BindQueue(queue, "queue_bind_exchange", "rk");

  channel->DeleteExchange("queue_bind_exchange");
  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_bind_badexchange)
{
  std::string queue = channel->DeclareQueue("queue_bind_badexchange");

  EXPECT_THROW(channel->BindQueue(queue, "queue_bind_exchangenotexist", "rk"), AmqpResponseServerException);

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_bind_badqueue)
{
  channel->DeclareExchange("queue_bind_badqueue");

  EXPECT_THROW(channel->BindQueue("queue_bind_queuenotexist", "queue_bind_badqueue", "rk"), AmqpResponseServerException);

  channel->DeleteExchange("queue_bind_badqueue");
}

TEST_F(connected_test, queue_bind_nokey)
{
  channel->DeclareExchange("queue_bind_exchange");
  std::string queue = channel->DeclareQueue("queue_bind_queue");

  channel->BindQueue(queue, "queue_bind_exchange");

  channel->DeleteExchange("queue_bind_exchange");
  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_unbind)
{
  channel->DeclareExchange("queue_unbind_exchange");
  std::string queue = channel->DeclareQueue("queue_unbind_queue");
  channel->BindQueue(queue, "queue_unbind_exchange", "rk");

  channel->UnbindQueue(queue, "queue_unbind_exchange", "rk");

  channel->DeleteExchange("queue_unbind_exchange");
  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_unbind_badbinding)
{
  EXPECT_THROW(channel->UnbindQueue("queue_unbind_queuenotexist", "queue_unbind_exchangenotexist", "rk"), AmqpResponseServerException);
}

TEST_F(connected_test, queue_purge)
{
  std::string queue = channel->DeclareQueue("queue_purge");
  BasicMessage::ptr_t message = BasicMessage::Create("Message Body");
  channel->BasicPublish("", queue, message, true);

  channel->PurgeQueue(queue);
  Envelope::ptr_t envelope;
  EXPECT_FALSE(channel->BasicGet(envelope, queue));

  channel->DeleteQueue(queue);
}

TEST_F(connected_test, queue_purge_badqueue)
{
  EXPECT_THROW(channel->PurgeQueue("purge_queue_queuenotexist"), AmqpResponseServerException);
}