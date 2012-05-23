/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MIT
 *
 * Portions created by VMware are Copyright (c) 2007-2012 VMware, Inc.
 * All Rights Reserved.
 *
 * Portions created by Tony Garnock-Jones are Copyright (c) 2009-2010
 * VMware, Inc. and Tony Garnock-Jones. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * ***** END LICENSE BLOCK *****
 */

#include "SimpleAmqpClient/SimplePublisher.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <string>
#include <sstream>

namespace AmqpClient
{

SimplePublisher::SimplePublisher(Channel::ptr_t channel, const std::string& publisher_name) :
	m_channel(channel), m_publisherExchange(publisher_name)
{
	if (m_publisherExchange == "")
	{
		m_publisherExchange = "SimplePublisher_";
		boost::uuids::random_generator uuid_gen;
		boost::uuids::uuid guid(uuid_gen());
		m_publisherExchange += boost::lexical_cast<std::string>(guid);
	}


	m_channel->DeclareExchange(m_publisherExchange, "fanout", false, false, false);
}

SimplePublisher::~SimplePublisher()
{
	m_channel->DeleteExchange(m_publisherExchange, false, false);
}

void SimplePublisher::Publish(const std::string& message)
{
	BasicMessage::ptr_t outgoing_message = BasicMessage::Create();
	outgoing_message->Body(message);

	Publish(outgoing_message);
}

void SimplePublisher::Publish(const BasicMessage::ptr_t message)
{
	m_channel->BasicPublish(m_publisherExchange, "", message);
}

} // namespace AmqpClient
