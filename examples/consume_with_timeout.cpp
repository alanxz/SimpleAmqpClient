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

#include <SimpleAmqpClient.h>
#include <SimpleAmqpClient/SimpleRpcClient.h>
#include <SimpleAmqpClient/SimpleRpcServer.h>

#define BOOST_ALL_NO_LIB
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/utility.hpp>

#include <iostream>
#include <stdlib.h>

using namespace AmqpClient;

class thread_body : boost::noncopyable
{
	public:
		thread_body()
		{
			char* szBroker = getenv("AMQP_BROKER");
			Channel::ptr_t channel;
			if (szBroker != NULL)
				channel = Channel::Create(szBroker);
			else
				channel = Channel::Create();


			server = SimpleRpcServer::Create(channel, "consume_timeout_test");

			m_thread =
				boost::make_shared<boost::thread>(boost::bind(&thread_body::run,
							this));
		}

		virtual ~thread_body()
		{
			std::cout << "Is joinable " << m_thread->joinable() << "inter reqd: " << m_thread->interruption_requested() << std::endl;
			m_thread->interrupt();
			std::cout << "Is joinable " << m_thread->joinable() << "inter reqd: " << m_thread->interruption_requested() << std::endl;
			m_thread->join();
		}

		void run()
		{
			while (!boost::this_thread::interruption_requested())
			{
				std::cout << "Waiting for message... " << std::flush;
				BasicMessage::ptr_t message;
				if (server->GetNextIncomingMessage(message, 1))
				{
					std::cout << "message received.\n";
          server->RespondToMessage(message, "this is a response");
				}
				else
				{
					std::cout << "message not received.\n";
				}
			}
		}

		SimpleRpcServer::ptr_t server;
		boost::shared_ptr<boost::thread> m_thread;

};
int main()
{
	boost::shared_ptr<thread_body> body = boost::make_shared<thread_body>();

	boost::this_thread::sleep(boost::posix_time::seconds(1));

  char* szBroker = getenv("AMQP_BROKER");
  Channel::ptr_t channel;
  if (szBroker != NULL)
    channel = Channel::Create(szBroker);
  else
    channel = Channel::Create();

  SimpleRpcClient::ptr_t client = SimpleRpcClient::Create(channel, "consume_timeout_test");
  std::string str = client->Call("Here is my message");
  std::cout << "Got response: " << str << std::endl;

	boost::this_thread::sleep(boost::posix_time::seconds(2));

	return 0;
}
