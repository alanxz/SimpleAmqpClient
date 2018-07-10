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

// Put these first to avoid warnings about INT#_C macro redefinition
#include <amqp.h>
#include <amqp_framing.h>
#include <amqp_tcp_socket.h>
#ifdef SAC_SSL_SUPPORT_ENABLED
#include <amqp_ssl_socket.h>
#endif

#include "SimpleAmqpClient/Channel.h"

#include "SimpleAmqpClient/AmqpLibraryException.h"
#include "SimpleAmqpClient/AmqpResponseLibraryException.h"
#include "SimpleAmqpClient/BadUriException.h"
#include "SimpleAmqpClient/ChannelImpl.h"
#include "SimpleAmqpClient/ConsumerCancelledException.h"
#include "SimpleAmqpClient/ConsumerTagNotFoundException.h"
#include "SimpleAmqpClient/MessageReturnedException.h"
#include "SimpleAmqpClient/TableImpl.h"
#include "SimpleAmqpClient/Util.h"

#include <map>
#include <new>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/cstdint.hpp>
#include <boost/limits.hpp>

#include <string.h>

namespace AmqpClient {

const std::string Channel::EXCHANGE_TYPE_DIRECT("direct");
const std::string Channel::EXCHANGE_TYPE_FANOUT("fanout");
const std::string Channel::EXCHANGE_TYPE_TOPIC("topic");

Channel::ptr_t Channel::CreateFromUri(const std::string &uri, int frame_max) {
  amqp_connection_info info;
  amqp_default_connection_info(&info);

  boost::shared_ptr<char> uri_dup =
      boost::shared_ptr<char>(strdup(uri.c_str()), free);

  if (0 != amqp_parse_url(uri_dup.get(), &info)) {
    throw BadUriException();
  }

  return Create(std::string(info.host), info.port, std::string(info.user),
                std::string(info.password), std::string(info.vhost), frame_max);
}

Channel::ptr_t Channel::CreateSecureFromUri(
    const std::string &uri, const std::string &path_to_ca_cert,
    const std::string &path_to_client_key,
    const std::string &path_to_client_cert, bool verify_hostname,
    int frame_max) {
  amqp_connection_info info;
  amqp_default_connection_info(&info);

  boost::shared_ptr<char> uri_dup =
      boost::shared_ptr<char>(strdup(uri.c_str()), free);

  if (0 != amqp_parse_url(uri_dup.get(), &info)) {
    throw BadUriException();
  }

  if (info.ssl) {
    return CreateSecure(path_to_ca_cert, std::string(info.host),
                        path_to_client_key, path_to_client_cert, info.port,
                        std::string(info.user), std::string(info.password),
                        std::string(info.vhost), frame_max, verify_hostname);
  }
  throw std::runtime_error(
      "CreateSecureFromUri only supports SSL-enabled URIs.");
}

Channel::Channel(const std::string &host, int port, const std::string &username,
                 const std::string &password, const std::string &vhost,
                 int frame_max)
    : m_impl(new Detail::ChannelImpl) {
  m_impl->m_connection = amqp_new_connection();

  if (NULL == m_impl->m_connection) {
    throw std::bad_alloc();
  }

  try {
    amqp_socket_t *socket = amqp_tcp_socket_new(m_impl->m_connection);
    int sock = amqp_socket_open(socket, host.c_str(), port);
    m_impl->CheckForError(sock);

    m_impl->DoLogin(username, password, vhost, frame_max);
  } catch (...) {
    amqp_destroy_connection(m_impl->m_connection);
    throw;
  }

  m_impl->SetIsConnected(true);
}

#ifdef SAC_SSL_SUPPORT_ENABLED
Channel::Channel(const std::string &host, int port, const std::string &username,
                 const std::string &password, const std::string &vhost,
                 int frame_max, const SSLConnectionParams &ssl_params)
    : m_impl(new Detail::ChannelImpl) {
  m_impl->m_connection = amqp_new_connection();
  if (NULL == m_impl->m_connection) {
    throw std::bad_alloc();
  }

  amqp_socket_t *socket = amqp_ssl_socket_new(m_impl->m_connection);
  if (NULL == socket) {
    throw std::bad_alloc();
  }
#if AMQP_VERSION >= 0x00080001
  amqp_ssl_socket_set_verify_peer(socket, ssl_params.verify_hostname);
  amqp_ssl_socket_set_verify_hostname(socket, ssl_params.verify_hostname);
#else
  amqp_ssl_socket_set_verify(socket, ssl_params.verify_hostname);
#endif

  try {
    int status =
        amqp_ssl_socket_set_cacert(socket, ssl_params.path_to_ca_cert.c_str());
    if (status) {
      throw AmqpLibraryException::CreateException(
          status, "Error setting CA certificate for socket");
    }

    if (ssl_params.path_to_client_key != "" &&
        ssl_params.path_to_client_cert != "") {
      status = amqp_ssl_socket_set_key(socket,
                                       ssl_params.path_to_client_cert.c_str(),
                                       ssl_params.path_to_client_key.c_str());
      if (status) {
        throw AmqpLibraryException::CreateException(
            status, "Error setting client certificate for socket");
      }
    }

    status = amqp_socket_open(socket, host.c_str(), port);
    if (status) {
      throw AmqpLibraryException::CreateException(
          status, "Error setting client certificate for socket");
    }

    m_impl->DoLogin(username, password, vhost, frame_max);
  } catch (...) {
    amqp_destroy_connection(m_impl->m_connection);
    throw;
  }

  m_impl->SetIsConnected(true);
}
#else
Channel::Channel(const std::string &, int, const std::string &,
                 const std::string &, const std::string &, int,
                 const SSLConnectionParams &) {
  throw std::logic_error(
      "SSL support has not been compiled into SimpleAmqpClient");
}
#endif

Channel::~Channel() {
  amqp_connection_close(m_impl->m_connection, AMQP_REPLY_SUCCESS);
  amqp_destroy_connection(m_impl->m_connection);
}

int Channel::GetSocketFD() const {
  return amqp_get_sockfd(m_impl->m_connection);
}

void Channel::DeclareExchange(const std::string &exchange_name,
                              const std::string &exchange_type, bool passive,
                              bool durable, bool auto_delete) {
  DeclareExchange(exchange_name, exchange_type, passive, durable, auto_delete,
                  Table());
}

void Channel::DeclareExchange(const std::string &exchange_name,
                              const std::string &exchange_type, bool passive,
                              bool durable, bool auto_delete,
                              const Table &arguments) {
  const boost::array<boost::uint32_t, 1> DECLARE_OK = {
      {AMQP_EXCHANGE_DECLARE_OK_METHOD}};
  m_impl->CheckIsConnected();

  amqp_exchange_declare_t declare = {};
  declare.exchange = amqp_cstring_bytes(exchange_name.c_str());
  declare.type = amqp_cstring_bytes(exchange_type.c_str());
  declare.passive = passive;
  declare.durable = durable;
  declare.auto_delete = auto_delete;
  declare.internal = false;
  declare.nowait = false;

  Detail::amqp_pool_ptr_t table_pool;
  declare.arguments =
      Detail::TableValueImpl::CreateAmqpTable(arguments, table_pool);

  amqp_frame_t frame =
      m_impl->DoRpc(AMQP_EXCHANGE_DECLARE_METHOD, &declare, DECLARE_OK);
  m_impl->MaybeReleaseBuffersOnChannel(frame.channel);
}

void Channel::DeleteExchange(const std::string &exchange_name, bool if_unused) {
  const boost::array<boost::uint32_t, 1> DELETE_OK = {
      {AMQP_EXCHANGE_DELETE_OK_METHOD}};
  m_impl->CheckIsConnected();

  amqp_exchange_delete_t del = {};
  del.exchange = amqp_cstring_bytes(exchange_name.c_str());
  del.if_unused = if_unused;
  del.nowait = false;

  amqp_frame_t frame =
      m_impl->DoRpc(AMQP_EXCHANGE_DELETE_METHOD, &del, DELETE_OK);
  m_impl->MaybeReleaseBuffersOnChannel(frame.channel);
}

void Channel::BindExchange(const std::string &destination,
                           const std::string &source,
                           const std::string &routing_key) {
  BindExchange(destination, source, routing_key, Table());
}

void Channel::BindExchange(const std::string &destination,
                           const std::string &source,
                           const std::string &routing_key,
                           const Table &arguments) {
  const boost::array<boost::uint32_t, 1> BIND_OK = {
      {AMQP_EXCHANGE_BIND_OK_METHOD}};
  m_impl->CheckIsConnected();

  amqp_exchange_bind_t bind = {};
  bind.destination = amqp_cstring_bytes(destination.c_str());
  bind.source = amqp_cstring_bytes(source.c_str());
  bind.routing_key = amqp_cstring_bytes(routing_key.c_str());
  bind.nowait = false;

  Detail::amqp_pool_ptr_t table_pool;
  bind.arguments =
      Detail::TableValueImpl::CreateAmqpTable(arguments, table_pool);

  amqp_frame_t frame = m_impl->DoRpc(AMQP_EXCHANGE_BIND_METHOD, &bind, BIND_OK);
  m_impl->MaybeReleaseBuffersOnChannel(frame.channel);
}

void Channel::UnbindExchange(const std::string &destination,
                             const std::string &source,
                             const std::string &routing_key) {
  UnbindExchange(destination, source, routing_key, Table());
}

void Channel::UnbindExchange(const std::string &destination,
                             const std::string &source,
                             const std::string &routing_key,
                             const Table &arguments) {
  const boost::array<boost::uint32_t, 1> UNBIND_OK = {
      {AMQP_EXCHANGE_UNBIND_OK_METHOD}};
  m_impl->CheckIsConnected();

  amqp_exchange_unbind_t unbind = {};
  unbind.destination = amqp_cstring_bytes(destination.c_str());
  unbind.source = amqp_cstring_bytes(source.c_str());
  unbind.routing_key = amqp_cstring_bytes(routing_key.c_str());
  unbind.nowait = false;

  Detail::amqp_pool_ptr_t table_pool;
  unbind.arguments =
      Detail::TableValueImpl::CreateAmqpTable(arguments, table_pool);

  amqp_frame_t frame =
      m_impl->DoRpc(AMQP_EXCHANGE_UNBIND_METHOD, &unbind, UNBIND_OK);
  m_impl->MaybeReleaseBuffersOnChannel(frame.channel);
}

std::string Channel::DeclareQueue(const std::string &queue_name, bool passive,
                                  bool durable, bool exclusive,
                                  bool auto_delete) {
  return DeclareQueue(queue_name, passive, durable, exclusive, auto_delete,
                      Table());
}

std::string Channel::DeclareQueue(const std::string &queue_name, bool passive,
                                  bool durable, bool exclusive,
                                  bool auto_delete, const Table &arguments) {
  boost::uint32_t message_count;
  boost::uint32_t consumer_count;
  return DeclareQueueWithCounts(queue_name, message_count, consumer_count,
                                passive, durable, exclusive, auto_delete,
                                arguments);
}

std::string Channel::DeclareQueueWithCounts(const std::string &queue_name,
                                            boost::uint32_t &message_count,
                                            boost::uint32_t &consumer_count,
                                            bool passive, bool durable,
                                            bool exclusive, bool auto_delete) {
  return DeclareQueueWithCounts(queue_name, message_count, consumer_count,
                                passive, durable, exclusive, auto_delete,
                                Table());
}

std::string Channel::DeclareQueueWithCounts(const std::string &queue_name,
                                            boost::uint32_t &message_count,
                                            boost::uint32_t &consumer_count,
                                            bool passive, bool durable,
                                            bool exclusive, bool auto_delete,
                                            const Table &arguments) {
  const boost::array<boost::uint32_t, 1> DECLARE_OK = {
      {AMQP_QUEUE_DECLARE_OK_METHOD}};
  m_impl->CheckIsConnected();

  amqp_queue_declare_t declare = {};
  declare.queue = amqp_cstring_bytes(queue_name.c_str());
  declare.passive = passive;
  declare.durable = durable;
  declare.exclusive = exclusive;
  declare.auto_delete = auto_delete;
  declare.nowait = false;

  Detail::amqp_pool_ptr_t table_pool;
  declare.arguments =
      Detail::TableValueImpl::CreateAmqpTable(arguments, table_pool);

  amqp_frame_t response =
      m_impl->DoRpc(AMQP_QUEUE_DECLARE_METHOD, &declare, DECLARE_OK);

  amqp_queue_declare_ok_t *declare_ok =
      (amqp_queue_declare_ok_t *)response.payload.method.decoded;

  std::string ret((char *)declare_ok->queue.bytes, declare_ok->queue.len);

  message_count = declare_ok->message_count;
  consumer_count = declare_ok->consumer_count;

  m_impl->MaybeReleaseBuffersOnChannel(response.channel);
  return ret;
}

void Channel::DeleteQueue(const std::string &queue_name, bool if_unused,
                          bool if_empty) {
  const boost::array<boost::uint32_t, 1> DELETE_OK = {
      {AMQP_QUEUE_DELETE_OK_METHOD}};
  m_impl->CheckIsConnected();

  amqp_queue_delete_t del = {};
  del.queue = amqp_cstring_bytes(queue_name.c_str());
  del.if_unused = if_unused;
  del.if_empty = if_empty;
  del.nowait = false;

  amqp_frame_t frame = m_impl->DoRpc(AMQP_QUEUE_DELETE_METHOD, &del, DELETE_OK);
  m_impl->MaybeReleaseBuffersOnChannel(frame.channel);
}

void Channel::BindQueue(const std::string &queue_name,
                        const std::string &exchange_name,
                        const std::string &routing_key) {
  BindQueue(queue_name, exchange_name, routing_key, Table());
}

void Channel::BindQueue(const std::string &queue_name,
                        const std::string &exchange_name,
                        const std::string &routing_key,
                        const Table &arguments) {
  const boost::array<boost::uint32_t, 1> BIND_OK = {
      {AMQP_QUEUE_BIND_OK_METHOD}};
  m_impl->CheckIsConnected();

  amqp_queue_bind_t bind = {};
  bind.queue = amqp_cstring_bytes(queue_name.c_str());
  bind.exchange = amqp_cstring_bytes(exchange_name.c_str());
  bind.routing_key = amqp_cstring_bytes(routing_key.c_str());
  bind.nowait = false;

  Detail::amqp_pool_ptr_t table_pool;
  bind.arguments =
      Detail::TableValueImpl::CreateAmqpTable(arguments, table_pool);

  amqp_frame_t frame = m_impl->DoRpc(AMQP_QUEUE_BIND_METHOD, &bind, BIND_OK);
  m_impl->MaybeReleaseBuffersOnChannel(frame.channel);
}

void Channel::UnbindQueue(const std::string &queue_name,
                          const std::string &exchange_name,
                          const std::string &routing_key) {
  UnbindQueue(queue_name, exchange_name, routing_key, Table());
}

void Channel::UnbindQueue(const std::string &queue_name,
                          const std::string &exchange_name,
                          const std::string &routing_key,
                          const Table &arguments) {
  const boost::array<boost::uint32_t, 1> UNBIND_OK = {
      {AMQP_QUEUE_UNBIND_OK_METHOD}};
  m_impl->CheckIsConnected();

  amqp_queue_unbind_t unbind = {};
  unbind.queue = amqp_cstring_bytes(queue_name.c_str());
  unbind.exchange = amqp_cstring_bytes(exchange_name.c_str());
  unbind.routing_key = amqp_cstring_bytes(routing_key.c_str());

  Detail::amqp_pool_ptr_t table_pool;
  unbind.arguments =
      Detail::TableValueImpl::CreateAmqpTable(arguments, table_pool);

  amqp_frame_t frame =
      m_impl->DoRpc(AMQP_QUEUE_UNBIND_METHOD, &unbind, UNBIND_OK);
  m_impl->MaybeReleaseBuffersOnChannel(frame.channel);
}

void Channel::PurgeQueue(const std::string &queue_name) {
  const boost::array<boost::uint32_t, 1> PURGE_OK = {
      {AMQP_QUEUE_PURGE_OK_METHOD}};
  m_impl->CheckIsConnected();

  amqp_queue_purge_t purge = {};
  purge.queue = amqp_cstring_bytes(queue_name.c_str());
  purge.nowait = false;

  amqp_frame_t frame = m_impl->DoRpc(AMQP_QUEUE_PURGE_METHOD, &purge, PURGE_OK);
  m_impl->MaybeReleaseBuffersOnChannel(frame.channel);
}

void Channel::BasicAck(const Envelope::ptr_t &message) {
  BasicAck(message->GetDeliveryInfo());
}

void Channel::BasicAck(const Envelope::DeliveryInfo &info) {
  BasicAck(info, false);
}

void Channel::BasicAck(const Envelope::DeliveryInfo &info, bool multiple) {
  m_impl->CheckIsConnected();
  // Delivery tag is local to the channel, so its important to use
  // that channel, sadly this can cause the channel to throw an exception
  // which will show up as an unrelated exception in a different method
  // that actually waits for a response from the broker
  amqp_channel_t channel = info.delivery_channel;
  if (!m_impl->IsChannelOpen(channel)) {
    throw std::runtime_error(
        "The channel that the message was delivered on has been closed");
  }

  m_impl->CheckForError(
      amqp_basic_ack(m_impl->m_connection, channel, info.delivery_tag, multiple));
}

void Channel::BasicReject(const Envelope::ptr_t &message, bool requeue,
                          bool multiple) {
  BasicReject(message->GetDeliveryInfo(), requeue, multiple);
}

void Channel::BasicReject(const Envelope::DeliveryInfo &info, bool requeue,
                          bool multiple) {
  m_impl->CheckIsConnected();
  // Delivery tag is local to the channel, so its important to use
  // that channel, sadly this can cause the channel to throw an exception
  // which will show up as an unrelated exception in a different method
  // that actually waits for a response from the broker
  amqp_channel_t channel = info.delivery_channel;
  if (!m_impl->IsChannelOpen(channel)) {
    throw std::runtime_error(
        "The channel that the message was delivered on has been closed");
  }
  amqp_basic_nack_t req;
  req.delivery_tag = info.delivery_tag;
  req.multiple = multiple;
  req.requeue = requeue;

  m_impl->CheckForError(amqp_send_method(m_impl->m_connection, channel,
                                         AMQP_BASIC_NACK_METHOD, &req));
}

void Channel::BasicPublish(const std::string &exchange_name,
                           const std::string &routing_key,
                           const BasicMessage::ptr_t message, bool mandatory,
                           bool immediate) {
  m_impl->CheckIsConnected();
  amqp_channel_t channel = m_impl->GetChannel();

  m_impl->CheckForError(amqp_basic_publish(
      m_impl->m_connection, channel, amqp_cstring_bytes(exchange_name.c_str()),
      amqp_cstring_bytes(routing_key.c_str()), mandatory, immediate,
      message->getAmqpProperties(), message->getAmqpBody()));

  // If we've done things correctly we can get one of 4 things back from the
  // broker
  // - basic.ack - our channel is in confirm mode, messsage was 'dealt with' by
  // the broker
  // - basic.return then basic.ack - the message wasn't delievered, but was
  // dealt with
  // - channel.close - probably tried to publish to a non-existant exchange, in
  // any case error!
  // - connection.clsoe - something really bad happened
  const boost::array<boost::uint32_t, 2> PUBLISH_ACK = {
      {AMQP_BASIC_ACK_METHOD, AMQP_BASIC_RETURN_METHOD}};
  amqp_frame_t response;
  boost::array<amqp_channel_t, 1> channels = {{channel}};
  m_impl->GetMethodOnChannel(channels, response, PUBLISH_ACK);

  if (AMQP_BASIC_RETURN_METHOD == response.payload.method.id) {
    MessageReturnedException message_returned =
        m_impl->CreateMessageReturnedException(
            *(reinterpret_cast<amqp_basic_return_t *>(
                response.payload.method.decoded)),
            channel);

    const boost::array<boost::uint32_t, 1> BASIC_ACK = {
        {AMQP_BASIC_ACK_METHOD}};
    m_impl->GetMethodOnChannel(channels, response, BASIC_ACK);
    m_impl->ReturnChannel(channel);
    m_impl->MaybeReleaseBuffersOnChannel(channel);
    throw message_returned;
  }

  m_impl->ReturnChannel(channel);
  m_impl->MaybeReleaseBuffersOnChannel(channel);
}

bool Channel::BasicGet(Envelope::ptr_t &envelope, const std::string &queue,
                       bool no_ack) {
  const boost::array<boost::uint32_t, 2> GET_RESPONSES = {
      {AMQP_BASIC_GET_OK_METHOD, AMQP_BASIC_GET_EMPTY_METHOD}};
  m_impl->CheckIsConnected();

  amqp_basic_get_t get = {};
  get.queue = amqp_cstring_bytes(queue.c_str());
  get.no_ack = no_ack;

  amqp_channel_t channel = m_impl->GetChannel();
  amqp_frame_t response = m_impl->DoRpcOnChannel(channel, AMQP_BASIC_GET_METHOD,
                                                 &get, GET_RESPONSES);

  if (AMQP_BASIC_GET_EMPTY_METHOD == response.payload.method.id) {
    m_impl->ReturnChannel(channel);
    m_impl->MaybeReleaseBuffersOnChannel(channel);
    return false;
  }

  amqp_basic_get_ok_t *get_ok =
      (amqp_basic_get_ok_t *)response.payload.method.decoded;
  boost::uint64_t delivery_tag = get_ok->delivery_tag;
  bool redelivered = (get_ok->redelivered == 0 ? false : true);
  std::string exchange((char *)get_ok->exchange.bytes, get_ok->exchange.len);
  std::string routing_key((char *)get_ok->routing_key.bytes,
                          get_ok->routing_key.len);

  BasicMessage::ptr_t message = m_impl->ReadContent(channel);
  envelope = Envelope::Create(message, "", delivery_tag, exchange, redelivered,
                              routing_key, channel);

  m_impl->ReturnChannel(channel);
  m_impl->MaybeReleaseBuffersOnChannel(channel);
  return true;
}

void Channel::BasicRecover(const std::string &consumer) {
  const boost::array<boost::uint32_t, 1> RECOVER_OK = {
      {AMQP_BASIC_RECOVER_OK_METHOD}};
  m_impl->CheckIsConnected();

  amqp_basic_recover_t recover = {};
  recover.requeue = true;

  amqp_channel_t channel = m_impl->GetConsumerChannel(consumer);

  m_impl->DoRpcOnChannel(channel, AMQP_BASIC_RECOVER_METHOD, &recover,
                         RECOVER_OK);
  m_impl->MaybeReleaseBuffersOnChannel(channel);
}

std::string Channel::BasicConsume(const std::string &queue,
                                  const std::string &consumer_tag,
                                  bool no_local, bool no_ack, bool exclusive,
                                  boost::uint16_t message_prefetch_count) {
  return BasicConsume(queue, consumer_tag, no_local, no_ack, exclusive,
                      message_prefetch_count, Table());
}
std::string Channel::BasicConsume(const std::string &queue,
                                  const std::string &consumer_tag,
                                  bool no_local, bool no_ack, bool exclusive,
                                  boost::uint16_t message_prefetch_count,
                                  const Table &arguments) {
  m_impl->CheckIsConnected();
  amqp_channel_t channel = m_impl->GetChannel();

  // Set this before starting the consume as it may have been set by a previous
  // consumer
  const boost::array<boost::uint32_t, 1> QOS_OK = {{AMQP_BASIC_QOS_OK_METHOD}};

  amqp_basic_qos_t qos = {};
  qos.prefetch_size = 0;
  qos.prefetch_count = message_prefetch_count;
  qos.global = m_impl->BrokerHasNewQosBehavior();

  m_impl->DoRpcOnChannel(channel, AMQP_BASIC_QOS_METHOD, &qos, QOS_OK);
  m_impl->MaybeReleaseBuffersOnChannel(channel);

  const boost::array<boost::uint32_t, 1> CONSUME_OK = {
      {AMQP_BASIC_CONSUME_OK_METHOD}};

  amqp_basic_consume_t consume = {};
  consume.queue = amqp_cstring_bytes(queue.c_str());
  consume.consumer_tag = amqp_cstring_bytes(consumer_tag.c_str());
  consume.no_local = no_local;
  consume.no_ack = no_ack;
  consume.exclusive = exclusive;
  consume.nowait = false;

  Detail::amqp_pool_ptr_t table_pool;
  consume.arguments =
      Detail::TableValueImpl::CreateAmqpTable(arguments, table_pool);

  amqp_frame_t response = m_impl->DoRpcOnChannel(
      channel, AMQP_BASIC_CONSUME_METHOD, &consume, CONSUME_OK);

  amqp_basic_consume_ok_t *consume_ok =
      (amqp_basic_consume_ok_t *)response.payload.method.decoded;
  std::string tag((char *)consume_ok->consumer_tag.bytes,
                  consume_ok->consumer_tag.len);
  m_impl->MaybeReleaseBuffersOnChannel(channel);

  m_impl->AddConsumer(tag, channel);

  return tag;
}

void Channel::BasicQos(const std::string &consumer_tag,
                       boost::uint16_t message_prefetch_count) {
  m_impl->CheckIsConnected();
  amqp_channel_t channel = m_impl->GetConsumerChannel(consumer_tag);

  const boost::array<boost::uint32_t, 1> QOS_OK = {{AMQP_BASIC_QOS_OK_METHOD}};

  amqp_basic_qos_t qos = {};
  qos.prefetch_size = 0;
  qos.prefetch_count = message_prefetch_count;
  qos.global = m_impl->BrokerHasNewQosBehavior();

  m_impl->DoRpcOnChannel(channel, AMQP_BASIC_QOS_METHOD, &qos, QOS_OK);
  m_impl->MaybeReleaseBuffersOnChannel(channel);
}

void Channel::BasicCancel(const std::string &consumer_tag) {
  m_impl->CheckIsConnected();
  amqp_channel_t channel = m_impl->GetConsumerChannel(consumer_tag);

  const boost::array<boost::uint32_t, 1> CANCEL_OK = {
      {AMQP_BASIC_CANCEL_OK_METHOD}};

  amqp_basic_cancel_t cancel = {};
  cancel.consumer_tag = amqp_cstring_bytes(consumer_tag.c_str());
  cancel.nowait = false;

  m_impl->DoRpcOnChannel(channel, AMQP_BASIC_CANCEL_METHOD, &cancel, CANCEL_OK);

  m_impl->RemoveConsumer(consumer_tag);

  // Lets go hunting to make sure we don't have any queued frames lying around
  // Otherwise these frames will potentially hang around when we don't want them
  // to
  // TODO: Implement queue purge
  m_impl->ReturnChannel(channel);
  m_impl->MaybeReleaseBuffersOnChannel(channel);
}

Envelope::ptr_t Channel::BasicConsumeMessage(const std::string &consumer_tag) {
  Envelope::ptr_t returnval;
  BasicConsumeMessage(consumer_tag, returnval);
  return returnval;
}

Envelope::ptr_t Channel::BasicConsumeMessage(
    const std::vector<std::string> &consumer_tags) {
  Envelope::ptr_t returnval;
  BasicConsumeMessage(consumer_tags, returnval);
  return returnval;
}

Envelope::ptr_t Channel::BasicConsumeMessage() {
  Envelope::ptr_t returnval;
  BasicConsumeMessage(returnval);
  return returnval;
}

bool Channel::BasicConsumeMessage(const std::string &consumer_tag,
                                  Envelope::ptr_t &message, int timeout) {
  m_impl->CheckIsConnected();
  amqp_channel_t channel = m_impl->GetConsumerChannel(consumer_tag);

  boost::array<amqp_channel_t, 1> channels = {{channel}};

  return m_impl->ConsumeMessageOnChannel(channels, message, timeout);
}

bool Channel::BasicConsumeMessage(const std::vector<std::string> &consumer_tags,
                                  Envelope::ptr_t &message, int timeout) {
  m_impl->CheckIsConnected();

  std::vector<amqp_channel_t> channels;
  channels.reserve(consumer_tags.size());

  for (std::vector<std::string>::const_iterator it = consumer_tags.begin();
       it != consumer_tags.end(); ++it) {
    channels.push_back(m_impl->GetConsumerChannel(*it));
  }

  return m_impl->ConsumeMessageOnChannel(channels, message, timeout);
}

bool Channel::BasicConsumeMessage(Envelope::ptr_t &message, int timeout) {
  m_impl->CheckIsConnected();

  std::vector<amqp_channel_t> channels = m_impl->GetAllConsumerChannels();

  if (0 == channels.size()) {
    throw ConsumerTagNotFoundException();
  }

  return m_impl->ConsumeMessageOnChannel(channels, message, timeout);
}

}  // namespace AmqpClient
