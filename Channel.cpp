#include "Channel.h"

#include <amqp_framing.h>

namespace AmqpClient {

Channel::Channel(amqp_connection_state_t connection, amqp_channel_t channel_num) :
    m_connection(connection), m_channel(channel_num)
{
    m_empty_table.num_entries = 0;
    m_empty_table.entries = NULL;

    amqp_channel_open(m_connection, m_channel);
}

Channel::~Channel()
{
    amqp_channel_close(m_connection, m_channel, AMQP_REPLY_SUCCESS);
}

void Channel::DeclareExchange(const std::string& exchange_name,
                              const std::string& exchange_type,
                              bool passive,
                              bool durable,
                              bool auto_delete)
{
    amqp_exchange_declare(m_connection, m_channel,
                          amqp_cstring_bytes(exchange_name.c_str()),
                          amqp_cstring_bytes(exchange_type.c_str()),
                          passive,
                          durable,
                          auto_delete,
                          m_empty_table);
}


void Channel::DeclareQueue(const std::string& queue_name,
                           bool passive,
                           bool durable,
                           bool exclusive,
                           bool auto_delete)
{
    amqp_queue_declare(m_connection, m_channel,
                       amqp_cstring_bytes(queue_name.c_str()),
                       passive,
                       durable,
                       exclusive,
                       auto_delete,
                       m_empty_table);
}

void Channel::BindQueue(const std::string& queue_name,
                        const std::string& exchange_name,
                        const std::string& routing_key)
{
    amqp_queue_bind(m_connection, m_channel,
                    amqp_cstring_bytes(queue_name.c_str()),
                    amqp_cstring_bytes(exchange_name.c_str()),
                    amqp_cstring_bytes(routing_key.c_str()),
                    m_empty_table);
}

} // namespace AmqpClient
