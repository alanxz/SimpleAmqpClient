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

#include <cstdint>
#include <cstring>

namespace AmqpClient {

struct BasicMessage::Impl {
  Impl()
      : m_deliveryMode(dm_notset),
      m_priority(0),
      m_timestamp(0),
      m_contentTypePresent(false),
      m_contentEncodingPresent(false),
      m_headersPresent(false),
      m_deliveryModePresent(false),
      m_priorityPresent(false),
      m_correlationIdPresent(false),
      m_replyToPresent(false),
      m_expirationPresent(false),
      m_messageIdPresent(false),
      m_timestampPresent(false),
      m_typePresent(false),
      m_userIdPresent(false),
      m_appIdPresent(false),
      m_clusterIdPresent(false) {}

  std::string m_body;

  std::string m_contentType;
  std::string m_contentEncoding;
  Table m_headers;
  BasicMessage::delivery_mode_t m_deliveryMode;
  std::uint8_t m_priority;
  std::string m_correlationId;
  std::string m_replyTo;
  std::string m_expiration;
  std::string m_messageId;
  std::uint64_t m_timestamp;
  std::string m_type;
  std::string m_userId;
  std::string m_appId;
  std::string m_clusterId;

  bool m_contentTypePresent : 1;
  bool m_contentEncodingPresent : 1;
  bool m_headersPresent : 1;
  bool m_deliveryModePresent : 1;
  bool m_priorityPresent : 1;
  bool m_correlationIdPresent : 1;
  bool m_replyToPresent : 1;
  bool m_expirationPresent : 1;
  bool m_messageIdPresent : 1;
  bool m_timestampPresent : 1;
  bool m_typePresent : 1;
  bool m_userIdPresent : 1;
  bool m_appIdPresent : 1;
  bool m_clusterIdPresent : 1;
};

BasicMessage::BasicMessage() : m_impl(new Impl) {}

BasicMessage::BasicMessage(const std::string &body)
    : m_impl(new Impl) {
  m_impl->m_body = body;
}

BasicMessage::BasicMessage(const BasicMessage &other)
    : m_impl(new Impl(*other.m_impl)) {}

BasicMessage::BasicMessage(BasicMessage &&other) noexcept
  : m_impl(std::move(other.m_impl)) {}

BasicMessage &BasicMessage::operator=(const BasicMessage &other) {
  if (this != &other) {
    m_impl.reset(new Impl(*other.m_impl));
  }
  return *this;
}

BasicMessage &BasicMessage::operator=(BasicMessage &&other) noexcept {
  if (this != &other) {
    m_impl = std::move(other.m_impl);
  }
  return *this;
}

BasicMessage::~BasicMessage() = default;

std::string BasicMessage::Body() const {
  return m_impl->m_body;
}

void BasicMessage::Body(const std::string &body) {
  m_impl->m_body = body;
}

std::string BasicMessage::ContentType() const {
  if (m_impl->m_contentTypePresent) {
    return m_impl->m_contentType;
  }
  return std::string();
}

void BasicMessage::ContentType(const std::string &content_type) {
  m_impl->m_contentType = content_type;
  m_impl->m_contentTypePresent = true;
}

bool BasicMessage::ContentTypeIsSet() const {
  return m_impl->m_contentTypePresent;
}

void BasicMessage::ContentTypeClear() {
  m_impl->m_contentTypePresent = false;
}

std::string BasicMessage::ContentEncoding() const {
  if (m_impl->m_contentEncodingPresent) {
    return m_impl->m_contentEncoding;
  }
  return std::string();
}

void BasicMessage::ContentEncoding(const std::string &content_encoding) {
  m_impl->m_contentEncoding = content_encoding;
  m_impl->m_contentEncodingPresent = true;
}

bool BasicMessage::ContentEncodingIsSet() const {
  return m_impl->m_contentEncodingPresent;
}

void BasicMessage::ContentEncodingClear() {
  m_impl->m_contentEncodingPresent = false;
}

BasicMessage::delivery_mode_t BasicMessage::DeliveryMode() const {
  if (m_impl->m_deliveryModePresent) {
    return m_impl->m_deliveryMode;
  }
  return dm_nonpersistent;
}

void BasicMessage::DeliveryMode(delivery_mode_t delivery_mode) {
  m_impl->m_deliveryMode = delivery_mode;
  m_impl->m_deliveryModePresent = true;
}

bool BasicMessage::DeliveryModeIsSet() const {
  return m_impl->m_deliveryModePresent;
}

void BasicMessage::DeliveryModeClear() {
  m_impl->m_deliveryModePresent = false;
}

std::uint8_t BasicMessage::Priority() const {
  if (m_impl->m_priorityPresent) {
    return m_impl->m_priority;
  }
  return 0;
}
void BasicMessage::Priority(std::uint8_t priority) {
  m_impl->m_priority = priority;
  m_impl->m_priorityPresent = true;
}

bool BasicMessage::PriorityIsSet() const {
  return m_impl->m_priorityPresent;
}

void BasicMessage::PriorityClear() {
  m_impl->m_priorityPresent = false;
}

std::string BasicMessage::CorrelationId() const {
  if (m_impl->m_correlationIdPresent) {
    return m_impl->m_correlationId;
  }
  return std::string();
}

void BasicMessage::CorrelationId(const std::string &correlation_id) {
  m_impl->m_correlationId = correlation_id;
  m_impl->m_correlationIdPresent = true;
}

bool BasicMessage::CorrelationIdIsSet() const {
  return m_impl->m_correlationIdPresent;
}

void BasicMessage::CorrelationIdClear() {
  m_impl->m_correlationIdPresent = false;
}

std::string BasicMessage::ReplyTo() const {
  if (m_impl->m_replyToPresent) {
    return m_impl->m_replyTo;
  }
  return std::string();
}
void BasicMessage::ReplyTo(const std::string &reply_to) {
  m_impl->m_replyTo = reply_to;
  m_impl->m_replyToPresent = true;
}

bool BasicMessage::ReplyToIsSet() const {
  return m_impl->m_replyToPresent;
}

void BasicMessage::ReplyToClear() {
  m_impl->m_replyToPresent = false;
}

std::string BasicMessage::Expiration() const {
  if (m_impl->m_expirationPresent) {
    return m_impl->m_expiration;
  }
  return std::string();
}

void BasicMessage::Expiration(const std::string &expiration) {
  m_impl->m_expiration = expiration;
  m_impl->m_expirationPresent = true;
}

bool BasicMessage::ExpirationIsSet() const {
  return m_impl->m_expirationPresent;
}

void BasicMessage::ExpirationClear() {
  m_impl->m_expirationPresent = false;
}

std::string BasicMessage::MessageId() const {
  if (m_impl->m_messageIdPresent) {
    return m_impl->m_messageId;
  }
  return std::string();
}

void BasicMessage::MessageId(const std::string &message_id) {
  m_impl->m_messageId = message_id;
  m_impl->m_messageIdPresent = true;
}

bool BasicMessage::MessageIdIsSet() const {
  return m_impl->m_messageIdPresent;
}

void BasicMessage::MessageIdClear() {
  m_impl->m_messageIdPresent = false;
}

std::uint64_t BasicMessage::Timestamp() const {
  if (m_impl->m_timestampPresent) {
    return m_impl->m_timestamp;
  }
  return 0;
}

void BasicMessage::Timestamp(std::uint64_t timestamp) {
  m_impl->m_timestamp = timestamp;
  m_impl->m_timestampPresent = true;
}

bool BasicMessage::TimestampIsSet() const {
  return m_impl->m_timestampPresent;
}

void BasicMessage::TimestampClear() {
  m_impl->m_timestampPresent = false;
}

std::string BasicMessage::Type() const {
  if (m_impl->m_typePresent) {
    return m_impl->m_type;
  }
  return std::string();
}

void BasicMessage::Type(const std::string &type) {
  m_impl->m_type = type;
  m_impl->m_typePresent = true;
}

bool BasicMessage::TypeIsSet() const {
  return m_impl->m_typePresent;
}

void BasicMessage::TypeClear() {
  m_impl->m_typePresent = false;
}

std::string BasicMessage::UserId() const {
  if (m_impl->m_userIdPresent) {
    return m_impl->m_userId;
  }
  return std::string();
}

void BasicMessage::UserId(const std::string &user_id) {
  m_impl->m_userId = user_id;
  m_impl->m_userIdPresent = true;
}

bool BasicMessage::UserIdIsSet() const {
  return m_impl->m_userIdPresent;
}

void BasicMessage::UserIdClear() {
  m_impl->m_userIdPresent = false;
}

std::string BasicMessage::AppId() const {
  if (m_impl->m_appIdPresent) {
    return m_impl->m_appId;
  }
  return std::string();
}

void BasicMessage::AppId(const std::string &app_id) {
  m_impl->m_appId = app_id;
  m_impl->m_appIdPresent = true;
}

bool BasicMessage::AppIdIsSet() const {
  return m_impl->m_appIdPresent;
}

void BasicMessage::AppIdClear() {
  m_impl->m_appIdPresent = false;
}

std::string BasicMessage::ClusterId() const {
  if (m_impl->m_clusterIdPresent) {
    return m_impl->m_clusterId;
  }
  return std::string();
}

void BasicMessage::ClusterId(const std::string &cluster_id) {
  m_impl->m_clusterId = cluster_id;
  m_impl->m_clusterIdPresent = true;
}

bool BasicMessage::ClusterIdIsSet() const {
  return m_impl->m_clusterIdPresent;
}

void BasicMessage::ClusterIdClear() {
  m_impl->m_clusterIdPresent = false;
}

Table BasicMessage::HeaderTable() const {
  if (m_impl->m_headersPresent) {
    return m_impl->m_headers;
  }
  return Table();
}

void BasicMessage::HeaderTable(const Table &header_table) {
  m_impl->m_headers = header_table;
  m_impl->m_headersPresent = true;
}

bool BasicMessage::HeaderTableIsSet() const {
  return m_impl->m_headersPresent;
}

void BasicMessage::HeaderTableClear() {
  m_impl->m_headersPresent = false;
}

}  // namespace AmqpClient
