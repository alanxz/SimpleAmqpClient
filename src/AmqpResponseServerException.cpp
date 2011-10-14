
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
#include <assert.h>
#include <sstream>

namespace AmqpClient {

AmqpResponseServerException::AmqpResponseServerException(const amqp_rpc_reply_t& reply, const std::string& context) throw() :
  std::runtime_error(context), m_context(context)
{
  assert(reply.reply_type == AMQP_RESPONSE_SERVER_EXCEPTION);

  switch (reply.reply.id)
  {
  case AMQP_CONNECTION_CLOSE_METHOD:
  {
    InitializeFromConnectionClose(*(reinterpret_cast<amqp_connection_close_t*>(reply.reply.decoded)));
    break;
  }
  case AMQP_CHANNEL_CLOSE_METHOD:
  {
    InitializeFromChannelClose(*(reinterpret_cast<amqp_channel_close_t*>(reply.reply.decoded)));
    break;
  }
  }
}

AmqpResponseServerException::AmqpResponseServerException(const amqp_channel_close_t& reply, const std::string& context) throw() :
  std::runtime_error(context), m_context(context)
{
  InitializeFromChannelClose(reply);
}

AmqpResponseServerException::AmqpResponseServerException(const amqp_connection_close_t& reply, const std::string& context) throw() :
  std::runtime_error(context), m_context(context)
{
  InitializeFromConnectionClose(reply);
}

void AmqpResponseServerException::InitializeFromConnectionClose(const amqp_connection_close_t& reply)
{
  m_type = ET_ChannelException;
  m_class_id = reply.class_id;
  m_method_id = reply.method_id;
  m_reply_code = reply.reply_code;
  m_reply_text = std::string((char*)reply.reply_text.bytes, reply.reply_text.len);
}

void AmqpResponseServerException::InitializeFromChannelClose(const amqp_channel_close_t& reply)
{
  m_type = ET_ChannelException;
  m_class_id = reply.class_id;
  m_method_id = reply.method_id;
  m_reply_code = reply.reply_code;
  m_reply_text = std::string((char*)reply.reply_text.bytes, reply.reply_text.len);
}


AmqpResponseServerException::AmqpResponseServerException(const AmqpResponseServerException& e) throw() :
  std::runtime_error(e), m_reply_text(e.m_reply_text), m_context(e.m_context), m_type(e.m_type),
    m_class_id(e.m_class_id), m_method_id(e.m_method_id), m_reply_code(m_reply_code)
{
}

AmqpResponseServerException& AmqpResponseServerException::operator=(const AmqpResponseServerException& e) throw()
{
	if (this == &e)
	{
		return *this;
	}

  std::runtime_error::operator=(e);

  m_reply_text = e.m_reply_text;
  m_context = e.m_context;
  m_reply_code = e.m_reply_code;
  m_type = e.m_type;
  m_class_id = e.m_class_id;
  m_method_id = e.m_method_id;

	return *this;
}

AmqpResponseServerException::~AmqpResponseServerException() throw()
{
}

const char* AmqpResponseServerException::what() const throw()
{
	std::ostringstream oss;
	oss << m_context;

	switch (m_type)
	{
  case ET_ConnectionException:
		{
		  oss << ": Server connection error: " << exception_code() << " status: " 
          << exception_message();
	  }
		break;

	case ET_ChannelException:
		{
			oss << ": Server channel error: " << exception_code() << " status: " 
				<< exception_message();
		}
		break;

		default:
			oss << ": Unknown server error";
	}
  const_cast<std::string&>(m_what) = oss.str();
  return m_what.c_str();
}

} // namespace AmqpClient
