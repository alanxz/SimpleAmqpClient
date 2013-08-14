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

#include "SimpleAmqpClient/SimpleSubscriber.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace AmqpClient
{

SimpleSubscriber::SimpleSubscriber(Channel::ptr_t channel, const std::string& publisher_name) :
	m_channel(channel)
{
	m_consumerQueue = "SimpleSubscriber_";
	boost::uuids::random_generator uuid_gen;
	boost::uuids::uuid guid(uuid_gen());
	m_consumerQueue += boost::lexical_cast<std::string>(guid);

	m_channel->DeclareQueue(m_consumerQueue);
	m_channel->BindQueue(m_consumerQueue, publisher_name);

	m_channel->BasicConsume(m_consumerQueue, m_consumerQueue);
}

SimpleSubscriber::~SimpleSubscriber()
{
}

std::string SimpleSubscriber::WaitForMessageString()
{
	BasicMessage::ptr_t incoming = WaitForMessage();
	return incoming->Body();
}

BasicMessage::ptr_t SimpleSubscriber::WaitForMessage()
{
	return m_channel->BasicConsumeMessage();
}

} //namespace AmqpClient
