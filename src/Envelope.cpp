#include "SimpleAmqpClient/Envelope.h"

namespace AmqpClient
{

Envelope::Envelope(const BasicMessage::ptr_t message, const std::string& consumer_tag,
    const uint64_t delivery_tag, const std::string& exchange, bool redelivered, const std::string& routing_key) 
  : m_message(message)
  , m_consumerTag(consumer_tag)
  , m_deliveryTag(delivery_tag)
  , m_exchange(exchange)
  , m_redelivered(redelivered)
  , m_routingKey(routing_key)
{
}

Envelope::~Envelope()
{
}

}