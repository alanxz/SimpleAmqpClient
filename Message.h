#ifndef MESSAGE_H
#define MESSAGE_H

#include <boost/cstdint.hpp>
#include <boost/utility.hpp>
#include <amqp.h>
#include <amqp_framing.h>
#include <string>


namespace AmqpClient {

class Message : boost::noncopyable
{
public:

    enum delivery_mode_t {
        dm_nonpersistent = 1,
        dm_persistent = 2
    };

    Message();
    virtual ~Message();

    const amqp_basic_properties_t* getAmqpProperties() const { return &m_properties; }
    amqp_bytes_t getAmqpBody() const { return m_body; }

    std::string Body() const;
    void Body(const std::string& body);

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
    amqp_pool_t m_pool;
    amqp_basic_properties_t m_properties;
    amqp_bytes_t m_body;

};

} // namespace AmqpClient

#endif // MESSAGE_H
