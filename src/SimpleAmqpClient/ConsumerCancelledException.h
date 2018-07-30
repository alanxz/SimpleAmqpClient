#ifndef SIMPLEAMQPCLIENT_CONSUMERCANCELLEDEXCEPTION_H
#define SIMPLEAMQPCLIENT_CONSUMERCANCELLEDEXCEPTION_H
/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MIT
 *
 * Copyright (c) 2013 Alan Antonuk
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
#include <string>

/// @file SimpleAmqpClient/ConsumerCancelledException.h
/// Defines AmqpClient::ConsumerCancelledException

namespace AmqpClient {

/**
 * "Consumer was cancelled" exception
 *
 * Happens when the server ends a consumer subscription, e.g. when the
 * subscribed queue is being deleted, or when a client issues basic.cancel
 * request.
 */
class ConsumerCancelledException : public std::runtime_error {
 public:
  /// Constructor
  explicit ConsumerCancelledException(const std::string &consumer_tag) throw()
      : std::runtime_error(
            std::string("Consumer was cancelled: ").append(consumer_tag)),
        m_consumer_tag(consumer_tag) {}

  /// Destructor
  virtual ~ConsumerCancelledException() throw() {}

  /// Getter of the consumer tag
  std::string GetConsumerTag() const { return m_consumer_tag; }

 private:
  std::string m_consumer_tag;
};
}  // namespace AmqpClient
#endif  // SIMPLEAMQPCLIENT_CONSUMERCANCELLEDEXCEPTION_H
