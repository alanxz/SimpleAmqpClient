#ifndef CONNECTION_H
#define CONNECTION_H

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

#include "Channel.h"

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <amqp.h>
#include <boost/utility.hpp>
#include <string>
#include <vector>

#define BROKER_HEARTBEAT 0

namespace AmqpClient {

/**
  * Connection to an AMQP Broker
  * The connection object represents a connection to an AMQP broker,
  * it holds, currently it connects to the broker on construction throwing
  * an exception if it fails. The only other operation is to create a channel
  * on which most operations can be performed.
  * Connection will disconnect when its destructed.
  */
class Connection : boost::noncopyable
{
public:
	typedef boost::shared_ptr<Connection> ptr_t;
	friend ptr_t boost::make_shared<Connection>(std::string const & a1, int const & a2,
			std::string const & a3, std::string const & a4,
			std::string const & a5, int const & a6, int const & a7);

	/**
	  * Creates a new connection object
	  * Creates a new connection to an AMQP broker using the supplied parameters
	  * @param host The hostname or IP address of the AMQP broker
	  * @param port The port to connect to the AMQP broker on
	  * @param username The username used to authenticate with the AMQP broker
	  * @param password The password corresponding to the username used to authenticate with the AMQP broker
	  * @param vhost The virtual host on the AMQP we should connect to
	  * @param channel_max Request that the server limit the number of channels for 
	  * this connection to the specified parameter, a value of zero will use the broker-supplied value
	  * @param frame_max Request that the server limit the maximum size of any frame to this value
	  * @return a new Connection object pointer
	  */
	static ptr_t Create(const std::string& host = "127.0.0.1",
						int port = 5672,
						const std::string& username = "guest",
						const std::string& password = "guest",
						const std::string& vhost = "/",
						int channel_max = 0,
						int frame_max = 131072)
	{
		return boost::make_shared<Connection>(host, port, username, password, vhost, channel_max, frame_max);
	}
private:
    explicit Connection(const std::string& host,
			   int port,
			   const std::string& username,
			   const std::string& password,
			   const std::string& vhost,
			   int channel_max,
			   int frame_max);
public:
	/**
	  * Destructor
	  * Connection automatically disconnects on destruction
	  */
    virtual ~Connection();

	/**
	  * Creates a new channel object
	  * Creates a new channel object pointer
	  */
    Channel::ptr_t CreateChannel();

protected:
    amqp_connection_state_t m_connection;
    amqp_channel_t m_nextChannel;
    std::vector<Channel::ptr_t> m_channels;
};

} // namespace AmqpClient

#endif // CONNECTION_H
