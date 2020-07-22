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

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "SimpleAmqpClient/BasicMessage.h"
#include "SimpleAmqpClient/Envelope.h"
#include "SimpleAmqpClient/Table.h"
#include "SimpleAmqpClient/Util.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251 4275)
#endif

/// @file SimpleAmqpClient/Channel.h
/// The AmqpClient::Channel class is defined in this header file.

namespace AmqpClient {

namespace Detail {
class ChannelImpl;
}

/**
 * A single channel multiplexed in an AMQP connection
 *
 * Represents a logical AMQP channel multiplexed over a connection
 */
class SIMPLEAMQPCLIENT_EXPORT Channel {
 public:
  /// a `std::shared_ptr` to Channel
  typedef std::shared_ptr<Channel> ptr_t;

  static const std::string
      EXCHANGE_TYPE_DIRECT;  ///< `"direct"` string constant
  static const std::string
      EXCHANGE_TYPE_FANOUT;                      ///< `"fanout"` string constant
  static const std::string EXCHANGE_TYPE_TOPIC;  ///< `"topic"` string constant

  /**
   * Creates a new channel object
   *
   * Creates a new connection to an AMQP broker using the supplied parameters
   * and opens a single channel for use
   * @param host The hostname or IP address of the AMQP broker
   * @param port The port to connect to the AMQP broker on
   * @param username The username used to authenticate with the AMQP broker
   * @param password The password corresponding to the username used to
   * authenticate with the AMQP broker
   * @param vhost The virtual host on the AMQP we should connect to
   * @param frame_max Request that the server limit the maximum size of any
   * frame to this value
   * @return a new Channel object pointer
   */
  static ptr_t Create(const std::string &host = "127.0.0.1", int port = 5672,
                      const std::string &username = "guest",
                      const std::string &password = "guest",
                      const std::string &vhost = "/", int frame_max = 131072) {
    return std::make_shared<Channel>(host, port, username, password, vhost,
                                     frame_max, false);
  }

  /**
   * Creates a new channel object
   * Creates a new connection to an AMQP broker using the supplied parameters
   * and opens a single channel for use
   * This channel uses the EXTERNAL SASL method for authentication to the broker
   * @param host The hostname or IP address of the AMQP broker
   * @param port The port to connect to the AMQP broker on
   * @param identity The identity used to authenticate with the AMQP broker
   * @param vhost The virtual host on the AMQP we should connect to
   * @param channel_max Request that the server limit the number of channels
   * for
   * this connection to the specified parameter, a value of zero will use the
   * broker-supplied value
   * @param frame_max Request that the server limit the maximum size of any
   * frame to this value
   * @return a new Channel object pointer
   */
  static ptr_t CreateSaslExternal(const std::string &host = "127.0.0.1",
                                  int port = 5672,
                                  const std::string &identity = "guest",
                                  const std::string &vhost = "/",
                                  int frame_max = 131072) {
    return std::make_shared<Channel>(host, port, identity, "", vhost, frame_max,
                                     true);
  }

 protected:
  /// A POD carrier of SSL connection parameters
  struct SSLConnectionParams {
    /// CA certificate filepath
    std::string path_to_ca_cert;
    /// Client key filepath
    std::string path_to_client_key;
    /// Client certificate filepath
    std::string path_to_client_cert;
    /// Whether to ignore server hostname mismatch
    bool verify_hostname;
    /// Wehter to verify the certificate
    bool verify_peer;
  };

 public:
  /**
   * Creates a new channel object, using TLS
   *
   * Creates a new connection to an AMQP broker using the supplied parameters
   * and opens a single channel for use
   * @param path_to_ca_cert Path to CA certificate file
   * @param host The hostname or IP address of the AMQP broker
   * @param path_to_client_key Path to client key file
   * @param path_to_client_cert Path to client certificate file
   * @param port The port to connect to the AMQP broker on
   * @param username The username used to authenticate with the AMQP broker
   * @param password The password corresponding to the username used to
   * authenticate with the AMQP broker
   * @param vhost The virtual host on the AMQP we should connect to
   * @param frame_max Request that the server limit the maximum size of any
   * frame to this value
   * @param verify_hostname Verify the hostname against the certificate when
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
                            bool verify_hostname_and_peer = true) {
    return CreateSecure(path_to_ca_cert, host, path_to_client_key,
                        path_to_client_cert, port, username, password, vhost,
                        frame_max, verify_hostname_and_peer,
                        verify_hostname_and_peer);
  }

  /**
   * Creates a new channel object
   * Creates a new connection to an AMQP broker using the supplied parameters
   * and opens a single channel for use
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
   * @param frame_max Request that the server limit the maximum size of any
   * frame to this value
   * @param verify_host Verify the hostname against the certificate when
   * opening the SSL connection.
   * @param verify_peer Verify the certificate chain that is sent by the broker
   * when opening the SSL connection.
   *
   * @return a new Channel object pointer
   */
  static ptr_t CreateSecure(const std::string &path_to_ca_cert,
                            const std::string &host,
                            const std::string &path_to_client_key,
                            const std::string &path_to_client_cert, int port,
                            const std::string &username,
                            const std::string &password,
                            const std::string &vhost, int frame_max,
                            bool verify_hostname, bool verify_peer) {
    SSLConnectionParams ssl_params;
    ssl_params.path_to_ca_cert = path_to_ca_cert;
    ssl_params.path_to_client_key = path_to_client_key;
    ssl_params.path_to_client_cert = path_to_client_cert;
    ssl_params.verify_hostname = verify_hostname;
    ssl_params.verify_peer = verify_peer;

    return std::make_shared<Channel>(host, port, username, password, vhost,
                                     frame_max, ssl_params, false);
  }

  /**
   * Creates a new channel object
   * Creates a new connection to an AMQP broker using the supplied parameters
   * and opens a single channel for use
   * This channel uses the EXTERNAL SASL method for authentication to the broker
   * @param path_to_ca_cert Path to ca certificate file
   * @param host The hostname or IP address of the AMQP broker
   * @param path_to_client_key Path to client key file
   * @param path_to_client_cert Path to client certificate file
   * @param port The port to connect to the AMQP broker on
   * @param identity The identity used to authenticate with the AMQP broker
   * @param vhost The virtual host on the AMQP we should connect to
   * @param channel_max Request that the server limit the number of channels for
   * this connection to the specified parameter, a value of zero will use the
   * broker-supplied value
   * @param frame_max Request that the server limit the maximum size of any
   * frame to this value
   * @param verify_host Verify the hostname against the certificate when
   * opening the SSL connection.
   * @param verify_peer Verify the certificate chain that is sent by the broker
   * when opening the SSL connection.
   *
   * @return a new Channel object pointer
   */
  static ptr_t CreateSecureSaslExternal(const std::string &path_to_ca_cert,
                                        const std::string &host,
                                        const std::string &path_to_client_key,
                                        const std::string &path_to_client_cert,
                                        int port, const std::string &identity,
                                        const std::string &vhost, int frame_max,
                                        bool verify_hostname,
                                        bool verify_peer) {
    SSLConnectionParams ssl_params;
    ssl_params.path_to_ca_cert = path_to_ca_cert;
    ssl_params.path_to_client_key = path_to_client_key;
    ssl_params.path_to_client_cert = path_to_client_cert;
    ssl_params.verify_hostname = verify_hostname;
    ssl_params.verify_peer = verify_peer;

    return std::make_shared<Channel>(host, port, identity, "", vhost, frame_max,
                                     ssl_params, true);
  }

  /**
   * Create a new Channel object from an AMQP URI
   *
   * @param uri a URI of the form:
   * `amqp://[username:password@]{HOSTNAME}[:PORT][/VHOST]`
   * @param frame_max requests that the broker limit the maximum size of
   * any frame to this value
   * @returns a new Channel object
   */
  static ptr_t CreateFromUri(const std::string &uri, int frame_max = 131072);

  /**
   * Create a new Channel object from an AMQP URI, secured with SSL.
   * If URI should start with amqps://
   *
   * @param uri a URI of the form:
   * `amqp[s]://[username:password@]{HOSTNAME}[:PORT][/VHOST]`
   * @param path_to_ca_cert Path to ca certificate file
   * @param path_to_client_key Path to client key file
   * @param path_to_client_cert Path to client certificate file
   * @param verify_hostname Verify the hostname against the certificate when
   * opening the SSL connection.
   * @param frame_max requests that the broker limit the maximum size of
   * any frame to this value
   * @returns a new Channel object
   */
  static ptr_t CreateSecureFromUri(const std::string &uri,
                                   const std::string &path_to_ca_cert,
                                   const std::string &path_to_client_key = "",
                                   const std::string &path_to_client_cert = "",
                                   bool verify_hostname = true,
                                   int frame_max = 131072);

  /**
   * Constructor. Synchronously connects and logs in to the broker.
   *
   * @param host The hostname or IP address of the AMQP broker
   * @param port The port to connect to the AMQP broker on
   * @param username The username used to authenticate with the AMQP broker
   * @param password The password corresponding to the username used to
   * authenticate with the AMQP broker
   * @param vhost The virtual host on the AMQP we should connect to
   * @param frame_max Request that the server limit the maximum size of any
   * frame to this value
   */
  explicit Channel(const std::string &host, int port,
                   const std::string &username, const std::string &password,
                   const std::string &vhost, int frame_max, bool sasl_external);

  /**
   * Constructor. Synchronously connects and logs in to the broker.
   *
   * @param host The hostname or IP address of the AMQP broker
   * @param port The port to connect to the AMQP broker on
   * @param username The username used to authenticate with the AMQP broker
   * @param password The password corresponding to the username used to
   * authenticate with the AMQP broker
   * @param vhost The virtual host on the AMQP we should connect to
   * @param frame_max Request that the server limit the maximum size of any
   * frame to this value
   * @param ssl_params TLS config
   */
  explicit Channel(const std::string &host, int port,
                   const std::string &username, const std::string &password,
                   const std::string &vhost, int frame_max,
                   const SSLConnectionParams &ssl_params, bool sasl_external);

 public:
  // Non-copyable
  Channel(const Channel &) = delete;
  Channel &operator=(const Channel &) = delete;

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
   *
   * Creates an exchange on the AMQP broker if it does not already exist
   * @param exchange_name the name of the exchange
   * @param exchange_type the type of exchange to be declared. Defaults to
   * `direct`; other types that could be used: `fanout`, `topic`
   * @param passive Indicates how the broker should react if the exchange does
   * not exist. If passive is `true` and the exhange does not exist the broker
   * will respond with an error and not create the exchange; exchange is created
   * otherwise. Defaults to `false` (exchange is created if needed)
   * @param durable Indicates whether the exchange is durable - e.g., will it
   * survive a broker restart.
   * @param auto_delete Indicates whether the exchange will automatically be
   * removed when no queues are bound to it.
   */
  void DeclareExchange(
      const std::string &exchange_name,
      const std::string &exchange_type = Channel::EXCHANGE_TYPE_DIRECT,
      bool passive = false, bool durable = false, bool auto_delete = false);

  /**
   * Declares an exchange
   *
   * Creates an exchange on the AMQP broker if it does not already exist
   * @param exchange_name the name of the exchange
   * @param exchange_type the type of exchange to be declared. Defaults to
   * `direct`; other types that could be used: `fanout`, `topic`
   * @param passive Indicates how the broker should react if the exchange does
   * not exist. If passive is `true` and the exhange does not exist the broker
   * will respond with an error and not create the exchange; exchange is created
   * otherwise. Defaults to `false` (exchange is created if needed)
   * @param durable Indicates whether the exchange is durable - e.g., will it
   * survive a broker restart
   * @param auto_delete Indicates whether the exchange will automatically be
   * removed when no queues are bound to it.
   * @param arguments A table of additional arguments used when creating the
   * exchange
   */
  void DeclareExchange(const std::string &exchange_name,
                       const std::string &exchange_type, bool passive,
                       bool durable, bool auto_delete, const Table &arguments);

  /**
   * Deletes an exchange on the AMQP broker
   *
   * @param exchange_name the name of the exchange to be deleted
   * @param if_unused when `true`, delete the exchange only if it has no queues
   * bound to it, or throw `AmqpResponseServerException`. Default `false` -
   * delete regardless.
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
   * Declare a queue
   *
   * Creates a queue on the AMQP broker if it does not already exist.
   * @param queue_name The desired name of the queue. If this is an empty
   * string, the broker will generate a queue name that this method will return.
   * @param passive Indicated how the broker should react if the queue does not
   * exist. The broker will raise an error if the queue doesn't already exist
   * and passive is `true`. With passive `false` (the default), the queue gets
   * created automatically, if needed.
   * @param durable Indicates whether the exchange is durable - e.g., will it
   * survive a broker restart.
   * @param exclusive Indicates that only client can use the queue. Defaults to
   * true. An exclusive queue is deleted when the connection is closed.
   * @param auto_delete the queue will be deleted after at least one exchange
   * has been bound to it, then has been unbound
   * @returns The name of the queue created on the broker. Used mostly when the
   * broker is asked to create a unique queue by not providing a queue name.
   */
  std::string DeclareQueue(const std::string &queue_name, bool passive = false,
                           bool durable = false, bool exclusive = true,
                           bool auto_delete = true);

  /**
   * Declares a queue
   *
   * Creates a queue on the AMQP broker if it does not already exist.
   * @param queue_name The desired name of the queue. If this is an empty
   * string, the broker will generate a queue name that this method will return.
   * @param passive Indicated how the broker should react if the queue does not
   * exist. The broker will raise an error if the queue doesn't already exist
   * and passive is `true`. With passive `false` (the default), the queue gets
   * created automatically, if needed.
   * @param durable Indicates whether the exchange is durable - e.g., will it
   * survive a broker restart.
   * @param exclusive Indicates that only client can use the queue. Defaults to
   * true. An exclusive queue is deleted when the connection is closed.
   * @param auto_delete the queue will be deleted after at least one exchange
   * has been bound to it, then has been unbound
   * @param arguments A table of additional arguments
   * @returns The name of the queue created on the broker. Used mostly when the
   * broker is asked to create a unique queue by not providing a queue name.
   */
  std::string DeclareQueue(const std::string &queue_name, bool passive,
                           bool durable, bool exclusive, bool auto_delete,
                           const Table &arguments);

  /**
   * Declares a queue and returns current message- and consumer counts
   *
   * Creates a queue on the AMQP broker if it does not already exist.
   * @param queue_name The desired name of the queue. If this is an empty
   * string, the broker will generate a queue name that this method will return.
   * @param [out] message_count The current number of messages in the declared
   * queue gets passed out through this argument.
   * @param [out] consumer_count The current number of consumers of the declared
   * queue gets passed out through this argument.
   * @param passive Indicated how the broker should react if the queue does not
   * exist. The broker will raise an error if the queue doesn't already exist
   * and passive is `true`. With passive `false` (the default), the queue gets
   * created automatically, if needed.
   * @param durable Indicates whether the exchange is durable - e.g., will it
   * survive a broker restart.
   * @param exclusive Indicates that only client can use the queue. Defaults to
   * true. An exclusive queue is deleted when the connection is closed.
   * @param auto_delete the queue will be deleted after at least one exchange
   * has been bound to it, then has been unbound
   * @returns The name of the queue created on the broker. Used mostly when the
   * broker is asked to create a unique queue by not providing a queue name.
   */
  std::string DeclareQueueWithCounts(const std::string &queue_name,
                                     std::uint32_t &message_count,
                                     std::uint32_t &consumer_count,
                                     bool passive = false, bool durable = false,
                                     bool exclusive = true,
                                     bool auto_delete = true);

  /**
   * Declares a queue and returns current message- and consumer counts
   *
   * Creates a queue on the AMQP broker if it does not already exist.
   * @param queue_name The desired name of the queue. If this is an empty
   * string, the broker will generate a queue name that this method will return.
   * @param [out] message_count The current number of messages in the declared
   * queue gets passed out through this argument.
   * @param [out] consumer_count The current number of consumers of the declared
   * queue gets passed out through this argument.
   * @param passive Indicated how the broker should react if the queue does not
   * exist. The broker will raise an error if the queue doesn't already exist
   * and passive is `true`. With passive `false` (the default), the queue gets
   * created automatically, if needed.
   * @param durable Indicates whether the exchange is durable - e.g., will it
   * survive a broker restart.
   * @param exclusive Indicates that only client can use the queue. Defaults to
   * true. An exclusive queue is deleted when the connection is closed.
   * @param auto_delete the queue will be deleted after at least one exchange
   * has been bound to it, then has been unbound
   * @param arguments A table of additional arguments
   * @returns The name of the queue created on the broker. Used mostly when the
   * broker is asked to create a unique queue by not providing a queue name.
   */
  std::string DeclareQueueWithCounts(const std::string &queue_name,
                                     std::uint32_t &message_count,
                                     std::uint32_t &consumer_count,
                                     bool passive, bool durable, bool exclusive,
                                     bool auto_delete, const Table &arguments);

  /**
   * Deletes a queue
   *
   * Removes a queue from the broker. There is no indication of whether the
   * queue was actually deleted on the broker.
   * @param queue_name The name of the queue to remove.
   * @param if_unused Only deletes the queue if the queue doesn't have any
   * active consumers.
   * @param if_empty Only deletes the queue if the queue is empty.
   */
  void DeleteQueue(const std::string &queue_name, bool if_unused = false,
                   bool if_empty = false);

  /**
   * Binds a queue to an exchange
   *
   * Connects a queue to an exchange on the broker.
   * @param queue_name The name of the queue to bind.
   * @param exchange_name The name of the exchange to bind to.
   * @param routing_key Defines the routing key of the binding. Only messages
   * with matching routing key will be delivered to the queue from the exchange.
   * Defaults to `""` which means all messages will be delivered.
   */
  void BindQueue(const std::string &queue_name,
                 const std::string &exchange_name,
                 const std::string &routing_key = "");

  /**
   * Binds a queue to an exchange
   *
   * Connects a queue to an exchange on the broker.
   * @param queue_name The name of the queue to bind.
   * @param exchange_name The name of the exchange to bind to.
   * @param routing_key Defines the routing key of the binding. Only messages
   * with matching routing key will be delivered to the queue from the exchange.
   * Defaults to `""` which means all messages will be delivered.
   * @param arguments A table of additional arguments used when binding the
   * queue
   */
  void BindQueue(const std::string &queue_name,
                 const std::string &exchange_name,
                 const std::string &routing_key, const Table &arguments);

  /**
   * Unbinds a queue from an exchange
   *
   * Disconnects a queue from an exchange.
   * @param queue_name The name of the queue to unbind.
   * @param exchange_name The name of the exchange to unbind.
   * @param routing_key This must match the routing_key of the binding.
   * @see BindQueue
   */
  void UnbindQueue(const std::string &queue_name,
                   const std::string &exchange_name,
                   const std::string &routing_key = "");

  /**
   * Unbinds a queue from an exchange
   *
   * Disconnects a queue from an exchange.
   * @param queue_name The name of the queue to unbind.
   * @param exchange_name The name of the exchange to unbind.
   * @param routing_key This must match the routing_key of the binding.
   * @param arguments A table of additional arguments.
   * @see BindQueue
   */
  void UnbindQueue(const std::string &queue_name,
                   const std::string &exchange_name,
                   const std::string &routing_key, const Table &arguments);

  /**
   * Purges a queue
   *
   * Removes all messages in a queue on the broker. The queue will become empty.
   * @param queue_name The name of the queue to purge.
   */
  void PurgeQueue(const std::string &queue_name);

  /**
   * Acknowledges a Basic message
   *
   * Acknowledges a message delievered using \ref BasicGet or \ref BasicConsume.
   * @param message The message that is being ack'ed.
   */
  void BasicAck(const Envelope::ptr_t &message);

  /**
   * Acknowledges a Basic message
   *
   * Acknowledges a message delivered using \ref BasicGet or \ref BasicConsume.
   * This overload doesn't require the \ref Envelope object to acknowledge.
   *
   * @param info The `delivery-tag` of the message to acknowledge.
   */
  void BasicAck(const Envelope::DeliveryInfo &info);

  /**
   * Acknowledges a Basic message
   *
   * Acknowledges a message delivered using \ref BasicGet or \ref BasicConsume.
   * This overload doesn't require the \ref Envelope object to acknowledge.
   *
   * @note Ack'ing multiple messages (using `multiple=true`) is
   * scoped to messages delivered on a given AMQP channel. Since
   * SimpleAmqpClient uses one channel per consumer, this means that
   * multiple-ack will acknowledge messages up to and including the current
   * message id *for a given consumer*.
   *
   * @param info The `delivery-tag` of the message to acknowledge.
   * @param multiple If `true`, ack all messages in this channel up to this
   * delivery tag.  If `false`, ack only this delivery tag.
   */
  void BasicAck(const Envelope::DeliveryInfo &info, bool multiple);

  /**
   * Reject (NAck) a Basic message
   *
   * Rejects a message delivered using \ref BasicGet or \ref BasicConsume
   * @param message The message that is being nack'ed.
   * @param requeue Tells the broker to requeue the message or not.
   * @param multiple If `true`, reject all messages in this channel up to this.
   */
  void BasicReject(const Envelope::ptr_t &message, bool requeue,
                   bool multiple = false);

  /**
   * Reject (NAck) a Basic message
   *
   * Rejects a message delivered using \ref BasicGet or \ref BasicConsume.
   * This overload doesn't require the \ref Envelope object to Reject
   * @param info The `delivery-tag` of the message to Reject.
   * @param requeue Tells the broker to requeue the message or not.
   * @param multiple If `true`, reject all messages in this channel up to this.
   */
  void BasicReject(const Envelope::DeliveryInfo &info, bool requeue,
                   bool multiple = false);

  /**
   * Publishes a Basic message
   *
   * Publishes a Basic message to an exchange
   * @param exchange_name The name of the exchange to publish the message to
   * @param routing_key The routing key to publish with, this is used to route
   * to corresponding queue(s).
   * @param message The \ref BasicMessage object to publish to the queue.
   * @param mandatory Requires the message to be delivered to a queue. A
   * \ref MessageReturnedException is thrown if the message cannot be routed to
   * a queue.
   * @param immediate Requires the message to be both routed to a queue, and
   * immediately delivered to a consumer. If the message is not routed, or a
   * consumer cannot immediately deliver the message,
   * a \ref MessageReturnedException is thrown. This has no effect when using
   * RabbitMQ v3.0 and newer.
   */
  void BasicPublish(const std::string &exchange_name,
                    const std::string &routing_key,
                    const BasicMessage::ptr_t message, bool mandatory = false,
                    bool immediate = false);

  /**
   * Synchronously consume a message from a queue
   *
   * This function will not wait for a message to arrive in a queue, it will
   * return `false` if the queue is empty.
   * @note This function effectively polls the broker for a message; in general,
   * better performance is realized using \ref BasicConsume / \ref
   * BasicConsumeMessage.
   * @param [out] message A message envelope pointer that will be populated if a
   * message is delivered.
   * @param queue The name of the queue to try to get the message from.
   * @param no_ack Can the message be un-ack'ed. Default `true` (message does
   * not need to be acked).
   * @returns `true` if a message was delivered, `false` if the queue was empty.
   */
  bool BasicGet(Envelope::ptr_t &message, const std::string &queue,
                bool no_ack = true);

  /**
   * Redeliver unacknowledged messages from the broker
   * @param consumer The consumer to recover message from
   */
  void BasicRecover(const std::string &consumer);

  /**
   * Starts consuming Basic messages on a queue
   *
   * Subscribes as a consumer to a queue, so all future messages on a queue
   * will be Basic.Delivered
   * @note Due to a limitation to how things are done, it is only possible to
   * reliably have **a single consumer per channel**; calling this
   * more than once per channel may result in undefined results.
   * @param queue The name of the queue to subscribe to.
   * @param consumer_tag The name of the consumer. This is used to do
   * operations with a consumer.
   * @param no_local Defaults to true
   * @param no_ack If `true`, ack'ing the message is automatically done when the
   * message is delivered. Defaults to `true` (message does not have to be
   * ack'ed).
   * @param exclusive Means only this consumer can access the queue.
   * @param message_prefetch_count Number of unacked messages the broker will
   * deliver. Setting this to more than 1 will allow the broker to deliver
   * messages while a current message is being processed. A value of
   * 0 means no limit. This option is ignored if `no_ack = true`.
   * @returns the consumer tag
   */
  std::string BasicConsume(const std::string &queue,
                           const std::string &consumer_tag = "",
                           bool no_local = true, bool no_ack = true,
                           bool exclusive = true,
                           std::uint16_t message_prefetch_count = 1);

  /**
   * Starts consuming Basic messages on a queue
   *
   * Subscribes as a consumer to a queue, so all future messages on a queue
   * will be Basic.Delivered
   * @note Due to a limitation to how things are done, it is only possible to
   * reliably have **a single consumer per channel**; calling this
   * more than once per channel may result in undefined results.
   * @param queue The name of the queue to subscribe to.
   * @param consumer_tag The name of the consumer. This is used to do
   * operations with a consumer.
   * @param no_local Defaults to true
   * @param no_ack If `true`, ack'ing the message is automatically done when the
   * message is delivered. Defaults to `true` (message does not have to be
   * ack'ed).
   * @param exclusive Means only this consumer can access the queue.
   * @param message_prefetch_count Number of unacked messages the broker will
   * deliver. Setting this to more than 1 will allow the broker to deliver
   * messages while a current message is being processed. A value of
   * 0 means no limit. This option is ignored if `no_ack = true`.
   * @param arguments A table of additional arguments when creating the consumer
   * @returns the consumer tag
   */
  std::string BasicConsume(const std::string &queue,
                           const std::string &consumer_tag, bool no_local,
                           bool no_ack, bool exclusive,
                           std::uint16_t message_prefetch_count,
                           const Table &arguments);

  /**
   * Modify consumer's message prefetch count
   *
   * Sets the number of unacknowledged messages that will be delivered
   * by the broker to a consumer.
   *
   * Has no effect for consumer with `no_ack` set.
   *
   * @param consumer_tag The consumer tag to adjust the prefetch for.
   * @param message_prefetch_count The number of unacknowledged message the
   * broker will deliver. A value of 0 means no limit.
   */
  void BasicQos(const std::string &consumer_tag,
                std::uint16_t message_prefetch_count);

  /**
   * Cancels a previously created Consumer
   *
   * Unsubscribes a consumer from a queue. In other words undoes what
   * \ref BasicConsume does.
   * @param consumer_tag The same `consumer_tag` used when the consumer was
   * created with \ref BasicConsume.
   * @see BasicConsume
   */
  void BasicCancel(const std::string &consumer_tag);

  /**
   * Consumes a single message
   *
   * Waits for a single Basic message to be Delivered.
   *
   * This function only works after `BasicConsume` has successfully been called.
   *
   * @param consumer_tag Consumer ID (returned from \ref BasicConsume).
   * @returns The next message on the queue
   */
  Envelope::ptr_t BasicConsumeMessage(const std::string &consumer_tag);

  /**
   * Consumes a single message from multiple consumers
   *
   * Waits for a single message to be delivered from a list of consumers.
   *
   * This function only works after \ref BasicConsume has been called.
   *
   * @returns The next message delivered from the broker
   */
  Envelope::ptr_t BasicConsumeMessage(
      const std::vector<std::string> &consumer_tags);

  /**
   * Consumes a single message from any open consumers
   *
   * Waits for a message from any consumer open on this Channel object.
   *
   * This function only works after \ref BasicConsume has been called.
   *
   * @returns The next message delivered from the broker
   */
  Envelope::ptr_t BasicConsumeMessage();

  /**
   * Consumes a single message with a timeout (gets an envelope object)
   *
   * Waits for a single Basic message to be Delivered or the timeout to expire.
   *
   * This function only works after \ref BasicConsume as been successfully
   * called.
   *
   * This function returns an envelope object which contains more information
   * about the message delivered.
   *
   * @param consumer_tag Consumer ID (returned from \ref BasicConsume).
   * @param [out] envelope The message object to save to. Empty pointer is ok.
   * @param timeout Timeout, in ms, for the first part of the message to be
   * delivered
   * @throws MessageReturnedException If a `basic.return` is received while
   * waiting for a message.
   * @returns `false` on timeout, `true` on message delivery
   */
  bool BasicConsumeMessage(const std::string &consumer_tag,
                           Envelope::ptr_t &envelope, int timeout = -1);

  /**
   * Consumes a single message with a timeout from multiple consumers
   *
   * Waits for a single message to be delivered to one of the listed consumer
   * tags, or for the timeout to expire.
   *
   * This function only works after `BasicConsume` has been successfully called.
   *
   * @param consumer_tags A list of the consumer tags to wait from.
   * @param [out] envelope The message object that is delivered.
   * @param timeout The timeout in milliseconds for the message to be
   * delivered. 0 works like a non-blocking read, -1 is an infinite timeout.
   * @returns `true` on message delivery, `false` on timeout.
   */
  bool BasicConsumeMessage(const std::vector<std::string> &consumer_tags,
                           Envelope::ptr_t &envelope, int timeout = -1);

  /**
   * Consumes a single message from any consumer on a Channel
   *
   * Waits for a single message to be delivered to one of the consumers opened
   * on this Channel, or for the timeout to occur.
   *
   * This function only works after \ref BasicConsume has been successfully
   * called.
   *
   * @param [out] envelope The message object that is delivered.
   * @param timeout The timeout in milliseconds for the message to be delivered.
   * 0 works like a non-blocking read, -1 is an infinite timeout.
   * @returns `true` on message delivery, `false` on timeout.
   */
  bool BasicConsumeMessage(Envelope::ptr_t &envelope, int timeout = -1);

 protected:
  /// PIMPL idiom
  std::unique_ptr<Detail::ChannelImpl> m_impl;
};

}  // namespace AmqpClient

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif  // SIMPLEAMQPCLIENT_CHANNEL_H
