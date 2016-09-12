/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MIT
 *
 * Copyright (c) 2010-2013 Alan Antonuk
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

#include <amqp.h>
#include <amqp_framing.h>

#include "SimpleAmqpClient/AmqpException.h"

#include <boost/lexical_cast.hpp>

#include <assert.h>

namespace AmqpClient {

const boost::uint16_t ContentTooLargeException::REPLY_CODE =
    AMQP_CONTENT_TOO_LARGE;
const boost::uint16_t NoRouteException::REPLY_CODE = AMQP_NO_ROUTE;
const boost::uint16_t NoConsumersException::REPLY_CODE = AMQP_NO_CONSUMERS;
const boost::uint16_t AccessRefusedException::REPLY_CODE = AMQP_ACCESS_REFUSED;
const boost::uint16_t NotFoundException::REPLY_CODE = AMQP_NOT_FOUND;
const boost::uint16_t ResourceLockedException::REPLY_CODE =
    AMQP_RESOURCE_LOCKED;
const boost::uint16_t PreconditionFailedException::REPLY_CODE =
    AMQP_PRECONDITION_FAILED;
const boost::uint16_t ConnectionForcedException::REPLY_CODE =
    AMQP_CONNECTION_FORCED;
const boost::uint16_t InvalidPathException::REPLY_CODE = AMQP_INVALID_PATH;
const boost::uint16_t FrameErrorException::REPLY_CODE = AMQP_FRAME_ERROR;
const boost::uint16_t SyntaxErrorException::REPLY_CODE = AMQP_SYNTAX_ERROR;
const boost::uint16_t CommandInvalidException::REPLY_CODE =
    AMQP_COMMAND_INVALID;
const boost::uint16_t ChannelErrorException::REPLY_CODE = AMQP_CHANNEL_ERROR;
const boost::uint16_t UnexpectedFrameException::REPLY_CODE =
    AMQP_UNEXPECTED_FRAME;
const boost::uint16_t ResourceErrorException::REPLY_CODE = AMQP_RESOURCE_ERROR;
const boost::uint16_t NotAllowedException::REPLY_CODE = AMQP_NOT_ALLOWED;
const boost::uint16_t NotImplementedException::REPLY_CODE =
    AMQP_NOT_IMPLEMENTED;
const boost::uint16_t InternalErrorException::REPLY_CODE = AMQP_INTERNAL_ERROR;

void AmqpException::Throw(const amqp_rpc_reply_t &reply) {
  assert(reply.reply_type == AMQP_RESPONSE_SERVER_EXCEPTION);

  switch (reply.reply.id) {
    case AMQP_CONNECTION_CLOSE_METHOD:
      Throw(
          *(reinterpret_cast<amqp_connection_close_t *>(reply.reply.decoded)));
      break;
    case AMQP_CHANNEL_CLOSE_METHOD:
      Throw(*(reinterpret_cast<amqp_channel_close_t *>(reply.reply.decoded)));
      break;
    default:
      throw std::logic_error(
          std::string(
              "Programming error: unknown server exception class/method")
              .append(boost::lexical_cast<std::string>(reply.reply.id)));
  }
}

void AmqpException::Throw(const amqp_channel_close_t &reply) {
  std::ostringstream what;

  std::string reply_text;
  if (reply.reply_text.bytes != NULL) {
    reply_text =
        std::string((char *)reply.reply_text.bytes, reply.reply_text.len);
  }

  const char *method_name =
      amqp_method_name(((reply.class_id << 16) | reply.method_id));
  if (method_name != NULL) {
    what << "channel error: " << reply.reply_code << ": " << method_name
         << " caused: " << reply_text;
  } else {
    what << "channel error: " << reply.reply_code << ": " << reply_text;
  }

  switch (reply.reply_code) {
    case ContentTooLargeException::REPLY_CODE:
      throw ContentTooLargeException(what.str(), reply_text, reply.class_id,
                                     reply.method_id);
    case NoRouteException::REPLY_CODE:
      throw NoRouteException(what.str(), reply_text, reply.class_id,
                             reply.method_id);
    case NoConsumersException::REPLY_CODE:
      throw NoConsumersException(what.str(), reply_text, reply.class_id,
                                 reply.method_id);
    case AccessRefusedException::REPLY_CODE:
      throw AccessRefusedException(what.str(), reply_text, reply.class_id,
                                   reply.method_id);
    case NotFoundException::REPLY_CODE:
      throw NotFoundException(what.str(), reply_text, reply.class_id,
                              reply.method_id);
    case ResourceLockedException::REPLY_CODE:
      throw ResourceLockedException(what.str(), reply_text, reply.class_id,
                                    reply.method_id);
    case PreconditionFailedException::REPLY_CODE:
      throw PreconditionFailedException(what.str(), reply_text, reply.class_id,
                                        reply.method_id);
    default:
      throw std::logic_error(
          std::string("Programming error: unknown channel reply code: ")
              .append(boost::lexical_cast<std::string>(reply.reply_code)));
  }
}

void AmqpException::Throw(const amqp_connection_close_t &reply) {
  std::ostringstream what;
  const char *method_name =
      amqp_method_name(((reply.class_id << 16) | reply.method_id));

  std::string reply_text;
  if (reply.reply_text.bytes != NULL) {
    reply_text =
        std::string((char *)reply.reply_text.bytes, reply.reply_text.len);
  }

  if (method_name != NULL) {
    what << "connection error: " << reply.reply_code << ": " << method_name
         << " caused: " << reply_text;
  } else {
    what << "connection error: " << reply.reply_code << ": " << reply_text;
  }

  switch (reply.reply_code) {
    case ConnectionForcedException::REPLY_CODE:
      throw ConnectionForcedException(what.str(), reply_text, reply.class_id,
                                      reply.method_id);
    case InvalidPathException::REPLY_CODE:
      throw InvalidPathException(what.str(), reply_text, reply.class_id,
                                 reply.method_id);
    case FrameErrorException::REPLY_CODE:
      throw FrameErrorException(what.str(), reply_text, reply.class_id,
                                reply.method_id);
    case SyntaxErrorException::REPLY_CODE:
      throw SyntaxErrorException(what.str(), reply_text, reply.class_id,
                                 reply.method_id);
    case CommandInvalidException::REPLY_CODE:
      throw CommandInvalidException(what.str(), reply_text, reply.class_id,
                                    reply.method_id);
    case ChannelErrorException::REPLY_CODE:
      throw ChannelErrorException(what.str(), reply_text, reply.class_id,
                                  reply.method_id);
    case UnexpectedFrameException::REPLY_CODE:
      throw UnexpectedFrameException(what.str(), reply_text, reply.class_id,
                                     reply.method_id);
    case ResourceErrorException::REPLY_CODE:
      throw ResourceErrorException(what.str(), reply_text, reply.class_id,
                                   reply.method_id);
    case NotAllowedException::REPLY_CODE:
      throw NotAllowedException(what.str(), reply_text, reply.class_id,
                                reply.method_id);
    case NotImplementedException::REPLY_CODE:
      throw NotImplementedException(what.str(), reply_text, reply.class_id,
                                    reply.method_id);
    case InternalErrorException::REPLY_CODE:
      throw InternalErrorException(what.str(), reply_text, reply.class_id,
                                   reply.method_id);
    case AccessRefusedException::REPLY_CODE:
      throw AccessRefusedException(what.str(), reply_text, reply.class_id,
                                   reply.method_id);
    default:
      throw std::logic_error(
          std::string("Programming error: unknown connection reply code: ")
              .append(boost::lexical_cast<std::string>(reply.reply_code)));
  }
}

AmqpException::AmqpException(const std::string &what,
                             const std::string &reply_text,
                             boost::uint16_t class_id,
                             boost::uint16_t method_id) throw()
    : std::runtime_error(what),
      m_reply_text(reply_text),
      m_class_id(class_id),
      m_method_id(method_id) {}
}
