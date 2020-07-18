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
#include "SimpleAmqpClient/BasicMessage.h"

#include <amqp.h>
#include <amqp_framing.h>

#include <boost/optional/optional.hpp>
#include <cstdint>
#include <cstring>
#include <string>

#include "SimpleAmqpClient/TableImpl.h"

namespace AmqpClient {

struct BasicMessage::Impl {
  std::string body;
  boost::optional<std::string> content_type;
  boost::optional<std::string> content_encoding;
  boost::optional<delivery_mode_t> delivery_mode;
  boost::optional<std::uint8_t> priority;
  boost::optional<std::string> correlation_id;
  boost::optional<std::string> reply_to;
  boost::optional<std::string> expiration;
  boost::optional<std::string> message_id;
  boost::optional<std::uint64_t> timestamp;
  boost::optional<std::string> type;
  boost::optional<std::string> user_id;
  boost::optional<std::string> app_id;
  boost::optional<std::string> cluster_id;
  boost::optional<Table> header_table;
};

BasicMessage::BasicMessage() : m_impl(new Impl) {}

BasicMessage::BasicMessage(const std::string& body) : m_impl(new Impl) {
  Body(body);
}

BasicMessage::~BasicMessage() {}

const std::string& BasicMessage::Body() const { return m_impl->body; }

std::string& BasicMessage::Body() { return m_impl->body; }

void BasicMessage::Body(const std::string& body) { m_impl->body = body; }

const std::string& BasicMessage::ContentType() const {
  if (ContentTypeIsSet()) {
    return m_impl->content_type.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::ContentType(const std::string& content_type) {
  m_impl->content_type = content_type;
}

bool BasicMessage::ContentTypeIsSet() const {
  return m_impl->content_type.is_initialized();
}

void BasicMessage::ContentTypeClear() { m_impl->content_type.reset(); }

const std::string& BasicMessage::ContentEncoding() const {
  if (ContentEncodingIsSet()) {
    return m_impl->content_encoding.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::ContentEncoding(const std::string& content_encoding) {
  m_impl->content_encoding = content_encoding;
}

bool BasicMessage::ContentEncodingIsSet() const {
  return m_impl->content_encoding.is_initialized();
}

void BasicMessage::ContentEncodingClear() { m_impl->content_encoding.reset(); }

BasicMessage::delivery_mode_t BasicMessage::DeliveryMode() const {
  return m_impl->delivery_mode.value_or(dm_notset);
}

void BasicMessage::DeliveryMode(delivery_mode_t delivery_mode) {
  m_impl->delivery_mode = delivery_mode;
}

bool BasicMessage::DeliveryModeIsSet() const {
  return m_impl->delivery_mode.is_initialized();
}

void BasicMessage::DeliveryModeClear() { m_impl->delivery_mode.reset(); }

std::uint8_t BasicMessage::Priority() const {
  return m_impl->priority.value_or(0);
}

void BasicMessage::Priority(std::uint8_t priority) {
  m_impl->priority = priority;
}

bool BasicMessage::PriorityIsSet() const {
  return m_impl->priority.is_initialized();
}

void BasicMessage::PriorityClear() { m_impl->priority.reset(); }

const std::string& BasicMessage::CorrelationId() const {
  if (CorrelationIdIsSet()) {
    return m_impl->correlation_id.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::CorrelationId(const std::string& correlation_id) {
  m_impl->correlation_id = correlation_id;
}

bool BasicMessage::CorrelationIdIsSet() const {
  return m_impl->correlation_id.is_initialized();
}

void BasicMessage::CorrelationIdClear() { m_impl->correlation_id.reset(); }

const std::string& BasicMessage::ReplyTo() const {
  if (ReplyToIsSet()) {
    return m_impl->reply_to.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::ReplyTo(const std::string& reply_to) {
  m_impl->reply_to = reply_to;
}

bool BasicMessage::ReplyToIsSet() const {
  return m_impl->reply_to.is_initialized();
}

void BasicMessage::ReplyToClear() { m_impl->reply_to.reset(); }

const std::string& BasicMessage::Expiration() const {
  if (ExpirationIsSet()) {
    return m_impl->expiration.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::Expiration(const std::string& expiration) {
  m_impl->expiration = expiration;
}

bool BasicMessage::ExpirationIsSet() const {
  return m_impl->expiration.is_initialized();
}

void BasicMessage::ExpirationClear() { m_impl->expiration.reset(); }

const std::string& BasicMessage::MessageId() const {
  if (MessageIdIsSet()) {
    return m_impl->message_id.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::MessageId(const std::string& message_id) {
  m_impl->message_id = message_id;
}

bool BasicMessage::MessageIdIsSet() const {
  return m_impl->message_id.is_initialized();
}

void BasicMessage::MessageIdClear() { m_impl->message_id.reset(); }

std::uint64_t BasicMessage::Timestamp() const {
  return m_impl->timestamp.value_or(0);
}

void BasicMessage::Timestamp(std::uint64_t timestamp) {
  m_impl->timestamp = timestamp;
}

bool BasicMessage::TimestampIsSet() const {
  return m_impl->timestamp.is_initialized();
}

void BasicMessage::TimestampClear() { m_impl->timestamp.reset(); }

const std::string& BasicMessage::Type() const {
  if (TypeIsSet()) {
    return m_impl->type.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::Type(const std::string& type) { m_impl->type = type; }

bool BasicMessage::TypeIsSet() const { return m_impl->type.is_initialized(); }

void BasicMessage::TypeClear() { m_impl->type.reset(); }

const std::string& BasicMessage::UserId() const {
  if (UserIdIsSet()) {
    return m_impl->user_id.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::UserId(const std::string& user_id) {
  m_impl->user_id = user_id;
}

bool BasicMessage::UserIdIsSet() const {
  return m_impl->user_id.is_initialized();
}

void BasicMessage::UserIdClear() { m_impl->user_id.reset(); }

const std::string& BasicMessage::AppId() const {
  if (AppIdIsSet()) {
    return m_impl->app_id.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::AppId(const std::string& app_id) { m_impl->app_id = app_id; }

bool BasicMessage::AppIdIsSet() const {
  return m_impl->app_id.is_initialized();
}

void BasicMessage::AppIdClear() { m_impl->app_id.reset(); }

const std::string& BasicMessage::ClusterId() const {
  if (ClusterIdIsSet()) {
    return m_impl->cluster_id.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::ClusterId(const std::string& cluster_id) {
  m_impl->cluster_id = cluster_id;
}

bool BasicMessage::ClusterIdIsSet() const {
  return m_impl->cluster_id.is_initialized();
}

void BasicMessage::ClusterIdClear() { m_impl->cluster_id.reset(); }

Table& BasicMessage::HeaderTable() {
  if (!HeaderTableIsSet()) {
    m_impl->header_table = Table();
  }
  return m_impl->header_table.value();
}

const Table& BasicMessage::HeaderTable() const {
  if (HeaderTableIsSet()) {
    return m_impl->header_table.value();
  }
  static const Table empty;
  return empty;
}

void BasicMessage::HeaderTable(const Table& header_table) {
  m_impl->header_table = header_table;
}

bool BasicMessage::HeaderTableIsSet() const {
  return m_impl->header_table.is_initialized();
}

void BasicMessage::HeaderTableClear() { m_impl->header_table.reset(); }

}  // namespace AmqpClient
