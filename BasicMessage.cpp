
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

#include "BasicMessage.h"

#include <cstring>

namespace AmqpClient {

BasicMessage::BasicMessage() :
	m_delivery_tag(0)
{
	m_body.bytes = NULL;
	m_body.len = 0;
	m_properties._flags = 0;
}

BasicMessage::BasicMessage(amqp_bytes_t body, amqp_basic_properties_t* properties, uint64_t delivery_tag) :
	m_body(body), m_properties(*properties), m_delivery_tag(delivery_tag)
{
	if (ContentTypeIsSet()) m_properties.content_type = amqp_bytes_malloc_dup(m_properties.content_type);
	if (ContentEncodingIsSet()) m_properties.content_encoding = amqp_bytes_malloc_dup(m_properties.content_encoding);
	if (CorrelationIdIsSet()) m_properties.correlation_id = amqp_bytes_malloc_dup(m_properties.correlation_id);
	if (ReplyToIsSet()) m_properties.reply_to = amqp_bytes_malloc_dup(m_properties.reply_to);
	if (ExpirationIsSet()) m_properties.expiration = amqp_bytes_malloc_dup(m_properties.expiration);
	if (MessageIdIsSet()) m_properties.message_id = amqp_bytes_malloc_dup(m_properties.message_id);
	if (TypeIsSet()) m_properties.type = amqp_bytes_malloc_dup(m_properties.type);
	if (UserIdIsSet()) m_properties.user_id = amqp_bytes_malloc_dup(m_properties.user_id);
	if (AppIdIsSet()) m_properties.app_id = amqp_bytes_malloc_dup(m_properties.app_id);
	if (ClusterIdIsSet()) m_properties.cluster_id = amqp_bytes_malloc_dup(m_properties.cluster_id);
}

BasicMessage::~BasicMessage()
{
	amqp_bytes_free(m_body);
	if (ContentTypeIsSet()) amqp_bytes_free(m_properties.content_type);
	if (ContentEncodingIsSet()) amqp_bytes_free(m_properties.content_encoding);
	if (CorrelationIdIsSet()) amqp_bytes_free(m_properties.correlation_id);
	if (ReplyToIsSet()) amqp_bytes_free(m_properties.reply_to);
	if (ExpirationIsSet()) amqp_bytes_free(m_properties.expiration);
	if (MessageIdIsSet()) amqp_bytes_free(m_properties.message_id);
	if (TypeIsSet()) amqp_bytes_free(m_properties.type);
	if (UserIdIsSet()) amqp_bytes_free(m_properties.user_id);
	if (AppIdIsSet()) amqp_bytes_free(m_properties.app_id);
	if (ClusterIdIsSet()) amqp_bytes_free(m_properties.cluster_id);
}

std::string BasicMessage::Body() const
{
    if (m_body.bytes == NULL)
        return std::string();
    else
        return std::string((char*)m_body.bytes, m_body.len);
}
void BasicMessage::Body(const std::string& body)
{
	m_body = amqp_bytes_malloc_dup(amqp_cstring_bytes(body.c_str()));
}

std::string BasicMessage::ContentType() const
{
    if (ContentTypeIsSet())
        return std::string((char*)m_properties.content_type.bytes, m_properties.content_type.len);
    else
        return std::string();
}

void BasicMessage::ContentType(const std::string& content_type)
{
	if (ContentTypeIsSet()) amqp_bytes_free(m_properties.content_type);
	m_properties.content_type = amqp_bytes_malloc_dup(amqp_cstring_bytes(content_type.c_str()));
    m_properties._flags |= AMQP_BASIC_CONTENT_TYPE_FLAG;
}


std::string BasicMessage::ContentEncoding() const
{
    if (ContentEncodingIsSet())
        return std::string((char*)m_properties.content_encoding.bytes, m_properties.content_encoding.len);
    else
        return std::string();
}

void BasicMessage::ContentEncoding(const std::string& content_encoding)
{
	if (ContentEncodingIsSet()) amqp_bytes_free(m_properties.content_encoding);
	m_properties.content_encoding = amqp_bytes_malloc_dup(amqp_cstring_bytes(content_encoding.c_str()));
    m_properties._flags |= AMQP_BASIC_CONTENT_ENCODING_FLAG;
}

BasicMessage::delivery_mode_t BasicMessage::DeliveryMode() const
{
    if (DeliveryModeIsSet())
        return (delivery_mode_t)m_properties.delivery_mode;
    else
        return (delivery_mode_t)0;
}
void BasicMessage::DeliveryMode(delivery_mode_t delivery_mode)
{
    m_properties.delivery_mode = delivery_mode;
    m_properties._flags |= AMQP_BASIC_DELIVERY_MODE_FLAG;
}

uint8_t BasicMessage::Priority() const
{
    if (PriorityIsSet())
        return m_properties.priority;
    else
        return 0;
}
void BasicMessage::Priority(uint8_t priority)
{
    m_properties.priority = priority;
    m_properties._flags |= AMQP_BASIC_PRIORITY_FLAG;
}

std::string BasicMessage::CorrelationId() const
{
    if (CorrelationIdIsSet())
        return std::string((char*)m_properties.correlation_id.bytes, m_properties.correlation_id.len);
    else
        return std::string();
}
void BasicMessage::CorrelationId(const std::string& correlation_id)
{
	if (CorrelationIdIsSet()) amqp_bytes_free(m_properties.correlation_id);
	m_properties.correlation_id = amqp_bytes_malloc_dup(amqp_cstring_bytes(correlation_id.c_str()));
    m_properties._flags |= AMQP_BASIC_CORRELATION_ID_FLAG;
}

std::string BasicMessage::ReplyTo() const
{
    if (ReplyToIsSet())
        return std::string((char*)m_properties.reply_to.bytes, m_properties.reply_to.len);
    else
        return std::string();
}
void BasicMessage::ReplyTo(const std::string& reply_to)
{
	if (ReplyToIsSet()) amqp_bytes_free(m_properties.reply_to);
	m_properties.reply_to = amqp_bytes_malloc_dup(amqp_cstring_bytes(reply_to.c_str()));
    m_properties._flags |= AMQP_BASIC_REPLY_TO_FLAG;
}

std::string BasicMessage::Expiration() const
{
    if (ExpirationIsSet())
        return std::string((char*)m_properties.expiration.bytes, m_properties.expiration.len);
    else
        return std::string();
}
void BasicMessage::Expiration(const std::string& expiration)
{
	if (ExpirationIsSet()) amqp_bytes_free(m_properties.expiration);
	m_properties.expiration = amqp_bytes_malloc_dup(amqp_cstring_bytes(expiration.c_str()));
    m_properties._flags |= AMQP_BASIC_EXPIRATION_FLAG;
}

std::string BasicMessage::MessageId() const
{
    if (MessageIdIsSet())
        return std::string((char*)m_properties.message_id.bytes, m_properties.message_id.len);
    else
        return std::string();
}
void BasicMessage::MessageId(const std::string& message_id)
{
	if (MessageIdIsSet()) amqp_bytes_free(m_properties.message_id);
	m_properties.message_id = amqp_bytes_malloc_dup(amqp_cstring_bytes(message_id.c_str()));
    m_properties._flags |= AMQP_BASIC_MESSAGE_ID_FLAG;
}

uint64_t BasicMessage::Timestamp() const
{
    if (ReplyToIsSet())
        return m_properties.timestamp;
    else
        return 0;
}
void BasicMessage::Timestamp(uint64_t timestamp)
{
    m_properties.timestamp = timestamp;
    m_properties._flags |= AMQP_BASIC_TIMESTAMP_FLAG;
}

std::string BasicMessage::Type() const
{
    if (TypeIsSet())
        return std::string((char*)m_properties.type.bytes, m_properties.type.len);
    else
        return std::string();
}
void BasicMessage::Type(const std::string& type)
{
	if (TypeIsSet()) amqp_bytes_free(m_properties.type);
	m_properties.type = amqp_bytes_malloc_dup(amqp_cstring_bytes(type.c_str()));
    m_properties._flags |= AMQP_BASIC_TYPE_FLAG;
}

std::string BasicMessage::UserId() const
{
    if (UserIdIsSet())
        return std::string((char*)m_properties.user_id.bytes, m_properties.user_id.len);
    else
        return std::string();
}
void BasicMessage::UserId(const std::string& user_id)
{
	if (UserIdIsSet()) amqp_bytes_free(m_properties.user_id);
	m_properties.user_id = amqp_bytes_malloc_dup(amqp_cstring_bytes(user_id.c_str()));
    m_properties._flags |= AMQP_BASIC_USER_ID_FLAG;
}

std::string BasicMessage::AppId() const
{
    if (AppIdIsSet())
        return std::string((char*)m_properties.app_id.bytes, m_properties.app_id.len);
    else
        return std::string();
}
void BasicMessage::AppId(const std::string& app_id)
{
	if (AppIdIsSet()) amqp_bytes_free(m_properties.app_id);
	m_properties.app_id = amqp_bytes_malloc_dup(amqp_cstring_bytes(app_id.c_str()));
    m_properties._flags |= AMQP_BASIC_APP_ID_FLAG;
}

std::string BasicMessage::ClusterId() const
{
    if (ClusterIdIsSet())
        return std::string((char*)m_properties.cluster_id.bytes, m_properties.cluster_id.len);
    else
        return std::string();
}
void BasicMessage::ClusterId(const std::string& cluster_id)
{
	if (AppIdIsSet()) amqp_bytes_free(m_properties.cluster_id);
	m_properties.cluster_id = amqp_bytes_malloc_dup(amqp_cstring_bytes(cluster_id.c_str()));
    m_properties._flags |= AMQP_BASIC_CLUSTER_ID_FLAG;
}

} // namespace AmqpClient
