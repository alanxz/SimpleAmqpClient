#ifndef SIMPLEAMQPCLIENT_MESSAGERETURNEDEXCEPTION_H
#define SIMPLEAMQPCLIENT_MESSAGERETURNEDEXCEPTION_H
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

#include <boost/cstdint.hpp>

#include <stdexcept>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251 4275)
#endif

namespace AmqpClient {

class SIMPLEAMQPCLIENT_EXPORT MessageReturnedException
    : public std::runtime_error {
 public:
  explicit MessageReturnedException(BasicMessage::ptr_t message,
                                    boost::uint32_t reply_code,
                                    const std::string &reply_text,
                                    const std::string &exchange,
                                    const std::string &routing_key) throw();

  virtual ~MessageReturnedException() throw() {}

  BasicMessage::ptr_t message() const throw() { return m_message; }
  boost::uint32_t reply_code() const throw() { return m_reply_code; }
  std::string reply_text() const throw() { return m_reply_text; }
  std::string exchange() const throw() { return m_exchange; }
  std::string routing_key() const throw() { return m_routing_key; }

 private:
  BasicMessage::ptr_t m_message;
  boost::uint32_t m_reply_code;
  std::string m_reply_text;
  std::string m_exchange;
  std::string m_routing_key;
  mutable std::string m_what;
};

}  // namespace AmqpClient

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif  // SIMPLEAMQPCLIENT_MESSAGERETURNEDEXCEPTION_H
