#ifndef CHANNEL_H
#define CHANNEL_H

#include <boost/cstdint.hpp>
#include <amqp.h>
#include <string>

namespace AmqpClient {

class Channel
{
public:
    Channel(amqp_connection_state_t connection, amqp_channel_t channel_num);
    virtual ~Channel();

    void DeclareExchange(const std::string& exchange_name,
                         const std::string& exchange_type = "amq.direct",
                         bool passive = false,
                         bool durable = false,
                         bool auto_delete = true);

    void DeclareQueue(const std::string& queue_name,
                      bool passive = false,
                      bool durable = false,
                      bool exclusive = true,
                      bool auto_delete = true);

    void BindQueue(const std::string& queue_name,
                   const std::string& exchange_name,
                   const std::string& routing_key = "");
protected:
    amqp_connection_state_t m_connection;
    amqp_channel_t m_channel;
    amqp_table_t m_empty_table;

};

} // namespace AmqpClient

#endif // CHANNEL_H
