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

#include <cstdint>
#include <memory>
#include <string>

#include "SimpleAmqpClient/BasicMessage.h"
#include "SimpleAmqpClient/Util.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4275 4251)
#endif  // _MSC_VER

/// @file SimpleAmqpClient/Envelope.h
/// The AmqpClient::Envelope class is defined in this header file.

namespace AmqpClient {

/**
 * A "message envelope" object containing the message body and delivery metadata
 */
class SIMPLEAMQPCLIENT_EXPORT Envelope {
 public:
  /// a `std::shared_ptr` pointer to Envelope
  typedef std::shared_ptr<Envelope> ptr_t;

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
   * @param delivery_channel channel ID of the delivery (see DeliveryInfo)
   * @returns a std::shared_ptr to an envelope object
   */
  static ptr_t Create(const BasicMessage::ptr_t message,
                      const std::string &consumer_tag,
                      const std::uint64_t delivery_tag,
                      const std::string &exchange, bool redelivered,
                      const std::string &routing_key,
                      const std::uint16_t delivery_channel) {
    return std::make_shared<Envelope>(message, consumer_tag, delivery_tag,
                                      exchange, redelivered, routing_key,
                                      delivery_channel);
  }

  /**
   * Construct a new Envelope object
   * @param message the payload
   * @param consumer_tag the consumer tag the message was delivered to
   * @param delivery_tag the delivery tag that the broker assigned to the
   * message
   * @param exchange the name of the exchange that the message was published to
   * @param redelivered a flag indicating whether the message consumed as a
   * result of a redelivery
   * @param routing_key the routing key that the message was published with
   * @param delivery_channel channel ID of the delivery (see DeliveryInfo)
   */
  explicit Envelope(const BasicMessage::ptr_t message,
                    const std::string &consumer_tag,
                    const std::uint64_t delivery_tag,
                    const std::string &exchange, bool redelivered,
                    const std::string &routing_key,
                    const std::uint16_t delivery_channel);

 public:
  // Non-copyable
  Envelope(const Envelope &) = delete;
  Envelope &operator=(const Envelope &) = delete;

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
  inline std::uint64_t DeliveryTag() const { return m_deliveryTag; }

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

  /**
   * Get the delivery channel
   */
  inline std::uint16_t DeliveryChannel() const { return m_deliveryChannel; }

  /**
   * A POD carrier of delivery-tag
   *
   * This is server-assigned and channel-specific.
   *
   * The delivery tag is valid only within the channel from which the message
   * was received. I.e. a client MUST NOT receive a message on one channel and
   * then acknowledge it on another.
   *
   * The server MUST NOT use a zero value for delivery tags. Zero is reserved
   * for client use, meaning "all messages so far received".
   */
  struct DeliveryInfo {
    /// A delivery tag, assigned by the broker to identify this delivery within
    /// a channel
    std::uint64_t delivery_tag;
    /// An ID of the delivery channel
    std::uint16_t delivery_channel;
  };

  /**
   * Getter of the delivery-tag
   */
  inline DeliveryInfo GetDeliveryInfo() const {
    DeliveryInfo info;
    info.delivery_tag = m_deliveryTag;
    info.delivery_channel = m_deliveryChannel;

    return info;
  }

 private:
  const BasicMessage::ptr_t m_message;
  const std::string m_consumerTag;
  const std::uint64_t m_deliveryTag;
  const std::string m_exchange;
  const bool m_redelivered;
  const std::string m_routingKey;
  const std::uint16_t m_deliveryChannel;
};

}  // namespace AmqpClient

#ifdef _MSC_VER
#pragma warning(pop)
#endif  // _MSC_VER

#endif  // SIMPLEAMQPCLIENT_ENVELOPE_H
