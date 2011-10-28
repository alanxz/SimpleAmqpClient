#ifndef AMQPRESPONSESERVEREXCEPTION_H
#define AMQPRESPONSESERVEREXCEPTION_H

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

#include "SimpleAmqpClient/Util.h"

#include <boost/cstdint.hpp>
#include <string>
#include <stdexcept>

#ifdef _MSC_VER
# pragma warning ( push )
# pragma warning ( disable: 4251 )
#endif

struct amqp_rpc_reply_t_;
struct amqp_channel_close_t_;
struct amqp_connection_close_t_;

namespace AmqpClient {

class SIMPLEAMQPCLIENT_EXPORT AmqpResponseServerException : public std::runtime_error
{
public:
  enum ExceptionType
  {
    ET_Unknown,
    ET_ChannelException,
    ET_ConnectionException
  };

	explicit AmqpResponseServerException(const amqp_rpc_reply_t_& reply, const std::string& context) throw();
  AmqpResponseServerException(const amqp_channel_close_t_& reply, const std::string& context) throw();
  AmqpResponseServerException(const amqp_connection_close_t_& reply, const std::string& context) throw();

  void InitializeFromConnectionClose(const amqp_connection_close_t_& reply);
  void InitializeFromChannelClose(const amqp_channel_close_t_& reply);

	AmqpResponseServerException(const AmqpResponseServerException& e) throw();
	AmqpResponseServerException& operator=(const AmqpResponseServerException& e) throw();

	virtual ~AmqpResponseServerException() throw();

	virtual const char* what() const throw();

  uint16_t class_id() const throw() { return m_class_id; }
  uint16_t method_id() const throw() { return m_method_id; }
  uint16_t reply_code() const throw() { return m_reply_code; }
  std::string reply_text() const throw() { return m_reply_text; }

  ExceptionType exception_type() const throw() { return m_type; }

  int exception_code() const throw() { return m_reply_code; }
  std::string exception_message() const throw() { return m_reply_text; }

private:
  std::string m_reply_text;
  std::string m_context;
  mutable std::string m_what;
  ExceptionType m_type;
  uint16_t m_class_id;
  uint16_t m_method_id;
  uint16_t m_reply_code;
};

} // namespace AmqpClient

#ifdef _MSC_VER
# pragma warning ( pop )
#endif

#endif // AMQPRESPONSESERVEREXCEPTION_H
