#ifndef UTIL_H
#define UTIL_H

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
#include <amqp.h>
#include <string>

#ifdef WIN32
# ifdef SimpleAmqpClient_EXPORTS
#  define SIMPLEAMQPCLIENT_EXPORT __declspec(dllexport)
# else
#  define SIMPLEAMQPCLIENT_EXPORT __declspec(dllimport)
# endif
#else
#define SIMPLEAMQPCLIENT_EXPORT
#endif

namespace AmqpClient {

class Util
{
public:
	/**
	  * Checks the result of a librabbitmq-c call that returns a amqp_rpc_reply_t
	  *
	  * Checks the result of a librabbitmq-c call that returns an
	  * amqp_rpc_reply_t struct and throws an exception if it fails.
	  */
    static void CheckRpcReply(amqp_rpc_reply_t reply, const std::string& context = "");
	
	/**
	  * Checks the result of a librabbitmq-c that doesn't return a amqp_rpc_reply_t
	  *
	  * Checks the result of a librabbitmq-c call that doesn't return an
	  * amqp_rpc_reply_t struct, but the result is stored as a part of the
	  * amqp_connection_state_t, and is checked with amqp_check_last_rpc()
	  * Throws an exception if an error condition is detected
	  */
	static void CheckLastRpcReply(amqp_connection_state_t connection, const std::string& context);
	/**
	  * Checks the result of a librabbitmq-c call that returns an int
	  *
	  * Checks the result of a librabbitmq-c call that returns an int, 
	  * throws an exception if an error condition is detected.
	  */
    static void CheckForError(int ret, const std::string& context = "");
private:
    Util();
};

} // namespace AmqpClient

#endif // UTIL_H
