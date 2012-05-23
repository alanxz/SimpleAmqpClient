#ifndef SIMPLERPCCLIENT_H
#define SIMPLERPCCLIENT_H
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

#include "SimpleAmqpClient/Channel.h"
#include "SimpleAmqpClient/BasicMessage.h"

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <string>

#ifdef _MSC_VER
# pragma warning ( push )
# pragma warning ( disable: 4275 4251 )
#endif

namespace AmqpClient {


class SIMPLEAMQPCLIENT_EXPORT SimpleTwoWayChannel : boost::noncopyable
{
public:
	typedef boost::shared_ptr<SimpleTwoWayChannel> ptr_t;
	friend ptr_t boost::make_shared<SimpleTwoWayChannel>(AmqpClient::Channel::ptr_t const & a1, std::string const & a2, std::string const & a3);

	static ptr_t Create(Channel::ptr_t channel, const std::string& incoming_name, const std::string& outgoing_name)
	{ return boost::make_shared<SimpleTwoWayChannel>(channel, incoming_name, outgoing_name); }

private:
	static const std::string ROUTING_KEY;

	explicit SimpleTwoWayChannel(Channel::ptr_t channel, const std::string& incoming_name, 
			const std::string& outgoing_name);

public:
	virtual ~SimpleTwoWayChannel();

	std::string getIncomingName() const { return m_incoming_tag; }
	std::string getOutgoingName() const { return m_outgoing_tag; }

	void Send(const std::string& message);
	void Send(BasicMessage::ptr_t message);

	std::string Receive();
	BasicMessage::ptr_t ReceiveMessage();

	std::string SendAndReceive(const std::string& message);
	BasicMessage::ptr_t SendAndReceive(BasicMessage::ptr_t message);

private:
	Channel::ptr_t m_channel;
	const std::string m_incoming_tag;
	const std::string m_outgoing_tag;
};


} // namespace AmqpClient

#ifdef _MSC_VER
# pragma warning ( pop )
#endif 

#endif //SIMPLERPCCLIENT_H

