#ifndef CHANNEL_H
#define CHANNEL_H

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

#include "BasicMessage.h"

#include <boost/cstdint.hpp>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <amqp.h>
#include <string>

namespace AmqpClient {

class Connection;
class Channel : boost::noncopyable
{
public:
    typedef boost::shared_ptr<Channel> ptr_t;

	friend ptr_t boost::make_shared<Channel>(amqp_connection_state_t const & a1, amqp_channel_t const & a2 );

	static ptr_t Create(amqp_connection_state_t connection, amqp_channel_t channel)
		{ return boost::make_shared<Channel>(connection, channel); }


private:
    explicit Channel(amqp_connection_state_t connection, amqp_channel_t channel_num);

public:
    virtual ~Channel();

    void DeclareExchange(const std::string& exchange_name,
                         const std::string& exchange_type = "amq.direct",
                         bool passive = false,
                         bool durable = false,
                         bool auto_delete = true);

	std::string DeclareQueue(const std::string& queue_name,
              		         bool passive = false,
							 bool durable = false,
							 bool exclusive = true,
							 bool auto_delete = true);

    void DeleteQueue(const std::string& queue_name,
                     bool if_unused = false,
                     bool if_empty = false);

    void BindQueue(const std::string& queue_name,
                   const std::string& exchange_name,
                   const std::string& routing_key = "");

    void UnbindQueue(const std::string& queue_name,
                     const std::string& exchange_name,
                     const std::string& binding_key = "");

    void PurgeQueue(const std::string& queue_name,
                    bool no_wait = false);

	void BasicAck(const BasicMessage::ptr_t message);

	void BasicAck(uint64_t delivery_tag);

    void BasicPublish(const std::string& exchange_name,
                      const std::string& routing_key,
                      const BasicMessage::ptr_t message,
                      bool mandatory = false,
                      bool immediate = false);

	void BasicConsume(const std::string& queue,
					  const std::string& consumer_tag,
					  bool no_local = true,
					  bool no_ack = true,
					  bool exclusive = true);

	void BasicCancel(const std::string& consumer_tag);

	BasicMessage::ptr_t BasicConsumeMessage();

protected:
    amqp_connection_state_t m_connection;
    amqp_channel_t m_channel;
    amqp_table_t m_empty_table;

};

} // namespace AmqpClient

#endif // CHANNEL_H
