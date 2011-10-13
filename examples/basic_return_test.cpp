#include <SimpleAmqpClient.h>

using namespace AmqpClient;

int main()
{
  char* szBroker = getenv("AMQP_BROKER");
  Channel::ptr_t channel;
  if (szBroker != NULL)
    channel = Channel::Create(szBroker);
  else
    channel = Channel::Create();

  BasicMessage::ptr_t the_message = BasicMessage::Create("Body Content");
  
  channel->DeclareQueue("BasicReturnTestQueue");
  channel->BasicConsume("BasicReturnTestQueue", "consumer_tag1");

  channel->BasicPublish("", "BasicReturnTestQueue", the_message, true, false);
  channel->BasicPublish("", "ThisDoesntExist", the_message, true, false);
  
  try
  {
    for (int i = 0; i < 2; ++i)
    {
      Envelope::ptr_t env;
      if (channel->BasicConsumeMessage(env, 0))
      {
        std::cout << "Received message with body: " << env->Message()->Body() << std::endl;
      }
    }
  }
  catch (MessageReturnedException& e)
  {
    std::cout << "Message got returned: " << e.what();
    std::cout << "\nMessage body: " << e.message()->Body();
  }

}
