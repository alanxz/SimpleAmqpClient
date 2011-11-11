#ifndef CONNECTION_CLOSED_EXCEPTION_H
#define CONNECTION_CLOSED_EXCEPTION_H

#include <stdexcept>

namespace AmqpClient
{

class ConnectionClosedException : public std::runtime_error
{
public:
  explicit ConnectionClosedException() :
    std::runtime_error("Connection is closed") {} 
};
} // namespace AmqpClient
#endif // CONNECTION_CLOSED_EXCEPTION_H