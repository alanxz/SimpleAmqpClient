
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

#include "SimpleAmqpClient/AmqpResponseServerException.h"

#include <boost/cstdint.hpp>
#include <amqp.h>
#include <amqp_framing.h>
#include <sstream>

namespace AmqpClient {

AmqpResponseServerException::AmqpResponseServerException(const amqp_rpc_reply_t& reply, const std::string& context) throw() :
	m_reply(reply), m_context(context)
{
}

AmqpResponseServerException::AmqpResponseServerException(const AmqpResponseServerException& e) throw() :
	m_reply(e.m_reply), m_what(e.m_what)
{
}
AmqpResponseServerException& AmqpResponseServerException::operator=(const AmqpResponseServerException& e) throw()
{
	if (this == &e)
	{
		return *this;
	}

	m_reply = e.m_reply;
	m_what = e.m_what;
	return *this;
}

AmqpResponseServerException::~AmqpResponseServerException() throw()
{
}

const char* AmqpResponseServerException::what() const throw()
{
	std::ostringstream oss;
	oss << m_context;

	switch (m_reply.reply.id)
	{
		case AMQP_CONNECTION_CLOSE_METHOD:
			{
				oss << ": Server connection error: " << exception_code() << " status: " 
          << exception_message();
			}
			break;

		case AMQP_CHANNEL_CLOSE_METHOD:
			{
				oss << ": Server channel error: " << exception_code() << " status: " 
					<< exception_message();
			}
			break;

		default:
			oss << ": Unknown server error, method: " << m_reply.reply.id;
	}
  const_cast<std::string&>(m_what) = oss.str();
  return m_what.c_str();
}

AmqpResponseServerException::ExceptionType AmqpResponseServerException::exception_type() const throw()
{
  switch (m_reply.reply.id)
  {
  case AMQP_CONNECTION_CLOSE_METHOD:
    return ET_ConnectionException;
  case AMQP_CHANNEL_CLOSE_METHOD:
    return ET_ChannelException;
  default:
    return ET_Unknown;
  }
}

int AmqpResponseServerException::exception_code() const throw()
{
  switch (m_reply.reply.id)
  {
  case AMQP_CONNECTION_CLOSE_METHOD:
    {
				amqp_connection_close_t* msg = reinterpret_cast<amqp_connection_close_t*>(m_reply.reply.decoded);
        return msg->reply_code;
    }
  case AMQP_CHANNEL_CLOSE_METHOD:
    {
      amqp_channel_close_t* msg = reinterpret_cast<amqp_channel_close_t*>(m_reply.reply.decoded);
      return msg->reply_code;
    }
  default:
    return 0;
  }
}

std::string AmqpResponseServerException::exception_message() const throw()
{
  switch (m_reply.reply.id)
  {
  case AMQP_CONNECTION_CLOSE_METHOD:
    {
      amqp_connection_close_t* msg = reinterpret_cast<amqp_connection_close_t*>(m_reply.reply.decoded);
      return std::string((char*)msg->reply_text.bytes, msg->reply_text.len);
    }
  case AMQP_CHANNEL_CLOSE_METHOD:
    {
      amqp_channel_close_t* msg = reinterpret_cast<amqp_channel_close_t*>(m_reply.reply.decoded);
      return std::string((char*)msg->reply_text.bytes, msg->reply_text.len);
    }
  default:
    return std::string();
  }
}

} // namespace AmqpClient
