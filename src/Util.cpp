
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

#include "SimpleAmqpClient/Util.h"

#include "SimpleAmqpClient/AmqpResponseLibraryException.h"
#include "SimpleAmqpClient/AmqpResponseServerException.h"

#include <stdexcept>
#include <sstream>

namespace AmqpClient {

void Util::CheckRpcReply(amqp_rpc_reply_t reply, const std::string& context)
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
            throw AmqpResponseLibraryException(reply, context);
            break;

        case AMQP_RESPONSE_SERVER_EXCEPTION:
            throw AmqpResponseServerException(reply, context);
            break;
        default:
            throw std::runtime_error("amqp_rpc_reply_t that didn't match!");
    }
}

void Util::CheckLastRpcReply(amqp_connection_state_t connection, const std::string& context)
{
    CheckRpcReply(amqp_get_rpc_reply(connection));
}

void Util::CheckForError(int ret, const std::string& context)
{
    if (ret < 0)
    {
        char* errstr = amqp_error_string(-ret);
        std::ostringstream oss;
        oss << context << ": " << errstr;
        free(errstr);
        throw std::runtime_error(oss.str().c_str());
    }

}
} // namespace AmqpClient
