#ifndef SIMPLEAMQPCLIENT_MESSAGEREJECTEDEXCEPTION_H
#define SIMPLEAMQPCLIENT_MESSAGEREJECTEDEXCEPTION_H
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

#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>

#include "SimpleAmqpClient/BasicMessage.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251 4275)
#endif

namespace AmqpClient {

class SIMPLEAMQPCLIENT_EXPORT MessageRejectedException
    : public std::runtime_error {
 public:
  MessageRejectedException(uint64_t delivery_tag)
      : std::runtime_error(
            std::string("Message rejected: ")
                .append(boost::lexical_cast<std::string>(delivery_tag))),
        m_delivery_tag(delivery_tag) {}

  uint64_t GetDeliverTag() { return m_delivery_tag; }

 private:
  uint64_t m_delivery_tag;
};

}  // namespace AmqpClient

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif  // SIMPLEAMQPCLIENT_MESSAGEREJECTEDEXCEPTION_H
