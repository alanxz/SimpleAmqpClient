#ifndef AMQPRESPONSESERVEREXCEPTION_H
#define AMQPRESPONSESERVEREXCEPTION_H

namespace AmqpClient {

class AmqpResponseServerException : public std::exception
{
public:
    AmqpResponseServerException();
};

} // namespace AmqpClient

#endif // AMQPRESPONSESERVEREXCEPTION_H
