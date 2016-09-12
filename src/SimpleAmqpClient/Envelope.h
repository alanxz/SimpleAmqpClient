#ifndef SIMPLEAMQPCLIENT_ENVELOPE_H
#define SIMPLEAMQPCLIENT_ENVELOPE_H
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
#include "SimpleAmqpClient/Util.h"

#include <boost/cstdint.hpp>
#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <string>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4275 4251)
#endif  // _MSC_VER

namespace AmqpClient {

class SIMPLEAMQPCLIENT_EXPORT Envelope : boost::noncopyable {
 public:
  typedef boost::shared_ptr<Envelope> ptr_t;

  /**
    * Creates an new envelope object
    * @param message the payload
    * @param consumer_tag the consumer tag the message was delivered to
    * @param delivery_tag the delivery tag that the broker assigned to the
   * message
    * @param exchange the name of the exchange that the message was published to
    * @param redelivered a flag indicating whether the message consumed as a
   * result of a redelivery
    * @param routing_key the routing key that the message was published with
    * @returns a boost::shared_ptr to an envelope object
    */
  static ptr_t Create(const BasicMessage::ptr_t message,
                      const std::string &consumer_tag,
                      const boost::uint64_t delivery_tag,
                      const std::string &exchange, bool redelivered,
                      const std::string &routing_key,
                      const boost::uint16_t delivery_channel) {
    return boost::make_shared<Envelope>(message, consumer_tag, delivery_tag,
                                        exchange, redelivered, routing_key,
                                        delivery_channel);
  }

  explicit Envelope(const BasicMessage::ptr_t message,
                    const std::string &consumer_tag,
                    const boost::uint64_t delivery_tag,
                    const std::string &exchange, bool redelivered,
                    const std::string &routing_key,
                    const boost::uint16_t delivery_channel);

 public:
  /**
    * destructor
    */
  virtual ~Envelope();

  /**
    * Get the payload of the envelope
    *
    * @returns the message
    */
  inline BasicMessage::ptr_t Message() const { return m_message; }

  /**
    * Get the consumer tag for the consumer that delivered the message
    *
    * @returns the consumer that delivered the message
    */
  inline std::string ConsumerTag() const { return m_consumerTag; }

  /**
    * Get the delivery tag for the message.
    *
    * The delivery tag is a unique tag for a given message assigned by the
   * broker
    * This tag is used when Ack'ing a message
    *
    * @returns the delivery tag for a message
    */
  inline boost::uint64_t DeliveryTag() const { return m_deliveryTag; }

  /**
    * Get the name of the exchange that the message was published to
    *
    * @returns the name of the exchange the message was published to
    */
  inline std::string Exchange() const { return m_exchange; }

  /**
    * Get the flag that indicates whether the message was redelivered
    *
    * A flag that indicates whether the message was redelievered means
    * the broker tried to deliver the message and the client did not Ack
    * the message, so the message was requeued, or the client asked the broker
    * to Recover which forced all non-Acked messages to be redelivered
    *
    * @return a boolean flag indicating whether the message was redelivered
    */
  inline bool Redelivered() const { return m_redelivered; }

  /**
    * Get the routing key that the message was published with
    *
    * @returns a string containing the routing key the message was published
   * with
    */
  inline std::string RoutingKey() const { return m_routingKey; }

  inline boost::uint16_t DeliveryChannel() const { return m_deliveryChannel; }

  struct DeliveryInfo {
    boost::uint64_t delivery_tag;
    boost::uint16_t delivery_channel;
  };

  inline DeliveryInfo GetDeliveryInfo() const {
    DeliveryInfo info;
    info.delivery_tag = m_deliveryTag;
    info.delivery_channel = m_deliveryChannel;

    return info;
  }

 private:
  const BasicMessage::ptr_t m_message;
  const std::string m_consumerTag;
  const boost::uint64_t m_deliveryTag;
  const std::string m_exchange;
  const bool m_redelivered;
  const std::string m_routingKey;
  const boost::uint16_t m_deliveryChannel;
};

}  // namespace AmqpClient

#ifdef _MSC_VER
#pragma warning(pop)
#endif  // _MSC_VER

#endif  // SIMPLEAMQPCLIENT_ENVELOPE_H
