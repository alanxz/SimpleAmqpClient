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

// Put these first to avoid warnings about INT#_C macro redefinition
#include <amqp.h>

#include "SimpleAmqpClient/AmqpLibraryException.h"

#include <stdlib.h>

namespace AmqpClient {

AmqpLibraryException AmqpLibraryException::CreateException(int error_code) {
  std::string message(amqp_error_string2(error_code));

  return AmqpLibraryException(message, error_code);
}

AmqpLibraryException AmqpLibraryException::CreateException(
    int error_code, const std::string &context) {
  std::string message(context);
  message.append(": ");
  message.append(amqp_error_string2(error_code));

  return AmqpLibraryException(message, error_code);
}

AmqpLibraryException::AmqpLibraryException(const std::string &message,
                                           int error_code) throw()
    : std::runtime_error(message), m_errorCode(error_code) {}

}  // namespace AmqpClient
