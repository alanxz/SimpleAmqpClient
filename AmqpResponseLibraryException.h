#ifndef AMQPRESPONSELIBRARYEXCEPTION_H
#define AMQPRESPONSELIBRARYEXCEPTION_H

namespace AmqpClient {

class AmqpResponseLibraryException : public std::exception
{
public:
    AmqpResponseLibraryException();
};

} // namespace AmqpClient

#endif // AMQPRESPONSELIBRARYEXCEPTION_H
