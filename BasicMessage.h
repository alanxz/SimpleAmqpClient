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
	friend ptr_t boost::make_shared<BasicMessage>(amqp_bytes_t const & a1, amqp_basic_properties_t* const & a2, uint64_t const& a3 );

	static ptr_t Create() { return boost::make_shared<BasicMessage>(); }
	static ptr_t Create(amqp_bytes_t body, amqp_basic_properties_t* properties, uint64_t delivery_tag = 0) 
		{ return boost::make_shared<BasicMessage>(body, properties, delivery_tag); }

private:
    BasicMessage();
	BasicMessage(amqp_bytes_t body, amqp_basic_properties_t* properties, uint64_t delivery_tag);

public:
    virtual ~BasicMessage();

    const amqp_basic_properties_t* getAmqpProperties() const { return &m_properties; }
    amqp_bytes_t getAmqpBody() const { return m_body; }

    std::string Body() const;
    void Body(const std::string& body);

	uint64_t DeliveryTag() const { return m_delivery_tag; }
	void DeliveryTag(uint64_t delivery_tag) { m_delivery_tag = delivery_tag; }

    std::string ContentType() const;
    void ContentType(const std::string& content_type);
    bool ContentTypeIsSet() const { return m_properties._flags & AMQP_BASIC_CONTENT_TYPE_FLAG; }
    void ContentTypeClear() { m_properties._flags &= ~AMQP_BASIC_CONTENT_TYPE_FLAG; }

    std::string ContentEncoding() const;
    void ContentEncoding(const std::string& content_encoding);
    bool ContentEncodingIsSet() const { return m_properties._flags & AMQP_BASIC_CONTENT_ENCODING_FLAG; }
    void ContentEncodingClear() { m_properties._flags &= ~AMQP_BASIC_CONTENT_ENCODING_FLAG; }

    delivery_mode_t DeliveryMode() const;
    void DeliveryMode(delivery_mode_t delivery_mode);
    bool DeliveryModeIsSet() const { return m_properties._flags & AMQP_BASIC_DELIVERY_MODE_FLAG; }
    void DeliveryModeClear() { m_properties._flags &= ~AMQP_BASIC_DELIVERY_MODE_FLAG; }

    uint8_t Priority() const;
    void Priority(uint8_t priority);
    bool PriorityIsSet() const { return m_properties._flags & AMQP_BASIC_PRIORITY_FLAG; }
    void PriorityClear() { m_properties._flags &= ~AMQP_BASIC_PRIORITY_FLAG; }

    std::string CorrelationId() const;
    void CorrelationId(const std::string& correlation_id);
    bool CorrelationIdIsSet() const { return m_properties._flags & AMQP_BASIC_CORRELATION_ID_FLAG; }
    void CorrelationIdClear() { m_properties._flags &= ~AMQP_BASIC_CORRELATION_ID_FLAG; }

    std::string ReplyTo() const;
    void ReplyTo(const std::string& reply_to);
    bool ReplyToIsSet() const { return m_properties._flags & AMQP_BASIC_REPLY_TO_FLAG; }
    void ReplyToClear() { m_properties._flags &= ~AMQP_BASIC_REPLY_TO_FLAG; }

    std::string Expiration() const;
    void Expiration(const std::string& expiration);
    bool ExpirationIsSet() const { return m_properties._flags & AMQP_BASIC_EXPIRATION_FLAG; }
    void ExpirationClear() { m_properties._flags &= ~AMQP_BASIC_EXPIRATION_FLAG; }

    std::string MessageId() const;
    void MessageId(const std::string& message_id);
    bool MessageIdIsSet() const { return m_properties._flags & AMQP_BASIC_MESSAGE_ID_FLAG; }
    void MessageIdClear() { m_properties._flags &= ~AMQP_BASIC_MESSAGE_ID_FLAG; }

    uint64_t Timestamp() const;
    void Timestamp(uint64_t timestamp);
    bool TimestampIsSet() const { return m_properties._flags & AMQP_BASIC_TIMESTAMP_FLAG; }
    void TimestampClear() { m_properties._flags &= ~AMQP_BASIC_TIMESTAMP_FLAG; }

    std::string Type() const;
    void Type(const std::string& type);
    bool TypeIsSet() const { return m_properties._flags & AMQP_BASIC_TYPE_FLAG; }
    void TypeClear() { m_properties._flags &= ~AMQP_BASIC_TYPE_FLAG; }

    std::string UserId() const;
    void UserId(const std::string& user_id);
    bool UserIdIsSet() const { return m_properties._flags & AMQP_BASIC_USER_ID_FLAG; }
    void UserIdClear() { m_properties._flags &= ~AMQP_BASIC_USER_ID_FLAG; }

    std::string AppId() const;
    void AppId(const std::string& app_id);
    bool AppIdIsSet() const { return m_properties._flags & AMQP_BASIC_APP_ID_FLAG; }
    void AppIdClear() { m_properties._flags &= ~AMQP_BASIC_APP_ID_FLAG; }

    std::string ClusterId() const;
    void ClusterId(const std::string& cluster_id);
    bool ClusterIdIsSet() const { return m_properties._flags & AMQP_BASIC_CLUSTER_ID_FLAG; }
    void ClusterIdClear() { m_properties._flags &= ~AMQP_BASIC_CLUSTER_ID_FLAG; }



protected:
    amqp_basic_properties_t m_properties;
    amqp_bytes_t m_body;
	uint64_t m_delivery_tag;

};

} // namespace AmqpClient

#endif // MESSAGE_H
