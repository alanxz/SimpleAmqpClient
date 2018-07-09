#ifndef SIMPLEAMQPCLIENT_CHANNEL_H
#define SIMPLEAMQPCLIENT_CHANNEL_H
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

#include "SimpleAmqpClient/BasicMessage.h"
#include "SimpleAmqpClient/Envelope.h"
#include "SimpleAmqpClient/Table.h"
#include "SimpleAmqpClient/Util.h"

#include <boost/cstdint.hpp>
#include <boost/make_shared.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <string>
#include <vector>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251 4275)
#endif

namespace AmqpClient {

namespace Detail {
class ChannelImpl;
}

/**
  * A single channel
  * Represents a logical AMQP channel over a connection
  */
class SIMPLEAMQPCLIENT_EXPORT Channel : boost::noncopyable {
 public:
  typedef boost::shared_ptr<Channel> ptr_t;

  static const std::string EXCHANGE_TYPE_DIRECT;
  static const std::string EXCHANGE_TYPE_FANOUT;
  static const std::string EXCHANGE_TYPE_TOPIC;

  /**
    * Creates a new channel object
    * Creates a new connection to an AMQP broker using the supplied parameters
   * and opens
    * a single channel for use
    * @param host The hostname or IP address of the AMQP broker
    * @param port The port to connect to the AMQP broker on
    * @param username The username used to authenticate with the AMQP broker
    * @param password The password corresponding to the username used to
   * authenticate with the AMQP broker
    * @param vhost The virtual host on the AMQP we should connect to
    * @param channel_max Request that the server limit the number of channels
   * for
    * this connection to the specified parameter, a value of zero will use the
   * broker-supplied value
    * @param frame_max Request that the server limit the maximum size of any
   * frame to this value
    * @return a new Channel object pointer
    */
  static ptr_t Create(const std::string &host = "127.0.0.1", int port = 5672,
                      const std::string &username = "guest",
                      const std::string &password = "guest",
                      const std::string &vhost = "/", int frame_max = 131072) {
    return boost::make_shared<Channel>(host, port, username, password, vhost,
                                       frame_max);
  }

 protected:
  struct SSLConnectionParams {
    std::string path_to_ca_cert;
    std::string path_to_client_key;
    std::string path_to_client_cert;
    bool verify_hostname;
  };

 public:
  /**
  * Creates a new channel object
  * Creates a new connection to an AMQP broker using the supplied parameters and
  * opens
  * a single channel for use
  * @param path_to_ca_cert Path to ca certificate file
  * @param host The hostname or IP address of the AMQP broker
  * @param path_to_client_key Path to client key file
  * @param path_to_client_cert Path to client certificate file
  * @param port The port to connect to the AMQP broker on
  * @param username The username used to authenticate with the AMQP broker
  * @param password The password corresponding to the username used to
  * authenticate with the AMQP broker
  * @param vhost The virtual host on the AMQP we should connect to
  * @param channel_max Request that the server limit the number of channels for
  * this connection to the specified parameter, a value of zero will use the
  * broker-supplied value
  * @param frame_max Request that the server limit the maximum size of any frame
  * to this value
  * @param verify_host Verify the hostname against the certificate when
  * opening the SSL connection.
  *
  * @return a new Channel object pointer
  */

  static ptr_t CreateSecure(const std::string &path_to_ca_cert = "",
                            const std::string &host = "127.0.0.1",
                            const std::string &path_to_client_key = "",
                            const std::string &path_to_client_cert = "",
                            int port = 5671,
                            const std::string &username = "guest",
                            const std::string &password = "guest",
                            const std::string &vhost = "/",
                            int frame_max = 131072,
                            bool verify_hostname = true) {
    SSLConnectionParams ssl_params;
    ssl_params.path_to_ca_cert = path_to_ca_cert;
    ssl_params.path_to_client_key = path_to_client_key;
    ssl_params.path_to_client_cert = path_to_client_cert;
    ssl_params.verify_hostname = verify_hostname;

    return boost::make_shared<Channel>(host, port, username, password, vhost,
                                       frame_max, ssl_params);
  }

  /**
   * Create a new Channel object from an AMQP URI
   *
   * @param uri [in] a URI of the form:
   * amqp://[username:password@]{HOSTNAME}[:PORT][/VHOST]
   * @param frame_max [in] requests that the broker limit the maximum size of
   * any frame to this value
   * @returns a new Channel object
   */
  static ptr_t CreateFromUri(const std::string &uri, int frame_max = 131072);

  /**
   * Create a new Channel object from an AMQP URI, secured with SSL.
   * If URI should start with amqps://
   *
   * @param uri [in] a URI of the form:
   * amqp[s]://[username:password@]{HOSTNAME}[:PORT][/VHOST]
   * @param path_to_ca_cert Path to ca certificate file
   * @param host The hostname or IP address of the AMQP broker
   * @param path_to_client_key Path to client key file
   * @param path_to_client_cert Path to client certificate file
   * @param verify_hostname Verify the hostname against the certificate when
   * opening the SSL connection.
   * @param frame_max [in] requests that the broker limit the maximum size of
   * any frame to this value
   * @returns a new Channel object
   */
  static ptr_t CreateSecureFromUri(const std::string &uri,
                                   const std::string &path_to_ca_cert,
                                   const std::string &path_to_client_key = "",
                                   const std::string &path_to_client_cert = "",
                                   bool verify_hostname = true,
                                   int frame_max = 131072);

  explicit Channel(const std::string &host, int port,
                   const std::string &username, const std::string &password,
                   const std::string &vhost, int frame_max);

  explicit Channel(const std::string &host, int port,
                   const std::string &username, const std::string &password,
                   const std::string &vhost, int frame_max,
                   const SSLConnectionParams &ssl_params);

 public:
  virtual ~Channel();

  /**
    * Exposes the underlying socket handle
    * @returns file descriptor number associated with the connection socket
    *
    * @warning This function exposes an internal implementation detail
    * of SimpleAmqpClient. Manipulating the socket descriptor will result in
    * undefined behavior of the library. Additionally SimpleAmqpClient's use of
    * the socket will change depending on what version of rabbitmq-c and
    * SimpleAmqpClient are used. Test carefully before depending on any specific
    * behavior.
    */
  int GetSocketFD() const;

  /**
    * Declares an exchange
    * Creates an exchange on the AMQP broker if it does not already exist
    * @param exchange_name the name of the exchange
    * @param exchange_type the type of exchange to be declared. Defaults to
   * direct
    *  other types that could be used: fanout and topic
    * @param passive Indicates how the broker should react if the exchange does
   * not exist.
    *  If passive is true and the exhange does not exist the broker will respond
   * with an error and
    *  not create the exchange, exchange is created otherwise. Defaults to false
   * (exchange is created
    *  if it does not already exist)
    * @param durable Indicates whether the exchange is durable - e.g., will it
   * survive a broker restart
    *  Defaults to false
    * @param auto_delete Indicates whether the exchange will automatically be
   * removed when no queues are
    *  bound to it. Defaults to false
    */
  void DeclareExchange(
      const std::string &exchange_name,
      const std::string &exchange_type = Channel::EXCHANGE_TYPE_DIRECT,
      bool passive = false, bool durable = false, bool auto_delete = false);

  /**
    * Declares an exchange
    * Creates an exchange on the AMQP broker if it does not already exist
    * @param exchange_name the name of the exchange
    * @param exchange_type the type of exchange to be declared. Defaults to
  * direct
    *  other types that could be used: fanout and topic
    * @param passive Indicates how the broker should react if the exchange does
  * not exist.
    *  If passive is true and the exchange does not exist the broker will
  * respond with an error and
    *  not create the exchange, exchange is created otherwise.
    * @param durable Indicates whether the exchange is durable - e.g., will it
  * survive a broker restart
    * @param auto_delete Indicates whether the exchange will automatically be
  * removed when no queues are
    *  bound to it.
  * @param arguments A table of additional arguments used when creating the
  * exchange
    */
  void DeclareExchange(const std::string &exchange_name,
                       const std::string &exchange_type, bool passive,
                       bool durable, bool auto_delete, const Table &arguments);

  /**
    * Deletes an exachange on the AMQP broker
    * @param exchange_name the name of the exchange to be deleteed
    * @param if_unused if true only delete the exchange if it has no queues
   * bound to it, throws
    *  AmqpResponseServerExeception otherwise
    */
  void DeleteExchange(const std::string &exchange_name, bool if_unused = false);

  /**
    * Binds one exchange to another exchange using a given key
    * @param destination the name of the exchange to route messages to
    * @param source the name of the exchange to route messages from
    * @param routing_key the routing key to use when binding
    */
  void BindExchange(const std::string &destination, const std::string &source,
                    const std::string &routing_key);

  /**
    * Binds one exchange to another exchange using a given key
    * @param destination the name of the exchange to route messages to
    * @param source the name of the exchange to route messages from
    * @param routing_key the routing key to use when binding
    * @param arguments A table of additional arguments used when creating the
   * binding
    */
  void BindExchange(const std::string &destination, const std::string &source,
                    const std::string &routing_key, const Table &arguments);

  /**
    * Unbind an existing exchange-exchange binding
    * @see BindExchange
    * @param destination the name of the exchange to route messages to
    * @param source the name of the exchange to route messages from
    * @param routing_key the routing key to use when binding
    */
  void UnbindExchange(const std::string &destination, const std::string &source,
                      const std::string &routing_key);

  /**
    * Unbind an existing exchange-exchange binding
    * @see BindExchange
    * @param destination the name of the exchange to route messages to
    * @param source the name of the exchange to route messages from
    * @param routing_key the routing key to use when binding
    * @param arguments A table of additional arguments when unbinding the
   * exchange
    */
  void UnbindExchange(const std::string &destination, const std::string &source,
                      const std::string &routing_key, const Table &arguments);
  /**
    * Declares a queue
    * Creates a queue on the AMQP broker if it does not already exist
    * @param queue_name the desired name of the queue. If this is a zero-length
  * string the broker
    *  will generate a queue name and it will be returned as a result from this
  * method
    * @param passive Indicated how the broker should react if the queue does not
  * exist.
    *  If passive is true and the queue does not exist the borker will respond
  * with an error and
    *  not create the queue, the queue is created otherwise. Defaults to false
  * (queue is created if it
    *  does not already exist)
    * @param durable Indicates whether the exchange is durable - e.g., will it
  * survive a broker restart
    *  Defaults to false
    * @param exclusive Indicates that only client can use the queue. Defaults to
  * true. An
  *  exclusive queue is deleted when the connection is closed
    * @param auto_delete the queue will be deleted after at least one exchange
  * has been bound to it,
  *  then has been unbound
    * @returns the name of the queue created on the broker. Used mostly when the
  * broker is asked to
    *  create a unique queue by not providing a queue name
    */
  std::string DeclareQueue(const std::string &queue_name, bool passive = false,
                           bool durable = false, bool exclusive = true,
                           bool auto_delete = true);

  /**
    * Declares a queue
    * Creates a queue on the AMQP broker if it does not already exist
    * @param queue_name the desired name of the queue. If this is a zero-length
  * string the broker
    *  will generate a queue name and it will be returned as a result from this
  * method
    * @param passive Indicated how the broker should react if the queue does not
  * exist.
    *  If passive is true and the queue does not exist the borker will respond
  * with an error and
    *  not create the queue, the queue is created otherwise. Defaults to false
  * (queue is created if it
    *  does not already exist)
    * @param durable Indicates whether the exchange is durable - e.g., will it
  * survive a broker restart
    *  Defaults to false
    * @param exclusive Indicates that only client can use the queue. Defaults to
  * true. An
  *  exclusive queue is deleted when the connection is closed
    * @param auto_delete the queue will be deleted after at least one exchange
  * has been bound to it,
  *  then has been unbound
  * @param arguments A table of additional arguments used when declaring a queue
    * @returns the name of the queue created on the broker. Used mostly when the
  * broker is asked to
    *  create a unique queue by not providing a queue name
    */
  std::string DeclareQueue(const std::string &queue_name, bool passive,
                           bool durable, bool exclusive, bool auto_delete,
                           const Table &arguments);

  /**
    * Declares a queue and returns current message- and consumer counts
    * Creates a queue on the AMQP broker if it does not already exist
    * @param queue_name the desired name of the queue. If this is a zero-length
   * string the broker
    *  will generate a queue name and it will be returned as a result from this
   * method
    * @param message_count Reference to an unsigned int which will receive the
   * current
    *  number of messages in the declared queue, if any.
    * @param consumer_count Reference to an unsigned int which will receive the
   * current
    *  number of consumers of the declared queue, if any.
    * @param passive Indicated how the broker should react if the queue does not
   * exist.
    *  If passive is true and the queue does not exist the broker will respond
   * with an error and
    *  not create the queue, the queue is created otherwise. Defaults to false
   * (queue is created if it
    *  does not already exist)
    * @param durable Indicates whether the exchange is durable - e.g., will it
   * survive a broker restart
    *  Defaults to false
    * @param exclusive Indicates that only client can use the queue. Defaults to
   * true. An
    *  exclusive queue is deleted when the connection is closed
    * @param auto_delete the queue will be deleted after at least one exchange
   * has been bound to it,
    *  then has been unbound
    * @returns the name of the queue created on the broker. Used mostly when the
   * broker is asked to
    *  create a unique queue by not providing a queue name
    */
  std::string DeclareQueueWithCounts(const std::string &queue_name,
                                     boost::uint32_t &message_count,
                                     boost::uint32_t &consumer_count,
                                     bool passive = false, bool durable = false,
                                     bool exclusive = true,
                                     bool auto_delete = true);

  /**
    * Declares a queue and returns current message- and consumer counts
    * Creates a queue on the AMQP broker if it does not already exist
    * @param queue_name the desired name of the queue. If this is a zero-length
   * string the broker
    *  will generate a queue name and it will be returned as a result from this
   * method
    * @param message_count Reference to an unsigned int which will receive the
   * current
    *  number of messages in the declared queue, if any.
    * @param consumer_count Reference to an unsigned int which will receive the
   * current
    *  number of consumers of the declared queue, if any.
    * @param passive Indicated how the broker should react if the queue does not
   * exist.
    *  If passive is true and the queue does not exist the broker will respond
   * with an error and
    *  not create the queue, the queue is created otherwise. Defaults to false
   * (queue is created if it
    *  does not already exist)
    * @param durable Indicates whether the exchange is durable - e.g., will it
   * survive a broker restart
    *  Defaults to false
    * @param exclusive Indicates that only client can use the queue. Defaults to
   * true. An
    *  exclusive queue is deleted when the connection is closed
    * @param auto_delete the queue will be deleted after at least one exchange
   * has been bound to it,
    *  then has been unbound
    * @param arguments A table of additional arguments used when declaring a
   * queue
    * @returns the name of the queue created on the broker. Used mostly when the
   * broker is asked to
    *  create a unique queue by not providing a queue name
    */
  std::string DeclareQueueWithCounts(const std::string &queue_name,
                                     boost::uint32_t &message_count,
                                     boost::uint32_t &consumer_count,
                                     bool passive, bool durable, bool exclusive,
                                     bool auto_delete, const Table &arguments);

  /**
    * Deletes a queue
    * Removes a queue from the broker. There is no indication of whether the
   * queue was actually deleted
    * on the broker.
    * @param queue_name the name of the queue to remove
    * @param if_unused only deletes the queue if the queue doesn't have any
   * active consumers. Defaults to false
    * @param if_empty only deletes the queue if the queue is empty. Defaults to
   * false.
    */
  void DeleteQueue(const std::string &queue_name, bool if_unused = false,
                   bool if_empty = false);

  /**
    * Binds a queue to an exchange
    * Connects an exchange to a queue on the broker
    * @param queue_name the name of the queue to bind
    * @param exchange_name the name of the exchange to bind
    * @param routing_key only messages sent to the exchange with this routing
   * key will be delivered to
    *  the queue. Defaults to "" which means all messages will be delivered
    */
  void BindQueue(const std::string &queue_name,
                 const std::string &exchange_name,
                 const std::string &routing_key = "");

  /**
    * Binds a queue to an exchange
    * Connects an exchange to a queue on the broker
    * @param queue_name the name of the queue to bind
    * @param exchange_name the name of the exchange to bind
    * @param routing_key only messages sent to the exchange with this routing
  * key will be delivered to
    *  the queue. Defaults to "" which means all messages will be delivered
  * @param arguments A table of additional arguments used when binding the queue
    */
  void BindQueue(const std::string &queue_name,
                 const std::string &exchange_name,
                 const std::string &routing_key, const Table &arguments);

  /**
    * Unbinds a queue from an exchange
    * Disconnects an exchange from a queue
    * @param queue_name the name of the queue to unbind
    * @param exchange_name the name of the exchange to unbind
    * @param routing_key this must match the routing_key used when creating the
   * binding
    * @see BindQueue
    */
  void UnbindQueue(const std::string &queue_name,
                   const std::string &exchange_name,
                   const std::string &routing_key = "");

  /**
    * Unbinds a queue from an exchange
    * Disconnects an exchange from a queue
    * @param queue_name the name of the queue to unbind
    * @param exchange_name the name of the exchange to unbind
    * @param routing_key this must match the routing_key used when creating the
  * binding
  * @param arguments A table of additional arguments used when unbinding a queue
    * @see BindQueue
    */
  void UnbindQueue(const std::string &queue_name,
                   const std::string &exchange_name,
                   const std::string &routing_key, const Table &arguments);

  /**
    * Purges a queue
    * Removes all the messages in a queue on the broker
    * @param queue_name the name of the queue to empty
    */
  void PurgeQueue(const std::string &queue_name);

  /**
    * Acknowledges a Basic message
    * Acknowledges a message delievered using BasicGet or BasicConsume
    * @param message the message that is being ack'ed
    */
  void BasicAck(const Envelope::ptr_t &message);

  /**
   * Acknowledges a Basic message
   * Acknowledges a message delivered using BasicGet or BasicConsume, this
   * overload
   * doesn't require the Envelope object to Acknowledge
   * @param delivery_info
   */
  void BasicAck(const Envelope::DeliveryInfo &info);

  /**
   * Acknowledges a Basic message
   * Acknowledges a message delivered using BasicGet or BasicConsume, this
   * overload
   * doesn't require the Envelope object to Acknowledge
   *
   * Note that ack'ing multiple message is scoped messages delivered on a given AMQP channel.
   * SimpleAmqpClient uses one channel per consumer, so multiple ack means all un-ack'd messages
   * up to and including the current message id for a given consumer.
   *
   * @param delivery_info
   * @param multiple if true, ack all messages up to this delivery tag, if false ack only this delivery tag
   */
  void BasicAck(const Envelope::DeliveryInfo &info, bool multiple);

  /**
    * Reject a Basic message
    * Rejects a message delievered using BasicGet or BasicConsume
    * @param message the message that is being nack'ed
    * @param requeue tells the broker to requeue the message or not
    */
  void BasicReject(const Envelope::ptr_t &message, bool requeue,
                   bool multiple = false);

  /**
   * Reject a Basic message
   * Rejects a message delivered using BasicGet or BasicConsume, this overload
   * doesn't require the Envelope object to Reject
   * @param delivery_info
   * @param requeue tells the broker to requeue the message or not
   */
  void BasicReject(const Envelope::DeliveryInfo &info, bool requeue,
                   bool multiple = false);

  /**
    * Publishes a Basic message
    * Publishes a Basic message to an exchange
    * @param exchange_name The name of the exchange to publish the message to
    * @param routing_key The routing key to publish with, this is used to route
  * to the correct queue
    * @param message the BasicMessage object to publish to the queue
    * @param mandatory requires the message to be delivered to a queue. A
  * MessageReturnedException is thrown
  *  if the message cannot be routed to a queue. Defaults to false
    * @param immediate requires the message to be both routed to a queue, and
  * immediately delivered via a consumer
  *  if the message is not routed, or a consumer cannot immediately deliver the
  * message a MessageReturnedException is
  *  thrown. Defaults to false
    *
    */
  void BasicPublish(const std::string &exchange_name,
                    const std::string &routing_key,
                    const BasicMessage::ptr_t message, bool mandatory = false,
                    bool immediate = false);

  /**
    * Attempts to get a message from a queue in a synchronous manner
    *
    * Note that this function effectively polls the broker for a message, in
   * general
    * better performance is realized using BasicConsume/BasicConsumeMessage.
   * This function
    * will not wait for a message to arrive in a queue, it will return false if
   * the queue
    * is empty.
    * @param message a message envelope pointer that will be populated if a
   * message is
    *  delivered
    * @param queue the name of the queue to try to get the message from
    * @param no_ack if the message does not need to be ack'ed. Default true
    *  (message does not need to be acked)
    * @returns true if a message was delivered, false if the queue was empty
    */
  bool BasicGet(Envelope::ptr_t &message, const std::string &queue,
                bool no_ack = true);

  /**
    * Redeliver any unacknowledged messages delivered from the broker
    * @param consumer the consumer to recover message from
    */
  void BasicRecover(const std::string &consumer);

  /**
    * Starts consuming Basic messages on a queue
    * Subscribes as a consumer to a queue, so all future messages on a queue
  * will be Basic.Delivered
    * Note: due to a limitation to how things are done, it is only possible to
  * reliably have a single
    *  consumer per channel, calling this more than once per channel may result
  * in undefined results
    *  from BasicConsumeMessage
    * @param queue the name of the queue to subscribe to
    * @param consumer_tag the name of the consumer. This is used to do
  * operations with a consumer
    * @param no_local Defaults to true
    * @param no_ack If true, ack'ing the message is automatically done when the
  * message is delivered.
    *  Defaults to true (message does not have to be ack'ed)
    * @param exclusive means only this consumer can access the queue. Defaults
  * to true
  * @param message_prefetch_count number of unacked messages the broker will
  * deliver. Setting this to
  *  more than 1 will allow the broker to deliver messages while a current
  * message is being processed
  *  for example. A value of 0 means no limit. This option is ignored if no_ack
  * = true
  * @returns the consumer tag
    */
  std::string BasicConsume(const std::string &queue,
                           const std::string &consumer_tag = "",
                           bool no_local = true, bool no_ack = true,
                           bool exclusive = true,
                           boost::uint16_t message_prefetch_count = 1);

  /**
    * Starts consuming Basic messages on a queue
    * Subscribes as a consumer to a queue, so all future messages on a queue
  * will be Basic.Delivered
    * Note: due to a limitation to how things are done, it is only possible to
  * reliably have a single
    *  consumer per channel, calling this more than once per channel may result
  * in undefined results
    *  from BasicConsumeMessage
    * @param queue the name of the queue to subscribe to
    * @param consumer_tag the name of the consumer. This is used to do
  * operations with a consumer
    * @param no_local Defaults to true
    * @param no_ack If true, ack'ing the message is automatically done when the
  * message is delivered.
    *  Defaults to true (message does not have to be ack'ed)
    * @param exclusive means only this consumer can access the queue. Defaults
  * to true
  * @param message_prefetch_count number of unacked messages the broker will
  * deliver. Setting this to
  *  more than 1 will allow the broker to deliver messages while a current
  * message is being processed
  *  for example. A value of 0 means no limit. This option is ignored if no_ack
  * = true
  * @param arguments A table of additional arguments when creating the consumer
  * @returns the consumer tag
    */
  std::string BasicConsume(const std::string &queue,
                           const std::string &consumer_tag, bool no_local,
                           bool no_ack, bool exclusive,
                           boost::uint16_t message_prefetch_count,
                           const Table &arguments);

  /**
    * Sets the number of unacknowledged messages that will be delivered
    * by the broker to a consumer. Note this effectively has no effect
    * for consumer with no_ack set
    * @param consumer_tag the conumser tag to adjust the prefect
    * @param message_prefetch_count the number of unacknowledged message the
    *  broker will deliver. A value of 0 means no limit.
    */
  void BasicQos(const std::string &consumer_tag,
                boost::uint16_t message_prefetch_count);

  /**
    * Cancels a previously created Consumer
    * Unsubscribes as a consumer to a queue. In otherwords undoes what
   * BasicConsume does.
    * @param consumer_tag The same consumer_tag used when the consumer was
   * created with BasicConsume
    * @see BasicConsume
    */
  void BasicCancel(const std::string &consumer_tag);

  /**
    * Consumes a single message
    * Waits for a single Basic message to be Delivered. This function only works
   * after BasicConsume
    * has successfully been called.
    * @returns The next message on the queue
    */
  Envelope::ptr_t BasicConsumeMessage(const std::string &consumer_tag);

  /**
   * Consumes a single message from a list of consumers
   * Waits for a single message to be delivered from a list of consumers. This
   * function only works
   * after BasicConsume has been called.
   *
   * @returns the next message delivered from the broker
   */
  Envelope::ptr_t BasicConsumeMessage(
      const std::vector<std::string> &consumer_tags);

  /**
   * Consumes a single message from any open consumers
   * Waits for a message from any consumer open on this Channel object. This
   * function only works
   * after BasicConsume has been called.
   *
   * @returns the next message delivered from the broker
   */
  Envelope::ptr_t BasicConsumeMessage();

  /**
    * Consumes a single message with a timeout (this gets an envelope object)
    * Waits for a single Basic message to be Delivered or the timeout to expire.
    * This function only works after BasicConsume as been successfully called.
  * This function differs in that it returns an envelope object which contains
  * more information
  * about the message delivered
    * @param envelope the message object to save it to. Is ok to be an empty
  * pointer
    * @param timeout the timeout for the first part of the message to be
  * delivered in ms
  * @throws MessageReturnedException if a basic.return is received while waiting
  * for a message
    * @returns true if a message was delivered before the timeout, false
  * otherwise
    */
  bool BasicConsumeMessage(const std::string &consumer_tag,
                           Envelope::ptr_t &envelope, int timeout = -1);

  /**
   * Consumes a single message with a timeout from a list of consumers
   *
   * Waits for a single message to be delivered to one of the listed consumer
   * tags to be
   * delivered or for the timeout to expire. This function only works after
   * BasicConsume has
   * been successfully called.
   *
   * @param consumer_tags [in] a list of the consumer tags to wait for a message
   * from
   * @param envelope [out] the message object that is delivered.
   * @param timeout [in] the timeout in milliseconds for the message to be
   * delivered. 0 works
   *  like a non-blocking read, -1 is an infinite timeout.
   * @returns true if a message was delivered before the timeout, false
   * otherwise.
   */
  bool BasicConsumeMessage(const std::vector<std::string> &consumer_tags,
                           Envelope::ptr_t &envelope, int timeout = -1);

  /**
   * Consumes a single message from any consumers opened for this Channel object
   *
   * Waits for a single message to be delivered to one of the consumers opened
   * on this Channel
   * object to be delivered, or for the timeout to occur. This function only
   * works after BasicConsume
   * has been successfully called.
   *
   * @param envelope [out] the message object that is delivered.
   * @param timeout [in] the timeout in milliseconds for the message to be
   * delivered. 0 works
   * like a non-blocking read, -1 is an infinite timeout.
   * @returns true if a message delivered before the timeout, false otherwise
   */
  bool BasicConsumeMessage(Envelope::ptr_t &envelope, int timeout = -1);

 protected:
  boost::scoped_ptr<Detail::ChannelImpl> m_impl;
};

}  // namespace AmqpClient

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif  // SIMPLEAMQPCLIENT_CHANNEL_H
