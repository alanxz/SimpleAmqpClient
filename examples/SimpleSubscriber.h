#ifndef SIMPLESUBSCRIBER_H
#define SIMPLESUBSCRIBER_H
/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MIT
 *
 * Copyright (c) 2010-2013 Alan Antonuk
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

#include "SimpleAmqpClient/BasicMessage.h"
#include "SimpleAmqpClient/Channel.h"
#include "SimpleAmqpClient/Util.h"

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <string>

#ifdef _MSC_VER
# pragma warning ( push )
# pragma warning ( disable: 4275 4251 )
#endif

namespace AmqpClient
{

class SIMPLEAMQPCLIENT_EXPORT SimpleSubscriber
{
public:
	typedef boost::shared_ptr<SimpleSubscriber> ptr_t;

	friend ptr_t
		boost::make_shared<SimpleSubscriber>(AmqpClient::Channel::ptr_t const & a1, std::string const & a2);

	static ptr_t Create(AmqpClient::Channel::ptr_t channel, const std::string& publisher_channel)
	{ return boost::make_shared<SimpleSubscriber>(channel, publisher_channel); }


private:
	SimpleSubscriber(Channel::ptr_t channel, const std::string &publisher_channel);

public:
	virtual ~SimpleSubscriber();

	std::string WaitForMessageString();
	BasicMessage::ptr_t WaitForMessage();

private:
	Channel::ptr_t m_channel;
	std::string m_consumerQueue;

};

}

#ifdef _MSC_VER
# pragma warning ( pop )
#endif

#endif // SIMPLESUBSCRIBER_H

