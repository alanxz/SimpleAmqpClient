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

#include "SimpleAmqpClient/BasicMessage.h"
#include "SimpleAmqpClient/TableImpl.h"

#include <cstring>

namespace AmqpClient {

namespace Detail {

class BasicMessageImpl {
 public:
  BasicMessageImpl() : m_properties(), m_body() {}
  amqp_basic_properties_t m_properties;
  amqp_bytes_t m_body;
  amqp_pool_ptr_t m_table_pool;
};
}

BasicMessage::BasicMessage() : m_impl(new Detail::BasicMessageImpl) {
  m_impl->m_body.bytes = NULL;
  m_impl->m_body.len = 0;
  m_impl->m_properties._flags = 0;
}

BasicMessage::BasicMessage(const std::string &body)
    : m_impl(new Detail::BasicMessageImpl) {
  Body(body);
  m_impl->m_properties._flags = 0;
}

BasicMessage::BasicMessage(const amqp_bytes_t &body,
                           const amqp_basic_properties_t *properties)
    : m_impl(new Detail::BasicMessageImpl) {
  m_impl->m_body = body;
  m_impl->m_properties = *properties;
  if (ContentTypeIsSet())
    m_impl->m_properties.content_type =
        amqp_bytes_malloc_dup(m_impl->m_properties.content_type);
  if (ContentEncodingIsSet())
    m_impl->m_properties.content_encoding =
        amqp_bytes_malloc_dup(m_impl->m_properties.content_encoding);
  if (CorrelationIdIsSet())
    m_impl->m_properties.correlation_id =
        amqp_bytes_malloc_dup(m_impl->m_properties.correlation_id);
  if (ReplyToIsSet())
    m_impl->m_properties.reply_to =
        amqp_bytes_malloc_dup(m_impl->m_properties.reply_to);
  if (ExpirationIsSet())
    m_impl->m_properties.expiration =
        amqp_bytes_malloc_dup(m_impl->m_properties.expiration);
  if (MessageIdIsSet())
    m_impl->m_properties.message_id =
        amqp_bytes_malloc_dup(m_impl->m_properties.message_id);
  if (TypeIsSet())
    m_impl->m_properties.type =
        amqp_bytes_malloc_dup(m_impl->m_properties.type);
  if (UserIdIsSet())
    m_impl->m_properties.user_id =
        amqp_bytes_malloc_dup(m_impl->m_properties.user_id);
  if (AppIdIsSet())
    m_impl->m_properties.app_id =
        amqp_bytes_malloc_dup(m_impl->m_properties.app_id);
  if (ClusterIdIsSet())
    m_impl->m_properties.cluster_id =
        amqp_bytes_malloc_dup(m_impl->m_properties.cluster_id);
  if (HeaderTableIsSet())
    m_impl->m_properties.headers = Detail::TableValueImpl::CopyTable(
        m_impl->m_properties.headers, m_impl->m_table_pool);
}

BasicMessage::~BasicMessage() {
  amqp_bytes_free(m_impl->m_body);
  if (ContentTypeIsSet()) amqp_bytes_free(m_impl->m_properties.content_type);
  if (ContentEncodingIsSet())
    amqp_bytes_free(m_impl->m_properties.content_encoding);
  if (CorrelationIdIsSet())
    amqp_bytes_free(m_impl->m_properties.correlation_id);
  if (ReplyToIsSet()) amqp_bytes_free(m_impl->m_properties.reply_to);
  if (ExpirationIsSet()) amqp_bytes_free(m_impl->m_properties.expiration);
  if (MessageIdIsSet()) amqp_bytes_free(m_impl->m_properties.message_id);
  if (TypeIsSet()) amqp_bytes_free(m_impl->m_properties.type);
  if (UserIdIsSet()) amqp_bytes_free(m_impl->m_properties.user_id);
  if (AppIdIsSet()) amqp_bytes_free(m_impl->m_properties.app_id);
  if (ClusterIdIsSet()) amqp_bytes_free(m_impl->m_properties.cluster_id);
}

const amqp_basic_properties_t *BasicMessage::getAmqpProperties() const {
  return &m_impl->m_properties;
}

const amqp_bytes_t &BasicMessage::getAmqpBody() const { return m_impl->m_body; }

std::string BasicMessage::Body() const {
  if (m_impl->m_body.bytes == NULL)
    return std::string();
  return std::string((char *)m_impl->m_body.bytes, m_impl->m_body.len);
}
void BasicMessage::Body(const std::string &body) {
  if (NULL != m_impl->m_body.bytes) {
    amqp_bytes_free(m_impl->m_body);
  }
  amqp_bytes_t body_bytes;
  body_bytes.bytes = const_cast<char *>(body.data());
  body_bytes.len = body.length();
  m_impl->m_body = amqp_bytes_malloc_dup(body_bytes);
}

std::string BasicMessage::ContentType() const {
  if (ContentTypeIsSet())
    return std::string((char *)m_impl->m_properties.content_type.bytes,
                       m_impl->m_properties.content_type.len);
  return std::string();
}

void BasicMessage::ContentType(const std::string &content_type) {
  if (ContentTypeIsSet()) amqp_bytes_free(m_impl->m_properties.content_type);
  m_impl->m_properties.content_type =
      amqp_bytes_malloc_dup(amqp_cstring_bytes(content_type.c_str()));
  m_impl->m_properties._flags |= AMQP_BASIC_CONTENT_TYPE_FLAG;
}

bool BasicMessage::ContentTypeIsSet() const {
  return AMQP_BASIC_CONTENT_TYPE_FLAG ==
         (m_impl->m_properties._flags & AMQP_BASIC_CONTENT_TYPE_FLAG);
}

void BasicMessage::ContentTypeClear() {
  if (ContentTypeIsSet()) amqp_bytes_free(m_impl->m_properties.content_type);
  m_impl->m_properties._flags &= ~AMQP_BASIC_CONTENT_TYPE_FLAG;
}

std::string BasicMessage::ContentEncoding() const {
  if (ContentEncodingIsSet())
    return std::string((char *)m_impl->m_properties.content_encoding.bytes,
                       m_impl->m_properties.content_encoding.len);
  return std::string();
}

void BasicMessage::ContentEncoding(const std::string &content_encoding) {
  if (ContentEncodingIsSet())
    amqp_bytes_free(m_impl->m_properties.content_encoding);
  m_impl->m_properties.content_encoding =
      amqp_bytes_malloc_dup(amqp_cstring_bytes(content_encoding.c_str()));
  m_impl->m_properties._flags |= AMQP_BASIC_CONTENT_ENCODING_FLAG;
}

bool BasicMessage::ContentEncodingIsSet() const {
  return AMQP_BASIC_CONTENT_ENCODING_FLAG ==
         (m_impl->m_properties._flags & AMQP_BASIC_CONTENT_ENCODING_FLAG);
}

void BasicMessage::ContentEncodingClear() {
  if (ContentEncodingIsSet())
    amqp_bytes_free(m_impl->m_properties.content_encoding);
  m_impl->m_properties._flags &= ~AMQP_BASIC_CONTENT_ENCODING_FLAG;
}

BasicMessage::delivery_mode_t BasicMessage::DeliveryMode() const {
  if (DeliveryModeIsSet())
    return (delivery_mode_t)m_impl->m_properties.delivery_mode;
  return (delivery_mode_t)0;
}

void BasicMessage::DeliveryMode(delivery_mode_t delivery_mode) {
  m_impl->m_properties.delivery_mode = static_cast<uint8_t>(delivery_mode);
  m_impl->m_properties._flags |= AMQP_BASIC_DELIVERY_MODE_FLAG;
}

bool BasicMessage::DeliveryModeIsSet() const {
  return AMQP_BASIC_DELIVERY_MODE_FLAG ==
         (m_impl->m_properties._flags & AMQP_BASIC_DELIVERY_MODE_FLAG);
}

void BasicMessage::DeliveryModeClear() {
  m_impl->m_properties._flags &= ~AMQP_BASIC_DELIVERY_MODE_FLAG;
}

boost::uint8_t BasicMessage::Priority() const {
  if (PriorityIsSet())
    return m_impl->m_properties.priority;
  return 0;
}
void BasicMessage::Priority(boost::uint8_t priority) {
  m_impl->m_properties.priority = priority;
  m_impl->m_properties._flags |= AMQP_BASIC_PRIORITY_FLAG;
}

bool BasicMessage::PriorityIsSet() const {
  return AMQP_BASIC_PRIORITY_FLAG ==
         (m_impl->m_properties._flags & AMQP_BASIC_PRIORITY_FLAG);
}

void BasicMessage::PriorityClear() {
  m_impl->m_properties._flags &= ~AMQP_BASIC_PRIORITY_FLAG;
}

std::string BasicMessage::CorrelationId() const {
  if (CorrelationIdIsSet())
    return std::string((char *)m_impl->m_properties.correlation_id.bytes,
                       m_impl->m_properties.correlation_id.len);
  return std::string();
}

void BasicMessage::CorrelationId(const std::string &correlation_id) {
  if (CorrelationIdIsSet())
    amqp_bytes_free(m_impl->m_properties.correlation_id);
  m_impl->m_properties.correlation_id =
      amqp_bytes_malloc_dup(amqp_cstring_bytes(correlation_id.c_str()));
  m_impl->m_properties._flags |= AMQP_BASIC_CORRELATION_ID_FLAG;
}

bool BasicMessage::CorrelationIdIsSet() const {
  return AMQP_BASIC_CORRELATION_ID_FLAG ==
         (m_impl->m_properties._flags & AMQP_BASIC_CORRELATION_ID_FLAG);
}

void BasicMessage::CorrelationIdClear() {
  if (CorrelationIdIsSet())
    amqp_bytes_free(m_impl->m_properties.correlation_id);
  m_impl->m_properties._flags &= ~AMQP_BASIC_CORRELATION_ID_FLAG;
}

std::string BasicMessage::ReplyTo() const {
  if (ReplyToIsSet())
    return std::string((char *)m_impl->m_properties.reply_to.bytes,
                       m_impl->m_properties.reply_to.len);
  return std::string();
}
void BasicMessage::ReplyTo(const std::string &reply_to) {
  if (ReplyToIsSet()) amqp_bytes_free(m_impl->m_properties.reply_to);
  m_impl->m_properties.reply_to =
      amqp_bytes_malloc_dup(amqp_cstring_bytes(reply_to.c_str()));
  m_impl->m_properties._flags |= AMQP_BASIC_REPLY_TO_FLAG;
}

bool BasicMessage::ReplyToIsSet() const {
  return AMQP_BASIC_REPLY_TO_FLAG ==
         (m_impl->m_properties._flags & AMQP_BASIC_REPLY_TO_FLAG);
}

void BasicMessage::ReplyToClear() {
  if (ReplyToIsSet()) amqp_bytes_free(m_impl->m_properties.reply_to);
  m_impl->m_properties._flags &= ~AMQP_BASIC_REPLY_TO_FLAG;
}

std::string BasicMessage::Expiration() const {
  if (ExpirationIsSet())
    return std::string((char *)m_impl->m_properties.expiration.bytes,
                       m_impl->m_properties.expiration.len);
  return std::string();
}
void BasicMessage::Expiration(const std::string &expiration) {
  if (ExpirationIsSet()) amqp_bytes_free(m_impl->m_properties.expiration);
  m_impl->m_properties.expiration =
      amqp_bytes_malloc_dup(amqp_cstring_bytes(expiration.c_str()));
  m_impl->m_properties._flags |= AMQP_BASIC_EXPIRATION_FLAG;
}

bool BasicMessage::ExpirationIsSet() const {
  return AMQP_BASIC_EXPIRATION_FLAG ==
         (m_impl->m_properties._flags & AMQP_BASIC_EXPIRATION_FLAG);
}

void BasicMessage::ExpirationClear() {
  if (ExpirationIsSet()) amqp_bytes_free(m_impl->m_properties.expiration);
  m_impl->m_properties._flags &= ~AMQP_BASIC_EXPIRATION_FLAG;
}

std::string BasicMessage::MessageId() const {
  if (MessageIdIsSet())
    return std::string((char *)m_impl->m_properties.message_id.bytes,
                       m_impl->m_properties.message_id.len);
  return std::string();
}
void BasicMessage::MessageId(const std::string &message_id) {
  if (MessageIdIsSet()) amqp_bytes_free(m_impl->m_properties.message_id);
  m_impl->m_properties.message_id =
      amqp_bytes_malloc_dup(amqp_cstring_bytes(message_id.c_str()));
  m_impl->m_properties._flags |= AMQP_BASIC_MESSAGE_ID_FLAG;
}

bool BasicMessage::MessageIdIsSet() const {
  return AMQP_BASIC_MESSAGE_ID_FLAG ==
         (m_impl->m_properties._flags & AMQP_BASIC_MESSAGE_ID_FLAG);
}

void BasicMessage::MessageIdClear() {
  if (MessageIdIsSet()) amqp_bytes_free(m_impl->m_properties.message_id);
  m_impl->m_properties._flags &= ~AMQP_BASIC_MESSAGE_ID_FLAG;
}

boost::uint64_t BasicMessage::Timestamp() const {
  if (TimestampIsSet())
    return m_impl->m_properties.timestamp;
  return 0;
}
void BasicMessage::Timestamp(boost::uint64_t timestamp) {
  m_impl->m_properties.timestamp = timestamp;
  m_impl->m_properties._flags |= AMQP_BASIC_TIMESTAMP_FLAG;
}

bool BasicMessage::TimestampIsSet() const {
  return AMQP_BASIC_TIMESTAMP_FLAG ==
         (m_impl->m_properties._flags & AMQP_BASIC_TIMESTAMP_FLAG);
}

void BasicMessage::TimestampClear() {
  m_impl->m_properties._flags &= ~AMQP_BASIC_TIMESTAMP_FLAG;
}

std::string BasicMessage::Type() const {
  if (TypeIsSet())
    return std::string((char *)m_impl->m_properties.type.bytes,
                       m_impl->m_properties.type.len);
  return std::string();
}
void BasicMessage::Type(const std::string &type) {
  if (TypeIsSet()) amqp_bytes_free(m_impl->m_properties.type);
  m_impl->m_properties.type =
      amqp_bytes_malloc_dup(amqp_cstring_bytes(type.c_str()));
  m_impl->m_properties._flags |= AMQP_BASIC_TYPE_FLAG;
}

bool BasicMessage::TypeIsSet() const {
  return AMQP_BASIC_TYPE_FLAG ==
         (m_impl->m_properties._flags & AMQP_BASIC_TYPE_FLAG);
}

void BasicMessage::TypeClear() {
  if (TypeIsSet()) amqp_bytes_free(m_impl->m_properties.type);
  m_impl->m_properties._flags &= ~AMQP_BASIC_TYPE_FLAG;
}

std::string BasicMessage::UserId() const {
  if (UserIdIsSet())
    return std::string((char *)m_impl->m_properties.user_id.bytes,
                       m_impl->m_properties.user_id.len);
  return std::string();
}

void BasicMessage::UserId(const std::string &user_id) {
  if (UserIdIsSet()) amqp_bytes_free(m_impl->m_properties.user_id);
  m_impl->m_properties.user_id =
      amqp_bytes_malloc_dup(amqp_cstring_bytes(user_id.c_str()));
  m_impl->m_properties._flags |= AMQP_BASIC_USER_ID_FLAG;
}

bool BasicMessage::UserIdIsSet() const {
  return AMQP_BASIC_USER_ID_FLAG ==
         (m_impl->m_properties._flags & AMQP_BASIC_USER_ID_FLAG);
}

void BasicMessage::UserIdClear() {
  if (UserIdIsSet()) amqp_bytes_free(m_impl->m_properties.user_id);
  m_impl->m_properties._flags &= ~AMQP_BASIC_USER_ID_FLAG;
}

std::string BasicMessage::AppId() const {
  if (AppIdIsSet())
    return std::string((char *)m_impl->m_properties.app_id.bytes,
                       m_impl->m_properties.app_id.len);
  return std::string();
}
void BasicMessage::AppId(const std::string &app_id) {
  if (AppIdIsSet()) amqp_bytes_free(m_impl->m_properties.app_id);
  m_impl->m_properties.app_id =
      amqp_bytes_malloc_dup(amqp_cstring_bytes(app_id.c_str()));
  m_impl->m_properties._flags |= AMQP_BASIC_APP_ID_FLAG;
}

bool BasicMessage::AppIdIsSet() const {
  return AMQP_BASIC_APP_ID_FLAG ==
         (m_impl->m_properties._flags & AMQP_BASIC_APP_ID_FLAG);
}

void BasicMessage::AppIdClear() {
  if (AppIdIsSet()) amqp_bytes_free(m_impl->m_properties.app_id);
  m_impl->m_properties._flags &= ~AMQP_BASIC_APP_ID_FLAG;
}

std::string BasicMessage::ClusterId() const {
  if (ClusterIdIsSet())
    return std::string((char *)m_impl->m_properties.cluster_id.bytes,
                       m_impl->m_properties.cluster_id.len);
  return std::string();
}
void BasicMessage::ClusterId(const std::string &cluster_id) {
  if (ClusterIdIsSet()) amqp_bytes_free(m_impl->m_properties.cluster_id);
  m_impl->m_properties.cluster_id =
      amqp_bytes_malloc_dup(amqp_cstring_bytes(cluster_id.c_str()));
  m_impl->m_properties._flags |= AMQP_BASIC_CLUSTER_ID_FLAG;
}

bool BasicMessage::ClusterIdIsSet() const {
  return AMQP_BASIC_CLUSTER_ID_FLAG ==
         (m_impl->m_properties._flags & AMQP_BASIC_CLUSTER_ID_FLAG);
}

void BasicMessage::ClusterIdClear() {
  if (ClusterIdIsSet()) amqp_bytes_free(m_impl->m_properties.cluster_id);
  m_impl->m_properties._flags &= ~AMQP_BASIC_CLUSTER_ID_FLAG;
}

Table BasicMessage::HeaderTable() const {
  if (HeaderTableIsSet())
    return Detail::TableValueImpl::CreateTable(m_impl->m_properties.headers);
  return Table();
}

void BasicMessage::HeaderTable(const Table &header_table) {
  m_impl->m_properties.headers = Detail::TableValueImpl::CreateAmqpTable(
      header_table, m_impl->m_table_pool);
  m_impl->m_properties._flags |= AMQP_BASIC_HEADERS_FLAG;
}

bool BasicMessage::HeaderTableIsSet() const {
  return AMQP_BASIC_HEADERS_FLAG ==
         (m_impl->m_properties._flags & AMQP_BASIC_HEADERS_FLAG);
}

void BasicMessage::HeaderTableClear() {
  if (HeaderTableIsSet()) {
    m_impl->m_table_pool.reset();
    m_impl->m_properties.headers.num_entries = 0;
    m_impl->m_properties.headers.entries = NULL;
  }
  m_impl->m_properties._flags &= ~AMQP_BASIC_HEADERS_FLAG;
}

}  // namespace AmqpClient
