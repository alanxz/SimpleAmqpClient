#ifndef MESSAGE_H
#define MESSAGE_H

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

#include <boost/cstdint.hpp>
#include <boost/utility.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <amqp.h>
#include <amqp_framing.h>
#include <string>


namespace AmqpClient {

class BasicMessage : boost::noncopyable
{
public:
	typedef boost::shared_ptr<BasicMessage> ptr_t;

    enum delivery_mode_t {
        dm_nonpersistent = 1,
        dm_persistent = 2
    };

	friend ptr_t boost::make_shared<BasicMessage>();
	friend ptr_t boost::make_shared<BasicMessage>(std::string const & a1);
	friend ptr_t boost::make_shared<BasicMessage>(amqp_bytes_t const & a1, amqp_basic_properties_t* const & a2, uint64_t const& a3 );

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
	static ptr_t Create(const std::string& body) { return boost::make_shared<BasicMessage>(body); }

	/**
	  * Create a new BasicMessage object
	  * Creates a new BasicMessage object with a given body, properties and
	  * optionally a delivery tag
	  * @param body the message body. The message body is NOT duplicated.
	  * Passed in message body is deallocated when: body is set or message is
	  * destructed.
	  * @properties the amqp_basic_properties_t struct. Note this makes a deep
	  * copy of the properties struct
	  * @param delivery_tag the delivery tag of the message
	  * @returns a new BasicMessage object
	  */
	static ptr_t Create(amqp_bytes_t body, amqp_basic_properties_t* properties, uint64_t delivery_tag = 0) 
		{ return boost::make_shared<BasicMessage>(body, properties, delivery_tag); }

private:
    BasicMessage();
	BasicMessage(const std::string& body);
	BasicMessage(amqp_bytes_t body, amqp_basic_properties_t* properties, uint64_t delivery_tag);

public:
	/**
	  * Destructor
	  */
    virtual ~BasicMessage();

	/**
	  * Gets the internal amqp_basic_properties_t struct
	  *
	  * Retrieves a reference to the internal structure used to keep track of
	  * the properties of the message. Changing members of this structure will
	  * modify the properties on the message.
	  * @returns the amqp_basic_properties_t* struct 
	  */
    const amqp_basic_properties_t* getAmqpProperties() const { return &m_properties; }
	/**
	  * Gets the amqp_bytes_t representation of the message body
	  *
	  * @returns the message body. Note this is owned by the message and will
	  * be freed when the BasicMessage is destructed
	  */
    amqp_bytes_t getAmqpBody() const { return m_body; }

	/**
	  * Gets the message body as a std::string
	  */
    std::string Body() const;
	/**
	  * Sets the message body as a std::string
	  */
    void Body(const std::string& body);

	/**
	  * Gets the message delivery tag
	  */

	uint64_t DeliveryTag() const { return m_delivery_tag; }
	/**
	  * Sets the delivery tag
	  *
	  * This should not be used by client programs
	  */
	void DeliveryTag(uint64_t delivery_tag) { m_delivery_tag = delivery_tag; }

	/**
	  * Gets the content type property
	  */
    std::string ContentType() const;
	/**
	  * Sets the content type property
	  */
    void ContentType(const std::string& content_type);
	/**
	  * Determines whether the content type property is set
	  */
    bool ContentTypeIsSet() const { return m_properties._flags & AMQP_BASIC_CONTENT_TYPE_FLAG; }
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
    void ContentEncoding(const std::string& content_encoding);
	/**
	  * Determines whether the content encoding property is set
	  */
    bool ContentEncodingIsSet() const { return m_properties._flags & AMQP_BASIC_CONTENT_ENCODING_FLAG; }
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
    bool DeliveryModeIsSet() const { return m_properties._flags & AMQP_BASIC_DELIVERY_MODE_FLAG; }
	/**
	  * Unsets the delivery mode property if it is set
	  */
    void DeliveryModeClear() { m_properties._flags &= ~AMQP_BASIC_DELIVERY_MODE_FLAG; }

	/**
	  * Gets the priority property
	  */
    uint8_t Priority() const;
	/**
	  * Sets the priority property
	  */
    void Priority(uint8_t priority);
	/**
	  * Determines whether the priority property is set
	  */
    bool PriorityIsSet() const { return m_properties._flags & AMQP_BASIC_PRIORITY_FLAG; }
	/**
	  * Unsets the priority property if it is set
	  */
    void PriorityClear() { m_properties._flags &= ~AMQP_BASIC_PRIORITY_FLAG; }

	/**
	  * Gets the correlation id property
	  */
    std::string CorrelationId() const;
	/**
	  * Sets the correlation id property
	  */
    void CorrelationId(const std::string& correlation_id);
	/**
	  * Determines whether the correlation id property is set
	  */
    bool CorrelationIdIsSet() const { return m_properties._flags & AMQP_BASIC_CORRELATION_ID_FLAG; }
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
    void ReplyTo(const std::string& reply_to);
	/**
	  * Determines whether the reply to property is set
	  */
    bool ReplyToIsSet() const { return m_properties._flags & AMQP_BASIC_REPLY_TO_FLAG; }
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
    void Expiration(const std::string& expiration);
	/**
	  * Determines whether the expiration property is set
	  */
    bool ExpirationIsSet() const { return m_properties._flags & AMQP_BASIC_EXPIRATION_FLAG; }
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
    void MessageId(const std::string& message_id);
	/**
	  * Determines if the message id property is set
	  */
    bool MessageIdIsSet() const { return m_properties._flags & AMQP_BASIC_MESSAGE_ID_FLAG; }
	/**
	  * Unsets the message id property
	  */
    void MessageIdClear();

	/**
	  * Gets the timestamp property 
	  */
    uint64_t Timestamp() const;
	/**
	  * Sets the timestamp property
	  */
    void Timestamp(uint64_t timestamp);
	/**
	  * Determines whether the timestamp property is set
	  */
    bool TimestampIsSet() const { return m_properties._flags & AMQP_BASIC_TIMESTAMP_FLAG; }
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
    void Type(const std::string& type);
	/**
	  * Determines whether the type property is set
	  */
    bool TypeIsSet() const { return m_properties._flags & AMQP_BASIC_TYPE_FLAG; }
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
    void UserId(const std::string& user_id);
	/**
	  * Determines whether the user id property is set
	  */
    bool UserIdIsSet() const { return m_properties._flags & AMQP_BASIC_USER_ID_FLAG; }
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
    void AppId(const std::string& app_id);
	/**
	  * Determines whether the app id property is set
	  */
    bool AppIdIsSet() const { return m_properties._flags & AMQP_BASIC_APP_ID_FLAG; }
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
    void ClusterId(const std::string& cluster_id);
	/**
	  * Determines if the cluster id property is set
	  */
    bool ClusterIdIsSet() const { return m_properties._flags & AMQP_BASIC_CLUSTER_ID_FLAG; }
	/**
	  * Unsets the cluster id property
	  */
    void ClusterIdClear();



protected:
    amqp_basic_properties_t m_properties;
    amqp_bytes_t m_body;
	uint64_t m_delivery_tag;

};

} // namespace AmqpClient

#endif // MESSAGE_H
