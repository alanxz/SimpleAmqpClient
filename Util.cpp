#include "Util.h"

#include "AmqpResponseLibraryException.h"
#include "AmqpResponseServerException.h"

#include <stdexcept>

namespace AmqpClient {

void Util::CheckRpcReply(amqp_rpc_reply_t& reply, const std::string& context = "")
{
    switch (reply.reply_type)
    {
        case AMQP_RESPONSE_NORMAL:
            return;
            break;

        case AMQP_RESPONSE_NONE:
            throw std::logic_error("Got a amqp_rpc_reply_t with no reply_type!");
            break;

        case AMQP_RESPONSE_LIBRARY_EXCEPTION:
            throw AmqpResponseLibraryException(reply.library_error, context);
            break;

        case AMQP_RESPONSE_SERVER_EXCEPTION:
            throw AmqpResponseServerException(reply, context);
            break;
        default:
            throw std::runtime_error("amqp_rpc_reply_t that didn't match!");
    }
}

void Util::CheckLastRpcReply(amqp_connection_state_t connection, const std::string& context = "")
{
    CheckRpcReply(amqp_get_rpc_reply(connection));
}

} // namespace AmqpClient
