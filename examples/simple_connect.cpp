#include "Connection.h"
#include "Channel.h"

#include <iostream>

using namespace AmqpClient;
int main()
{
	Connection::ptr_t conn = Connection::Create("127.0.0.1", 5672);
	Channel::ptr_t channel = conn->CreateChannel();

	channel->DeclareQueue("alanqueue");
	channel->BindQueue("alanqueue", "amq.direct", "alankey");

	Message::ptr_t msg_in = Message::Create();

	msg_in->Body("This is a small message.");

	channel->BasicPublish("amq.direct", "alankey", msg_in);

	channel->BasicConsume("alanqueue", "consumertag");

	Message::ptr_t msg_out = channel->BasicConsumeMessage();

	std::cout << "Message text: " << msg_out->Body() << std::endl;

}

