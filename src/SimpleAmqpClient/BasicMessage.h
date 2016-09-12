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

#include "SimpleAmqpClient/Table.h"
#include "SimpleAmqpClient/Util.h"

#include <boost/cstdint.hpp>
#include <boost/make_shared.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <string>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4275 4251)
#endif

struct amqp_bytes_t_;
struct amqp_basic_properties_t_;

namespace AmqpClient {

namespace Detail {
class BasicMessageImpl;
}

class SIMPLEAMQPCLIENT_EXPORT BasicMessage : boost::noncopyable {
 public:
  typedef boost::shared_ptr<BasicMessage> ptr_t;

  enum delivery_mode_t { dm_nonpersistent = 1, dm_persistent = 2 };

  /**
    * Create a new empty BasicMessage object
    */
  static ptr_t Create() { return boost::make_shared<BasicMessage>(); }

  /**
    * Create a new BasicMessage object
    * Creates a new BasicMessage object with a given body
    * @param body the message body.
    * @returns a new BasicMessage object
    */
  static ptr_t Create(const std::string &body) {
    return boost::make_shared<BasicMessage>(body);
  }

  /**
    * Create a new BasicMessage object
    * Creates a new BasicMessage object with a given body, properties
    * @param body the message body. The message body is NOT duplicated.
    * Passed in message body is deallocated when: body is set or message is
    * destructed.
    * @properties the amqp_basic_properties_t struct. Note this makes a deep
    * copy of the properties struct
    * @returns a new BasicMessage object
    */
  static ptr_t Create(amqp_bytes_t_ &body,
                      amqp_basic_properties_t_ *properties) {
    return boost::make_shared<BasicMessage>(body, properties);
  }

  BasicMessage();
  BasicMessage(const std::string &body);
  BasicMessage(const amqp_bytes_t_ &body,
               const amqp_basic_properties_t_ *properties);

 public:
  /**
    * Destructor
    */
  virtual ~BasicMessage();

  /**
    * INTERNAL INTERFACE: Gets the internal amqp_basic_properties_t struct
    *
    * Retrieves a reference to the internal structure used to keep track of
    * the properties of the message. Changing members of this structure will
    * modify the properties on the message.
    * @returns the amqp_basic_properties_t* struct
    */
  const amqp_basic_properties_t_ *getAmqpProperties() const;
  /**
    * INTERNAL INTERFACE: Gets the amqp_bytes_t representation of the message
   * body
    *
    * @returns the message body. Note this is owned by the message and will
    * be freed when the BasicMessage is destructed
    */
  const amqp_bytes_t_ &getAmqpBody() const;

  /**
    * Gets the message body as a std::string
    */
  std::string Body() const;
  /**
    * Sets the message body as a std::string
    */
  void Body(const std::string &body);

  /**
    * Gets the content type property
    */
  std::string ContentType() const;
  /**
    * Sets the content type property
    */
  void ContentType(const std::string &content_type);
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
  std::string ContentEncoding() const;
  /**
    * Sets the content encoding property
    */
  void ContentEncoding(const std::string &content_encoding);
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
  std::string CorrelationId() const;
  /**
    * Sets the correlation id property
    */
  void CorrelationId(const std::string &correlation_id);
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
  std::string ReplyTo() const;
  /**
    * Sets the reply to property
    */
  void ReplyTo(const std::string &reply_to);
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
  std::string Expiration() const;
  /**
    * Sets the expiration property
    */
  void Expiration(const std::string &expiration);
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
  std::string MessageId() const;
  /**
    * Sets the message id property
    */
  void MessageId(const std::string &message_id);
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
  std::string Type() const;
  /**
    * Sets the type property
    */
  void Type(const std::string &type);
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
  std::string UserId() const;
  /**
    * Sets the user id property
    */
  void UserId(const std::string &user_id);
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
  std::string AppId() const;
  /**
    * Sets the app id property
    */
  void AppId(const std::string &app_id);
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
  std::string ClusterId() const;
  /**
    * Sets the custer id property
    */
  void ClusterId(const std::string &cluster_id);
  /**
    * Determines if the cluster id property is set
    */
  bool ClusterIdIsSet() const;
  /**
    * Unsets the cluster id property
    */
  void ClusterIdClear();

  /**
    * Gets the cluster id property
    */
  Table HeaderTable() const;
  /**
    * Sets the custer id property
    */
  void HeaderTable(const Table &header_table);
  /**
    * Is there a header table associated with the message
    */
  bool HeaderTableIsSet() const;
  /**
    * Unsets the cluster id property
    */
  void HeaderTableClear();

 protected:
  boost::scoped_ptr<Detail::BasicMessageImpl> m_impl;
};

}  // namespace AmqpClient

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif  // SIMPLEAMQPCLIENT_BASICMESSAGE_H
