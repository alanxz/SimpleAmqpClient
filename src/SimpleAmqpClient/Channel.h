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

#include "SimpleAmqpClient/BasicMessage.h"
#include "SimpleAmqpClient/Envelope.h"
#include "SimpleAmqpClient/Util.h"

#include <boost/cstdint.hpp>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <amqp.h>
#include <string>

#define BROKER_HEARTBEAT 0
#define DEFAULT_CHANNEL 1

#ifdef _MSC_VER
# pragma warning ( push )
# pragma warning ( disable: 4251 4275 )
#endif


namespace AmqpClient {

class Connection;
/**
  * A single channel
  * Represents a logical AMQP channel over a connection
  */
class SIMPLEAMQPCLIENT_EXPORT Channel : boost::noncopyable
{
public:
	typedef boost::shared_ptr<Channel> ptr_t;
	friend ptr_t boost::make_shared<Channel>(std::string const & a1, int const & a2,
			std::string const & a3, std::string const & a4,
			std::string const & a5, int const & a6);

	static const std::string EXCHANGE_TYPE_DIRECT;
	static const std::string EXCHANGE_TYPE_FANOUT;
	static const std::string EXCHANGE_TYPE_TOPIC;

	/**
	  * Creates a new channel object
	  * Creates a new connection to an AMQP broker using the supplied parameters and opens
	  * a single channel for use
	  * @param host The hostname or IP address of the AMQP broker
	  * @param port The port to connect to the AMQP broker on
	  * @param username The username used to authenticate with the AMQP broker
	  * @param password The password corresponding to the username used to authenticate with the AMQP broker
	  * @param vhost The virtual host on the AMQP we should connect to
	  * @param channel_max Request that the server limit the number of channels for
	  * this connection to the specified parameter, a value of zero will use the broker-supplied value
	  * @param frame_max Request that the server limit the maximum size of any frame to this value
	  * @return a new Channel object pointer
	  */
	static ptr_t Create(const std::string& host = "127.0.0.1",
						int port = 5672,
						const std::string& username = "guest",
						const std::string& password = "guest",
						const std::string& vhost = "/",
						int frame_max = 131072)
	{
		return boost::make_shared<Channel>(host, port, username, password, vhost, frame_max);
	}

private:
	explicit Channel(const std::string& host,
			   int port,
			   const std::string& username,
			   const std::string& password,
			   const std::string& vhost,
			   int frame_max);
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
      * Deletes an exachange on the AMQP broker
      * @param exchange_name the name of the exchange to be deleteed
      * @param if_unused if true only delete the exchange if it has no queues bound to it
      * @param nowait if true do not wait for the exchange to become unused before deleting it
      */
    void DeleteExchange(const std::string& exchange_name,
                        bool if_unused = false,
                        bool nowait = true);

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
    * @throws MessageReturnedException if a basic.return is received while waiting for a message
	  * @returns The next message on the queue
	  */
	BasicMessage::ptr_t BasicConsumeMessage();

	/**
	  * Consumes a single message with a timeout
	  * Waits for a single Basic message to be Delivered or the timeout to expire.
	  * This function only works after BasicConsume as been successfully called.
	  * @param message the message object to save it to. Is ok to be an empty pointer
	  * @param timeout the timeout for the first part of the message to be delivered in ms
    * @throws MessageReturnedException if a basic.return is received while waiting for a message
	  * @returns true if a message was delivered before the timeout, false otherwise
	  */
	bool BasicConsumeMessage(BasicMessage::ptr_t& message, int timeout);

	/**
	  * Consumes a single message with a timeout (this gets an envelope object)
	  * Waits for a single Basic message to be Delivered or the timeout to expire.
	  * This function only works after BasicConsume as been successfully called.
    * This function differs in that it returns an envelope object which contains more information
    * about the message delivered
	  * @param envelope the message object to save it to. Is ok to be an empty pointer
	  * @param timeout the timeout for the first part of the message to be delivered in ms
    * @throws MessageReturnedException if a basic.return is received while waiting for a message
	  * @returns true if a message was delivered before the timeout, false otherwise
	  */
	bool BasicConsumeMessage(Envelope::ptr_t& envelope, int timeout);

  /**
    * Closes the current channel and reopens immediately opens a new channel
    * Note this will destroy anything that is bound to a channel (consumers, auto-delete queues, and auto-delete exchanges
    * This is required after a AmqpResponseServerException with ExceptionType = ET_ChannelException
    * all other functions will fail otherwise.
    */
  void ResetChannel();

protected:
	/**
	  * Checks the result of a librabbitmq-c call that returns a amqp_rpc_reply_t
	  *
	  * Checks the result of a librabbitmq-c call that returns an
	  * amqp_rpc_reply_t struct and throws an exception if it fails.
	  */
    void CheckRpcReply(amqp_rpc_reply_t& reply, const std::string& context = "");
	
	/**
	  * Checks the result of a librabbitmq-c that doesn't return a amqp_rpc_reply_t
	  *
	  * Checks the result of a librabbitmq-c call that doesn't return an
	  * amqp_rpc_reply_t struct, but the result is stored as a part of the
	  * amqp_connection_state_t, and is checked with amqp_check_last_rpc()
	  * Throws an exception if an error condition is detected
	  */
	void CheckLastRpcReply(amqp_connection_state_t& connection, const std::string& context);
	/**
	  * Checks the result of a librabbitmq-c call that returns an int
	  *
	  * Checks the result of a librabbitmq-c call that returns an int, 
	  * throws an exception if an error condition is detected.
	  */
    void CheckForError(int ret, const std::string& context = "");

    void CheckFrameForClose(amqp_frame_t& frame);
    void CheckChannelIsOpen();

    void FinishCloseChannel();
    void FinishCloseConnection();

    BasicMessage::ptr_t Channel::ReadContent();

    amqp_connection_state_t m_connection;
    amqp_channel_t m_channel;

    bool m_connection_ok;
    bool m_channel_ok;

    static const amqp_table_t EMPTY_TABLE;

};

} // namespace AmqpClient

#ifdef _MSC_VER
# pragma warning ( pop )
#endif

#endif // CHANNEL_H
