#ifndef CHANNEL_H
#define CHANNEL_H

#include "Message.h"

#include <boost/cstdint.hpp>
#include <boost/utility.hpp>
#include <amqp.h>
#include <string>

namespace AmqpClient {

class Channel : boost::noncopyable
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

    void DeleteQueue(const std::string& queue_name,
                     bool if_unused = false,
                     bool if_empty = false);

    void BindQueue(const std::string& queue_name,
                   const std::string& exchange_name,
                   const std::string& routing_key = "");

    void UnbindQueue(const std::string& queue_name,
                     const std::string& exchange_name,
                     const std::string& binding_key = "");

    void PurgeQueue(const std::string& queue_name,
                    bool no_wait = false);

    void BasicPublish(const std::string& exchange_name,
                      const std::string& routing_key,
                      const Message& message,
                      bool mandatory = false,
                      bool immediate = false);
protected:
    amqp_connection_state_t m_connection;
    amqp_channel_t m_channel;
    amqp_table_t m_empty_table;

};

} // namespace AmqpClient

#endif // CHANNEL_H
