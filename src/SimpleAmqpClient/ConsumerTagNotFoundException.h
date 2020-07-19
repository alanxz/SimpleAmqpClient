#ifndef SIMPLEAMQPCLIENT_CONSUMERTAGNOTFOUND_H
#define SIMPLEAMQPCLIENT_CONSUMERTAGNOTFOUND_H
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

#include <stdexcept>

#include "Util.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251 4275)
#endif

/// @file SimpleAmqpClient/ConsumerTagNotFoundException.h
/// Defines AmqpClient::ConsumerTagNotFoundException

namespace AmqpClient {

/**
 * "Consumer tag not found" exception
 *
 * @see BasicConsume
 */
class SIMPLEAMQPCLIENT_EXPORT ConsumerTagNotFoundException
    : public std::runtime_error {
 public:
  /// Constructor
  ConsumerTagNotFoundException() throw()
      : std::runtime_error("The specified consumer tag is unknown") {}
  /// Destructor
  virtual ~ConsumerTagNotFoundException() throw() {}
};

}  // namespace AmqpClient
#endif  // SIMPLEAMQPCLIENT_CONSUMERTAGNOTFOUND_H
