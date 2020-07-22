#ifndef SIMPLEAMQPCLIENT_AMQPRESPONSELIBRARYEXCEPTION_H
#define SIMPLEAMQPCLIENT_AMQPRESPONSELIBRARYEXCEPTION_H
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
#include <stdexcept>
#include <string>

#include "SimpleAmqpClient/Util.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251 4275)
#endif

/// @file SimpleAmqpClient/AmqpResponseLibraryException.h
/// Defines AmqpClient::AmqpResponseLibraryException

struct amqp_rpc_reply_t_;

namespace AmqpClient {

/**
 * `AMQP_RESPONSE_LIBRARY_EXCEPTION`: an RPC response returned a library error
 */
class SIMPLEAMQPCLIENT_EXPORT AmqpResponseLibraryException
    : public std::runtime_error {
 public:
  /// Factory-construct from RPC reply and a string context
  static AmqpResponseLibraryException CreateException(
      const amqp_rpc_reply_t_ &reply, const std::string &context);

 protected:
  /// Construct an instance with exact message
  explicit AmqpResponseLibraryException(const std::string &message) throw();
};

}  // namespace AmqpClient

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif  // SIMPLEAMQPCLIENT_AMQPRESPONSELIBRARYEXCEPTION_H
