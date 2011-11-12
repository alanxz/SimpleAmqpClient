#ifndef CHANNEL_EXCEPTION_H
#define CHANNEL_EXCEPTION_H
/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1
 *
 * ``The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is SimpleAmqpClient for RabbitMQ.
 *
 * The Initial Developer of the Original Code is Alan Antonuk.
 * Original code is Copyright (C) Alan Antonuk.
 *
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the GNU General Public License Version 2 or later (the "GPL"), in
 * which case the provisions of the GPL are applicable instead of those
 * above. If you wish to allow use of your version of this file only
 * under the terms of the GPL, and not to allow others to use your
 * version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the
 * notice and other provisions required by the GPL. If you do not
 * delete the provisions above, a recipient may use your version of
 * this file under the terms of any one of the MPL or the GPL.
 *
 * ***** END LICENSE BLOCK *****
 */

#include "Util.h"

#include <boost/cstdint.hpp>

#include <stdexcept>

#ifdef _MSC_VER
# pragma warning ( push )
# pragma warning ( disable: 4251 4275 )
#endif 

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

  explicit AmqpException(const std::string& reply_text, uint16_t class_id, uint16_t method_id) throw();
  virtual ~AmqpException() throw() {}
  
  virtual bool is_soft_error() const throw() = 0;
  virtual uint16_t reply_code() const throw() = 0;
  virtual uint16_t class_id() const throw() { return m_class_id; }
  virtual uint16_t method_id() const throw() { return m_method_id; }
  virtual std::string reply_text() const throw() { return m_reply_text; }

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

#ifdef _MSC_VER
# pragma warning ( pop )
#endif

#endif // CHANNEL_EXCEPTION_H
