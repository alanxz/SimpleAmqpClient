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
/**
  * A single channel
  * Represents a logical AMQP channel over a connection
  */
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

	/**
	  * Declares an exchange
	  * Creates an exchange on the AMQP broker if it does not already exist
	  * @param exchange_name the name of the exchange
	  * @param exchange_type the type of exchange to be declared. Defaults to amp.direct 
	  *  other types that could be used: fanout and topic
	  * @param passive Indicates how the broker should react if the exchange does not exist.
	  *  If passive is true and the exhange does not exist the broker will respond with an error and
	  *  not create the exchange, exchange is created otherwise. Defaults to false (exchange is created
	  *  if it does not already exist)
	  * @param durable Indicates whether the exchange is durable - e.g., will it survive a broker restart
	  *  Defaults to false
	  * @param auto_delete Indicates whether the exchange will automatically be removed when no queues are 
	  *  bound to it. Defaults to true
	  */
    void DeclareExchange(const std::string& exchange_name,
                         const std::string& exchange_type = "amq.direct",
                         bool passive = false,
                         bool durable = false,
                         bool auto_delete = true);

	/**
	  * Declares a queue
	  * Creates a queue on the AMQP broker if it does not already exist
	  * @param queue_name the desired name of the queue. If this is a zero-length string the broker
	  *  will generate a queue name and it will be returned as a result from this metho
	  * @param passive Indicated how the broker should react if the queue does not exist.
	  *  If passive is true and the queue does not exist the borker will respond with an error and
	  *  not create the queue, the queue is created otherwise. Defaults to false (queue is created if it
	  *  does not already exist)
	  * @param exclusive Indicates that only client can use the queue. Defaults to true
	  * @param auto_delete Indicates whether the queue will automatically be removed when no clients are
	  *  connected to it. Defaults to true
	  * @returns the name of the queue created on the broker. Used mostly when the broker is asked to 
	  *  create a unique queue by not providing a queue name
	  */
	std::string DeclareQueue(const std::string& queue_name,
              		         bool passive = false,
							 bool durable = false,
							 bool exclusive = true,
							 bool auto_delete = true);

	/**
	  * Deletes a queue
	  * Removes a queue from the broker. There is no indication of whether the queue was actually deleted
	  * on the broker.
	  * @param queue_name the name of the queue to remove
	  * @param if_unused only deletes the queue if the queue is not bound to an exchange. Defaults to false.
	  * @param if_empty only deletes the queue if the queue is empty. Defaults to false.
	  */
    void DeleteQueue(const std::string& queue_name,
                     bool if_unused = false,
                     bool if_empty = false);

	/**
	  * Binds a queue to an exchange
	  * Connects an exchange to a queue on the broker
	  * @param queue_name the name of the queue to bind
	  * @param exchange_name the name of the exchange to bind
	  * @param routing_key only messages sent to the exchange with this routing key will be delivered to
	  *  the queue. Defaults to "" which means all messages will be delivered
	  */
    void BindQueue(const std::string& queue_name,
                   const std::string& exchange_name,
                   const std::string& routing_key = "");

	/**
	  * Unbinds a queue from an exchange
	  * Disconnects an exchange from a queue
	  * @param queue_name the name of the queue to unbind
	  * @param exchange_name the name of the exchange to unbind
	  * @param routing_key this must match the routing_key used when creating the binding
	  * @see BindQueue
	  */
    void UnbindQueue(const std::string& queue_name,
                     const std::string& exchange_name,
                     const std::string& binding_key = "");

	/**
	  * Purges a queue
	  * Removes all the messages in a queue on the broker
	  * @param queue_name the name of the queue to empty
	  * @param no_wait whether to do it synchronously (no_wait = false) or asynchronously (no_wait = true)
	  *  Default is false (synchonous)
	  */
    void PurgeQueue(const std::string& queue_name,
                    bool no_wait = false);

	/**
	  * Acknowledges a Basic message
	  * Acknowledges a message delievered using BasicGet or BasicConsume
	  * @param message the message that is being ack'ed
	  */
	void BasicAck(const BasicMessage::ptr_t message);

	/**
	  * Acknowledges a Basic message
	  * Acknowledges a message delievered using BasicGet or BasicConsume
	  * @param delivery_tag the delivery tage of the message being acknowledged
	  */
	void BasicAck(uint64_t delivery_tag);

	/**
	  * Publishes a Basic message
	  * Publishes a Basic message to an exchange
	  * @param exchange_name The name of the exchange to publish the message to
	  * @param routing_key The routing key to publish with, this is used to route to the correct queue
	  * @param message the BasicMessage object to publish to the queue
	  * @param mandatory Must deliver to a client. If it cannot be delievered immediatatly fail. Defaults
	  *  to false
	  * @param immediate Immediately deliver to a client. If it cannot be immediately Consumed by a client
	  *  fail. Defaults to false.
	  * 
	  * I am unsure what the difference between the mandatory and immediate flags are in this context
	  */
    void BasicPublish(const std::string& exchange_name,
                      const std::string& routing_key,
                      const BasicMessage::ptr_t message,
                      bool mandatory = false,
                      bool immediate = false);

	/**
	  * Starts consuming Basic messages on a queue
	  * Subscribes as a consumer to a queue, so all future messages on a queue will be Basic.Delivered
	  * Note: due to a limitation to how things are done, it is only possible to reliably have a single
	  *  consumer per channel, calling this more than once per channel may result in undefined results
	  *  from BasicConsumeMessage
	  * @param queue the name of the queue to subscribe to
	  * @param consumer_tag the name of the consumer. This is used to cancel the consumer subscription
	  * @param no_local Defaults to true
	  * @param no_ack If true, ack'ing the message is automatically done when the message is delivered.
	  *  Defaults to true
	  * @param exclusive Defaults to true
	  */
	void BasicConsume(const std::string& queue,
					  const std::string& consumer_tag,
					  bool no_local = true,
					  bool no_ack = true,
					  bool exclusive = true);

	/**
	  * Cancels a previously created Consumer
	  * Unsubscribes as a consumer to a queue. In otherwords undoes what BasicConsume does.
	  * @param consumer_tag The same consumer_tag used when the consumer was created with BasicConsume
	  * @see BasicConsume
	  */
	void BasicCancel(const std::string& consumer_tag);

	/**
	  * Consumes a single message
	  * Waits for a single Basic message to be Delivered. This function only works after BasicConsume
	  * has successfully been called.
	  * @returns The next message on the queue
	  */
	BasicMessage::ptr_t BasicConsumeMessage();

protected:
    amqp_connection_state_t m_connection;
    amqp_channel_t m_channel;
    amqp_table_t m_empty_table;

};

} // namespace AmqpClient

#endif // CHANNEL_H
