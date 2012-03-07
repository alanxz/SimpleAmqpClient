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

#include <amqp.h>
#include <amqp_framing.h>

#include "SimpleAmqpClient/AmqpException.h"

#include <boost/lexical_cast.hpp>

#include <assert.h>

namespace AmqpClient
{

const boost::uint16_t ContentTooLargeException::REPLY_CODE = AMQP_CONTENT_TOO_LARGE;
const boost::uint16_t NoRouteException::REPLY_CODE = AMQP_NO_ROUTE;
const boost::uint16_t NoConsumersException::REPLY_CODE = AMQP_NO_CONSUMERS;
const boost::uint16_t AccessRefusedException::REPLY_CODE = AMQP_ACCESS_REFUSED;
const boost::uint16_t NotFoundException::REPLY_CODE = AMQP_NOT_FOUND;
const boost::uint16_t ResourceLockedException::REPLY_CODE = AMQP_RESOURCE_LOCKED;
const boost::uint16_t PreconditionFailedException::REPLY_CODE = AMQP_PRECONDITION_FAILED;
const boost::uint16_t ConnectionForcedException::REPLY_CODE = AMQP_CONNECTION_FORCED;
const boost::uint16_t InvalidPathException::REPLY_CODE = AMQP_INVALID_PATH;
const boost::uint16_t FrameErrorException::REPLY_CODE = AMQP_FRAME_ERROR;
const boost::uint16_t SyntaxErrorException::REPLY_CODE = AMQP_SYNTAX_ERROR;
const boost::uint16_t CommandInvalidException::REPLY_CODE = AMQP_COMMAND_INVALID;
const boost::uint16_t ChannelErrorException::REPLY_CODE = AMQP_CHANNEL_ERROR;
const boost::uint16_t UnexpectedFrameException::REPLY_CODE = AMQP_UNEXPECTED_FRAME;
const boost::uint16_t ResourceErrorException::REPLY_CODE = AMQP_RESOURCE_ERROR;
const boost::uint16_t NotAllowedException::REPLY_CODE = AMQP_NOT_ALLOWED;
const boost::uint16_t NotImplementedException::REPLY_CODE = AMQP_NOT_IMPLEMENTED;
const boost::uint16_t InternalErrorException::REPLY_CODE = AMQP_INTERNAL_ERROR;

void AmqpException::Throw(const amqp_rpc_reply_t& reply)
{
  assert(reply.reply_type == AMQP_RESPONSE_SERVER_EXCEPTION);

  switch (reply.reply.id)
  {
  case AMQP_CONNECTION_CLOSE_METHOD:
    Throw(*(reinterpret_cast<amqp_connection_close_t*>(reply.reply.decoded)));
    break;
  case AMQP_CHANNEL_CLOSE_METHOD:
    Throw(*(reinterpret_cast<amqp_channel_close_t*>(reply.reply.decoded)));
    break;
  default:
    throw std::logic_error(std::string("Programming error: unknown server exception class/method").append(boost::lexical_cast<std::string>(reply.reply.id)));
  }
}

void AmqpException::Throw(const amqp_channel_close_t& reply)
{
  switch (reply.reply_code)
  {
  case ContentTooLargeException::REPLY_CODE:
    throw ContentTooLargeException(std::string((char*)reply.reply_text.bytes, reply.reply_text.len), reply.class_id, reply.method_id);
  case NoRouteException::REPLY_CODE:
    throw NoRouteException(std::string((char*)reply.reply_text.bytes, reply.reply_text.len), reply.class_id, reply.method_id);
  case NoConsumersException::REPLY_CODE:
    throw NoConsumersException(std::string((char*)reply.reply_text.bytes, reply.reply_text.len), reply.class_id, reply.method_id);
  case AccessRefusedException::REPLY_CODE:
    throw AccessRefusedException(std::string((char*)reply.reply_text.bytes, reply.reply_text.len), reply.class_id, reply.method_id);
  case NotFoundException::REPLY_CODE:
    throw NotFoundException(std::string((char*)reply.reply_text.bytes, reply.reply_text.len), reply.class_id, reply.method_id);
  case ResourceLockedException::REPLY_CODE:
    throw ResourceLockedException(std::string((char*)reply.reply_text.bytes, reply.reply_text.len), reply.class_id, reply.method_id);
  case PreconditionFailedException::REPLY_CODE:
    throw PreconditionFailedException(std::string((char*)reply.reply_text.bytes, reply.reply_text.len), reply.class_id, reply.method_id);
  default:
    throw std::logic_error(std::string("Programming error: unknown channel reply code: ").append(boost::lexical_cast<std::string>(reply.reply_code)));
  }
}

void AmqpException::Throw(const amqp_connection_close_t& reply)
{
  switch (reply.reply_code)
  {
  case ConnectionForcedException::REPLY_CODE:
    throw ConnectionForcedException(std::string((char*)reply.reply_text.bytes, reply.reply_text.len), reply.class_id, reply.method_id);
  case InvalidPathException::REPLY_CODE:
    throw InvalidPathException(std::string((char*)reply.reply_text.bytes, reply.reply_text.len), reply.class_id, reply.method_id);
  case FrameErrorException::REPLY_CODE:
    throw FrameErrorException(std::string((char*)reply.reply_text.bytes, reply.reply_text.len), reply.class_id, reply.method_id);
  case SyntaxErrorException::REPLY_CODE:
    throw SyntaxErrorException(std::string((char*)reply.reply_text.bytes, reply.reply_text.len), reply.class_id, reply.method_id);
  case CommandInvalidException::REPLY_CODE:
    throw CommandInvalidException(std::string((char*)reply.reply_text.bytes, reply.reply_text.len), reply.class_id, reply.method_id);
  case ChannelErrorException::REPLY_CODE:
    throw ChannelErrorException(std::string((char*)reply.reply_text.bytes, reply.reply_text.len), reply.class_id, reply.method_id);
  case UnexpectedFrameException::REPLY_CODE:
    throw UnexpectedFrameException(std::string((char*)reply.reply_text.bytes, reply.reply_text.len), reply.class_id, reply.method_id);
  case ResourceErrorException::REPLY_CODE:
    throw ResourceErrorException(std::string((char*)reply.reply_text.bytes, reply.reply_text.len), reply.class_id, reply.method_id);
  case NotAllowedException::REPLY_CODE:
    throw NotAllowedException(std::string((char*)reply.reply_text.bytes, reply.reply_text.len), reply.class_id, reply.method_id);
  case NotImplementedException::REPLY_CODE:
    throw NotImplementedException(std::string((char*)reply.reply_text.bytes, reply.reply_text.len), reply.class_id, reply.method_id);
  case InternalErrorException::REPLY_CODE:
    throw InternalErrorException(std::string((char*)reply.reply_text.bytes, reply.reply_text.len), reply.class_id, reply.method_id);
  default:
    throw std::logic_error(std::string("Programming error: unknown connection reply code: ").append(boost::lexical_cast<std::string>(reply.reply_code)));
  }
}

AmqpException::AmqpException(const std::string& reply_text, boost::uint16_t class_id, boost::uint16_t method_id) throw() :
      std::runtime_error(std::string(amqp_method_name((class_id << 16) | method_id)).append(" caused: ").append(reply_text)),
      m_reply_text(reply_text),
      m_class_id(class_id),
      m_method_id(method_id)
      {}
}
