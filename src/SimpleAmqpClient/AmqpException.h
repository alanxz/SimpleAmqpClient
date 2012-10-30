#ifndef CHANNEL_EXCEPTION_H
#define CHANNEL_EXCEPTION_H
/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MIT
 *
 * Copyright (c) 2010-2012 Alan Antonuk
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * ***** END LICENSE BLOCK *****
 */

#include "Util.h"

#include <boost/cstdint.hpp>

#include <stdexcept>
#include <string>

#ifdef _MSC_VER
# pragma warning ( push )
# pragma warning ( disable: 4251 4275 )
#endif 

struct amqp_rpc_reply_t_;
struct amqp_channel_close_t_;
struct amqp_connection_close_t_;

namespace AmqpClient
{

/**
 * Base-class for exceptions from the broker
 *
 */
class SIMPLEAMQPCLIENT_EXPORT AmqpException : public std::runtime_error
{
public:
  /**
   * Construct an AmqpException from an amqp_rpc_reply_t and throw it
   *
   * @param [in] reply the reply from the RPC call
   */
  static void Throw(const amqp_rpc_reply_t_& reply);

  /**
   * Construct an AmqpException from an amqp_channel_close_t and throw it
   *
   * @param [in] reply the channel.close AMQP method
   */
  static void Throw(const amqp_channel_close_t_& reply);

  /**
   * Construct an AmqpException from an amqp_connection_close_t and throw it
   *
   * @param [in] reply the connection.close AMQP method
   */
  static void Throw(const amqp_connection_close_t_& reply);

  /**
   * AmqpException constructor
   *
   * @param [in] what the error string to pass to the std::runtime_error base-class
   * @param [in] reply_text the error message (if any) from the broker
   * @param [in] class_id the class id of the method that caused the error
   * @param [in] method_id the method id of the method that caused the error
   */
  explicit AmqpException(const std::string& what, const std::string &reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw();

  /**
   * Destructor
   */
  virtual ~AmqpException() throw() {}

  /**
   * Query to see if the error is soft
   *
   * A soft error is generally recoverable, meaning the Channel
   * object that it originated from can be reused. If its a hard error
   * the Channel object is closed and will throw exceptions if its used
   * again.
   *
   * @returns true if its a soft error, false otherwise
   */
  virtual bool is_soft_error() const throw() = 0;

  /**
   * Get the error code returned from the broker
   *
   * @returns the error code
   */
  virtual boost::uint16_t reply_code() const throw() = 0;

  /**
   * Get the class id of the method that caused the error
   *
   * @returns the class id
   */
  virtual boost::uint16_t class_id() const throw() { return m_class_id; }

  /**
   * Get the method id of the method that caused the error
   *
   * @returns the method id
   */
  virtual boost::uint16_t method_id() const throw() { return m_method_id; }

  /**
   * Get the error string returned from the broker
   *
   * @returns the error string from the broker
   */
  virtual std::string reply_text() const throw() { return m_reply_text; }

protected:
  /** @cond INTERNAL */
  std::string m_reply_text;
  boost::uint16_t m_class_id;
  boost::uint16_t m_method_id;
  /** @endcond */
};

/**
 * Base class for exceptions that result in the connection being closed
 *
 * If the program receives this kind of exception, the Channel object that
 * threw the exception is now disconnected from the broker and any further
 * use of the channel will cause further errors
 */
class SIMPLEAMQPCLIENT_EXPORT ConnectionException : public AmqpException
{
public:
  /**
   * Constructor
   *
   * @param [in] what
   * @param [in] reply_text
   * @param [in] class_id
   * @param [in] method_id
   */
  explicit ConnectionException(const std::string &what, const std::string& reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    AmqpException(what, reply_text, class_id, method_id) {}

  virtual bool is_soft_error() const throw() { return false; }
};


/**
 * Base class for exceptions that are soft errors
 *
 * If the program receives this kind of exception, the Channel object
 * that threw the exception will continue to be functional.
 */
class SIMPLEAMQPCLIENT_EXPORT ChannelException : public AmqpException
{
public:
  /**
   * Constructor
   *
   * @param [in] what
   * @param [in] reply_text
   * @param [in] class_id
   * @param [in] method_id
   */
  explicit ChannelException(const std::string &what, const std::string& reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    AmqpException(what, reply_text, class_id, method_id) {}

  virtual bool is_soft_error() const throw() { return true; }
};

/**
 * The connection was force closed by an operator
 */
class SIMPLEAMQPCLIENT_EXPORT ConnectionForcedException : public ConnectionException
{
public:
  /** reply code */
  static const boost::uint16_t REPLY_CODE;

  /**
   * Constructor
   */
  explicit ConnectionForcedException(const std::string &what, const std::string& reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    ConnectionException(what, reply_text, class_id, method_id) {}

  virtual boost::uint16_t reply_code() const throw() { return REPLY_CODE; }
};

/**
 * The client tried to work with an invalid virtual host
 */
class SIMPLEAMQPCLIENT_EXPORT InvalidPathException : public ConnectionException
{
public:
  /** reply code */
  static const boost::uint16_t REPLY_CODE;

  /**
   * Constructor
   */
  explicit InvalidPathException(const std::string &what, const std::string& reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    ConnectionException(what, reply_text, class_id, method_id) {}

  virtual boost::uint16_t reply_code() const throw() { return REPLY_CODE; }
};

/**
 * The broker received a malformed frame.
 *
 * This likely indicates a bug in SimpleAmqpClient
 */
class SIMPLEAMQPCLIENT_EXPORT FrameErrorException : public ConnectionException
{
public:
  /** reply code */
  static const boost::uint16_t REPLY_CODE;

  /**
   * Constructor
   */
  explicit FrameErrorException(const std::string &what, const std::string& reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    ConnectionException(what, reply_text, class_id, method_id) {}

  virtual boost::uint16_t reply_code() const throw() { return REPLY_CODE; }
};

/**
 * The client sent a frame with bad values (out of range)
 *
 * This likely indicates a bug in SimpleAmqpClient
 */
class SIMPLEAMQPCLIENT_EXPORT SyntaxErrorException : public ConnectionException
{
public:
  /** reply code */
  static const boost::uint16_t REPLY_CODE;
  /**
   * Constructor
   */
  explicit SyntaxErrorException(const std::string &what, const std::string& reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    ConnectionException(what, reply_text, class_id, method_id) {}

  virtual boost::uint16_t reply_code() const throw() { return REPLY_CODE; }
};

/**
 * The client sent an invalid sequence of frames
 *
 * This likely indicates a bug in SimpleAmqpClient
 */
class SIMPLEAMQPCLIENT_EXPORT CommandInvalidException : public ConnectionException
{
public:
  /** reply code */
  static const boost::uint16_t REPLY_CODE;

  /**
   * Constructor
   */
  explicit CommandInvalidException(const std::string &what, const std::string& reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    ConnectionException(what, reply_text, class_id, method_id) {}

  virtual boost::uint16_t reply_code() const throw() { return REPLY_CODE; }
};

/**
 * The client tried to use a Channel that isn't open
 *
 * This likely indicates a bug in SimpleAmqpClient
 */
class SIMPLEAMQPCLIENT_EXPORT ChannelErrorException : public ConnectionException
{
public:
  /** reply code */
  static const boost::uint16_t REPLY_CODE;
  /**
   * Constructor
   */
  explicit ChannelErrorException(const std::string &what, const std::string& reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    ConnectionException(what, reply_text, class_id, method_id) {}

  virtual boost::uint16_t reply_code() const throw() { return REPLY_CODE; }
};

/**
 * The client sent a frame the broker wasn't expecting
 *
 * This likely indicates a bug in SimpleAmqpClient
 */
class SIMPLEAMQPCLIENT_EXPORT UnexpectedFrameException : public ConnectionException
{
public:
  /** reply code */
  static const boost::uint16_t REPLY_CODE;

  /**
   * Constructor
   */
  explicit UnexpectedFrameException(const std::string &what, const std::string& reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    ConnectionException(what, reply_text, class_id, method_id) {}

  virtual boost::uint16_t reply_code() const throw() { return REPLY_CODE; }
};

/**
 * The broker could not complete the request because of lack of resources
 *
 * Likely due to clients creating too many of something
 */
class SIMPLEAMQPCLIENT_EXPORT ResourceErrorException : public ConnectionException
{
public:
  /** reply code */
  static const boost::uint16_t REPLY_CODE;

  /**
   * Constructor
   */
  explicit ResourceErrorException(const std::string &what, const std::string& reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    ConnectionException(what, reply_text, class_id, method_id) {}

  virtual boost::uint16_t reply_code() const throw() { return REPLY_CODE; }
};

/**
 * The client tried to work with some entity in a manner that is prohibited by the server.
 *
 * Like due to security settings or by some other criteria.
 */
class SIMPLEAMQPCLIENT_EXPORT NotAllowedException : public ConnectionException
{
public:
  /** reply code */
  static const boost::uint16_t REPLY_CODE;

  /**
   * Constructor
   */
  explicit NotAllowedException(const std::string &what, const std::string& reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    ConnectionException(what, reply_text, class_id, method_id) {}

  virtual boost::uint16_t reply_code() const throw() { return REPLY_CODE; }
};

/**
 * The client tried to use a method that isn't implemented by the broker
 *
 */
class SIMPLEAMQPCLIENT_EXPORT NotImplementedException : public ConnectionException
{
public:
  /** reply code */
  static const boost::uint16_t REPLY_CODE;

  /**
   * Constructor
   */
  explicit NotImplementedException(const std::string &what, const std::string& reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    ConnectionException(what, reply_text, class_id, method_id) {}

  virtual boost::uint16_t reply_code() const throw() { return REPLY_CODE; }
};

/**
 * An internal error occurred on the broker
 */
class SIMPLEAMQPCLIENT_EXPORT InternalErrorException : public ConnectionException
{
public:
  /** reply code */
  static const boost::uint16_t REPLY_CODE;

  /**
   * Constructor
   */
  explicit InternalErrorException(const std::string &what, const std::string& reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    ConnectionException(what, reply_text, class_id, method_id) {}

  virtual boost::uint16_t reply_code() const throw() { return REPLY_CODE; }
};

/**
 * The client tried to send content that was too large
 *
 * Try sending the content at a later time
 */
class SIMPLEAMQPCLIENT_EXPORT ContentTooLargeException : public ChannelException
{
public:
  /** reply code */
  static const boost::uint16_t REPLY_CODE;

  /**
   * Constructor
   */
  explicit ContentTooLargeException(const std::string &what, const std::string reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    ChannelException(what, reply_text, class_id, method_id) {}

  virtual boost::uint16_t reply_code() const throw() { return REPLY_CODE; }
};

/**
 * When e
 */
class SIMPLEAMQPCLIENT_EXPORT NoRouteException : public ChannelException
{
public:
  static const boost::uint16_t REPLY_CODE;
  explicit NoRouteException(const std::string &what, const std::string reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    ChannelException(what, reply_text, class_id, method_id) {}

  virtual boost::uint16_t reply_code() const throw() { return REPLY_CODE; }
};
class SIMPLEAMQPCLIENT_EXPORT NoConsumersException : public ChannelException
{
public:
  static const boost::uint16_t REPLY_CODE;
  explicit NoConsumersException(const std::string &what, const std::string reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    ChannelException(what, reply_text, class_id, method_id) {}

  virtual boost::uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT AccessRefusedException : public ChannelException
{
public:
  static const boost::uint16_t REPLY_CODE;
  explicit AccessRefusedException(const std::string &what, const std::string reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    ChannelException(what, reply_text, class_id, method_id) {}

  virtual boost::uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT NotFoundException : public ChannelException
{
public:
  static const boost::uint16_t REPLY_CODE;
  explicit NotFoundException(const std::string &what, const std::string reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    ChannelException(what, reply_text, class_id, method_id) {}

  virtual boost::uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT ResourceLockedException : public ChannelException
{
public:
  static const boost::uint16_t REPLY_CODE;
  explicit ResourceLockedException(const std::string &what, const std::string reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    ChannelException(what, reply_text, class_id, method_id) {}

  virtual boost::uint16_t reply_code() const throw() { return REPLY_CODE; }
};

class SIMPLEAMQPCLIENT_EXPORT PreconditionFailedException : public ChannelException
{
public:
  static const boost::uint16_t REPLY_CODE;
  explicit PreconditionFailedException(const std::string &what, const std::string reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
    ChannelException(what, reply_text, class_id, method_id) {}

  virtual boost::uint16_t reply_code() const throw() { return REPLY_CODE; }
};

} // namespace AmqpClient

#ifdef _MSC_VER
# pragma warning ( pop )
#endif

#endif // CHANNEL_EXCEPTION_H
