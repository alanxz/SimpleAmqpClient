#ifndef UTIL_H
#define UTIL_H

#include <boost/cstdint.hpp>
#include <amqp.h>
#include <string>

namespace AmqpClient {

class Util
{
public:
    static void CheckRpcReply(amqp_rpc_reply_t reply, const std::string& context = "");
	static void CheckLastRpcReply(amqp_connection_state_t connection, const std::string& context);
    static void CheckForError(int ret, const std::string& context = "");
private:
    Util();
};

} // namespace AmqpClient

#endif // UTIL_H
