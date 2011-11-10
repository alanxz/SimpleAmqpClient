#ifndef CONNECTED_TEST_H
#define CONNECTED_TEST_H

#include <SimpleAmqpClient.h>
#include <gtest/gtest.h>

using namespace AmqpClient;

class connected_test : public ::testing::Test
{
public:
  virtual void SetUp()
  {
    channel = Channel::Create();
  }

  Channel::ptr_t channel;
};

#endif // CONNECTED_TEST_H