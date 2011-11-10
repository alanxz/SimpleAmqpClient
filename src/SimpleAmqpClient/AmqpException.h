#ifndef CHANNEL_EXCEPTION_H
#define CHANNEL_EXCEPTION_H

#include "Util.h"

#include <boost/cstdint.hpp>

#include <stdexcept>

struct amqp_rpc_reply_t_;
struct amqp_channel_close_t_;
struct amqp_connection_close_t_;

namespace AmqpClient
{

class SIMPLEAMQPCLIENT_EXPORT AmqpException : public std::runtime_error
{
public:
  static void Throw(const amqp_rpc_reply_t_& reply);
  static void Throw(const amqp_channel_close_t_& reply);
  static void Throw(const amqp_connection_close_t_& reply);

  explicit AmqpException(const std::string& reply_text, uint16_t class_id, uint16_t method_id) throw() :
      std::runtime_error(reply_text),
      m_reply_text(reply_text),
      m_class_id(class_id),
      m_method_id(method_id)
      {}
  
  virtual bool is_soft_error() const throw() = 0;
  virtual uint16_t reply_code() const throw() = 0;
  virtual uint16_t class_id() const throw() { return m_class_id; }
  virtual uint16_t method_id() const throw() { return m_method_id; }

protected:
  std::string m_reply_text;
  uint16_t m_class_id;
  uint16_t m_method_id;
};

class SIMPLEAMQPCLIENT_EXPORT ConnectionException : public AmqpException
{
public:
  explicit ConnectionException(const std::string& reply_text, uint16_t class_id, uint16_t method_id) throw() :
    AmqpException(std::string("connection exception: ").append(reply_text), class_id, method_id) {}

  virtual bool is_soft_error() const throw() { return false; }
};

class SIMPLEAMQPCLIENT_EXPORT ChannelException : public AmqpException
{
public:
  explicit ChannelException(const std::string& reply_text, uint16_t class_id, uint16_t method_id) throw() :
    AmqpException(std::string("channel exception: ").append(reply_text), class_id, method_id) {}

  virtual bool is_soft_error() const throw() { return true; }
};

class SIMPLEAMQPCLIENT_EXPORT ConnectionForcedException : public ConnectionException
{
public:
  static const uint16_t REPLY_CODE;
  explicit ConnectionForcedException(const std::string& reply_text, uint16_t class_id, uint16_t method_id) throw() :
    ConnectionException(reply_text, class_id, method_id) {}

  virtual uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT InvalidPathException : public ConnectionException
{
public:
  static const uint16_t REPLY_CODE;
  explicit InvalidPathException(const std::string& reply_text, uint16_t class_id, uint16_t method_id) throw() :
    ConnectionException(reply_text, class_id, method_id) {}

  virtual uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT FrameErrorException : public ConnectionException
{
public:
  static const uint16_t REPLY_CODE;
  explicit FrameErrorException(const std::string& reply_text, uint16_t class_id, uint16_t method_id) throw() :
    ConnectionException(reply_text, class_id, method_id) {}

  virtual uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT SyntaxErrorException : public ConnectionException
{
public:
  static const uint16_t REPLY_CODE;
  explicit SyntaxErrorException(const std::string& reply_text, uint16_t class_id, uint16_t method_id) throw() :
    ConnectionException(reply_text, class_id, method_id) {}

  virtual uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT CommandInvalidException : public ConnectionException
{
public:
  static const uint16_t REPLY_CODE;
  explicit CommandInvalidException(const std::string& reply_text, uint16_t class_id, uint16_t method_id) throw() :
    ConnectionException(reply_text, class_id, method_id) {}

  virtual uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT ChannelErrorException : public ConnectionException
{
public:
  static const uint16_t REPLY_CODE;
  explicit ChannelErrorException(const std::string& reply_text, uint16_t class_id, uint16_t method_id) throw() :
    ConnectionException(reply_text, class_id, method_id) {}

  virtual uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT UnexpectedFrameException : public ConnectionException
{
public:
  static const uint16_t REPLY_CODE;
  explicit UnexpectedFrameException(const std::string& reply_text, uint16_t class_id, uint16_t method_id) throw() :
    ConnectionException(reply_text, class_id, method_id) {}

  virtual uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT ResourceErrorException : public ConnectionException
{
public:
  static const uint16_t REPLY_CODE;
  explicit ResourceErrorException(const std::string& reply_text, uint16_t class_id, uint16_t method_id) throw() :
    ConnectionException(reply_text, class_id, method_id) {}

  virtual uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT NotAllowedException : public ConnectionException
{
public:
  static const uint16_t REPLY_CODE;
  explicit NotAllowedException(const std::string& reply_text, uint16_t class_id, uint16_t method_id) throw() :
    ConnectionException(reply_text, class_id, method_id) {}

  virtual uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT NotImplementedException : public ConnectionException
{
public:
  static const uint16_t REPLY_CODE;
  explicit NotImplementedException(const std::string& reply_text, uint16_t class_id, uint16_t method_id) throw() :
    ConnectionException(reply_text, class_id, method_id) {}

  virtual uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT InternalErrorException : public ConnectionException
{
public:
  static const uint16_t REPLY_CODE;
  explicit InternalErrorException(const std::string& reply_text, uint16_t class_id, uint16_t method_id) throw() :
    ConnectionException(reply_text, class_id, method_id) {}

  virtual uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT ContentTooLargeException : public ChannelException
{
public:
  static const uint16_t REPLY_CODE;
  explicit ContentTooLargeException(const std::string reply_text, uint16_t class_id, uint16_t method_id) throw() :
    ChannelException(reply_text, class_id, method_id) {}

  virtual uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT NoRouteException : public ChannelException
{
public:
  static const uint16_t REPLY_CODE;
  explicit NoRouteException(const std::string reply_text, uint16_t class_id, uint16_t method_id) throw() :
    ChannelException(reply_text, class_id, method_id) {}

  virtual uint16_t reply_code() const throw() { return REPLY_CODE; }
};
class SIMPLEAMQPCLIENT_EXPORT NoConsumersException : public ChannelException
{
public:
  static const uint16_t REPLY_CODE;
  explicit NoConsumersException(const std::string reply_text, uint16_t class_id, uint16_t method_id) throw() :
    ChannelException(reply_text, class_id, method_id) {}

  virtual uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT AccessRefusedException : public ChannelException
{
public:
  static const uint16_t REPLY_CODE;
  explicit AccessRefusedException(const std::string reply_text, uint16_t class_id, uint16_t method_id) throw() :
    ChannelException(reply_text, class_id, method_id) {}

  virtual uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT NotFoundException : public ChannelException
{
public:
  static const uint16_t REPLY_CODE;
  explicit NotFoundException(const std::string reply_text, uint16_t class_id, uint16_t method_id) throw() :
    ChannelException(reply_text, class_id, method_id) {}

  virtual uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT ResourceLockedException : public ChannelException
{
public:
  static const uint16_t REPLY_CODE;
  explicit ResourceLockedException(const std::string reply_text, uint16_t class_id, uint16_t method_id) throw() :
    ChannelException(reply_text, class_id, method_id) {}

  virtual uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT PreconditionFailedException : public ChannelException
{
public:
  static const uint16_t REPLY_CODE;
  explicit PreconditionFailedException(const std::string reply_text, uint16_t class_id, uint16_t method_id) throw() :
    ChannelException(reply_text, class_id, method_id) {}

  virtual uint16_t reply_code() const throw() { return REPLY_CODE; }
};

} // namespace AmqpClient
#endif // CHANNEL_EXCEPTION_H