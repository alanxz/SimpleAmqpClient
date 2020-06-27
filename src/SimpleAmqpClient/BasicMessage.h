#ifndef SIMPLEAMQPCLIENT_BASICMESSAGE_H
#define SIMPLEAMQPCLIENT_BASICMESSAGE_H
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

#include <boost/cstdint.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <memory>
#include <string>

#include "SimpleAmqpClient/Table.h"
#include "SimpleAmqpClient/Util.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4275 4251)
#endif

/// @file SimpleAmqpClient/BasicMessage.h
/// The AmqpClient::BasicMessage class is defined in this header file.

namespace AmqpClient {

/**
 * An AMQP BasicMessage
 */
class SIMPLEAMQPCLIENT_EXPORT BasicMessage : boost::noncopyable {
 public:
  /// A shared pointer to BasicMessage
  typedef boost::shared_ptr<BasicMessage> ptr_t;

  /// With durable queues, messages can be requested to persist or not
  enum delivery_mode_t {
    dm_notset = 0,
    dm_nonpersistent = 1,
    dm_persistent = 2
  };

  /**
   * Create a new empty BasicMessage object
   */
  static ptr_t Create() { return boost::make_shared<BasicMessage>(); }

  /**
   * Create a new BasicMessage object with given body
   *
   * @param body the message body.
   * @returns a new BasicMessage object
   */
  static ptr_t Create(const std::string& body) {
    return boost::make_shared<BasicMessage>(body);
  }

  /// Construct empty BasicMessage
  BasicMessage();
  /// Construct BasicMessage with given body
  BasicMessage(const std::string& body);

 public:
  /**
   * Destructor
   */
  virtual ~BasicMessage();

  /**
   * Gets the message body as a std::string
   */
  const std::string& Body() const;
  std::string& Body();

  /**
   * Sets the message body as a std::string
   */
  void Body(const std::string& body);

  /**
   * Gets the content type property
   */
  const std::string& ContentType() const;
  /**
   * Sets the content type property
   */
  void ContentType(const std::string& content_type);
  /**
   * Determines whether the content type property is set
   */
  bool ContentTypeIsSet() const;
  /**
   * Unsets the content type property if it is set
   */
  void ContentTypeClear();

  /**
   * Gets the content encoding property
   */
  const std::string& ContentEncoding() const;
  /**
   * Sets the content encoding property
   */
  void ContentEncoding(const std::string& content_encoding);
  /**
   * Determines whether the content encoding property is set
   */
  bool ContentEncodingIsSet() const;
  /**
   * Unsets the content encoding property if it is set
   */
  void ContentEncodingClear();

  /**
   * Gets the delivery mode property
   */
  delivery_mode_t DeliveryMode() const;
  /**
   * Sets the delivery mode property
   */
  void DeliveryMode(delivery_mode_t delivery_mode);
  /**
   * Determines whether the delivery mode property is set
   */
  bool DeliveryModeIsSet() const;
  /**
   * Unsets the delivery mode property if it is set
   */
  void DeliveryModeClear();

  /**
   * Gets the priority property
   */
  boost::uint8_t Priority() const;
  /**
   * Sets the priority property
   */
  void Priority(boost::uint8_t priority);
  /**
   * Determines whether the priority property is set
   */
  bool PriorityIsSet() const;
  /**
   * Unsets the priority property if it is set
   */
  void PriorityClear();

  /**
   * Gets the correlation id property
   */
  const std::string& CorrelationId() const;
  /**
   * Sets the correlation id property
   */
  void CorrelationId(const std::string& correlation_id);
  /**
   * Determines whether the correlation id property is set
   */
  bool CorrelationIdIsSet() const;
  /**
   * Unsets the correlation id property
   */
  void CorrelationIdClear();

  /**
   * Gets the reply to property
   */
  const std::string& ReplyTo() const;
  /**
   * Sets the reply to property
   */
  void ReplyTo(const std::string& reply_to);
  /**
   * Determines whether the reply to property is set
   */
  bool ReplyToIsSet() const;
  /**
   * Unsets the reply to property
   */
  void ReplyToClear();

  /**
   * Gets the expiration property
   */
  const std::string& Expiration() const;
  /**
   * Sets the expiration property
   */
  void Expiration(const std::string& expiration);
  /**
   * Determines whether the expiration property is set
   */
  bool ExpirationIsSet() const;
  /**
   * Unsets the expiration property
   */
  void ExpirationClear();

  /**
   * Gets the message id property
   */
  const std::string& MessageId() const;
  /**
   * Sets the message id property
   */
  void MessageId(const std::string& message_id);
  /**
   * Determines if the message id property is set
   */
  bool MessageIdIsSet() const;
  /**
   * Unsets the message id property
   */
  void MessageIdClear();

  /**
   * Gets the timestamp property
   */
  boost::uint64_t Timestamp() const;
  /**
   * Sets the timestamp property
   */
  void Timestamp(boost::uint64_t timestamp);
  /**
   * Determines whether the timestamp property is set
   */
  bool TimestampIsSet() const;
  /**
   * Unsets the timestamp property
   */
  void TimestampClear();

  /**
   * Gets the type property
   */
  const std::string& Type() const;
  /**
   * Sets the type property
   */
  void Type(const std::string& type);
  /**
   * Determines whether the type property is set
   */
  bool TypeIsSet() const;
  /**
   * Unsets the type property
   */
  void TypeClear();

  /**
   * Gets the user id property
   */
  const std::string& UserId() const;
  /**
   * Sets the user id property
   */
  void UserId(const std::string& user_id);
  /**
   * Determines whether the user id property is set
   */
  bool UserIdIsSet() const;
  /**
   * Unsets the user id property
   */
  void UserIdClear();

  /**
   * Gets the app id property
   */
  const std::string& AppId() const;
  /**
   * Sets the app id property
   */
  void AppId(const std::string& app_id);
  /**
   * Determines whether the app id property is set
   */
  bool AppIdIsSet() const;
  /**
   * Unsets the app id property
   */
  void AppIdClear();

  /**
   * Gets the cluster id property
   */
  const std::string& ClusterId() const;
  /**
   * Sets the custer id property
   */
  void ClusterId(const std::string& cluster_id);
  /**
   * Determines if the cluster id property is set
   */
  bool ClusterIdIsSet() const;
  /**
   * Unsets the cluster id property
   */
  void ClusterIdClear();

  /**
   * Gets the header table property
   */
  Table& HeaderTable();
  const Table& HeaderTable() const;
  /**
   * Sets the header table property
   */
  void HeaderTable(const Table& header_table);
  /**
   * Is there a header table associated with the message
   */
  bool HeaderTableIsSet() const;
  /**
   * Unsets the header table property
   */
  void HeaderTableClear();

 protected:
  struct Impl;
  /// PIMPL idiom
  std::unique_ptr<Impl> m_impl;
};

}  // namespace AmqpClient

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif  // SIMPLEAMQPCLIENT_BASICMESSAGE_H
