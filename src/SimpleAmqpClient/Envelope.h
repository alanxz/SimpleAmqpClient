#ifndef ENVELOPE_H
#define ENVELOPE_H
/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1
 *
 * ``The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is SimpleAmqpClient for RabbitMQ.
 *
 * The Initial Developer of the Original Code is Alan Antonuk.
 * Original code is Copyright (C) Alan Antonuk.
 *
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the GNU General Public License Version 2 or later (the "GPL"), in
 * which case the provisions of the GPL are applicable instead of those
 * above. If you wish to allow use of your version of this file only
 * under the terms of the GPL, and not to allow others to use your
 * version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the
 * notice and other provisions required by the GPL. If you do not
 * delete the provisions above, a recipient may use your version of
 * this file under the terms of any one of the MPL or the GPL.
 *
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
# pragma warning ( push )
# pragma warning ( disable: 4275 4251 )
#endif // _MSC_VER

namespace AmqpClient
{

class SIMPLEAMQPCLIENT_EXPORT Envelope : boost::noncopyable
{
public:
  typedef boost::shared_ptr<Envelope> ptr_t;

  friend ptr_t boost::make_shared<Envelope>(AmqpClient::BasicMessage::ptr_t const & a1, std::string const & a2, uint64_t const & a3,
    std::string const & a4, bool const & a5, std::string const & a6, uint16_t const & a7);

  /**
    * Creates an new envelope object
    * @param message the payload 
    * @param consumer_tag the consumer tag the message was delivered to
    * @param delivery_tag the delivery tag that the broker assigned to the message
    * @param exchange the name of the exchange that the message was published to
    * @param redelivered a flag indicating whether the message consumed as a result of a redelivery
    * @param routing_key the routing key that the message was published with
    * @returns a boost::shared_ptr to an envelope object
    */
  static ptr_t Create(const BasicMessage::ptr_t message, const std::string& consumer_tag, 
    const uint64_t delivery_tag, const std::string& exchange, bool redelivered, const std::string& routing_key, const uint16_t delivery_channel)
  {   return boost::make_shared<Envelope>(message, consumer_tag, delivery_tag, exchange, redelivered, routing_key, delivery_channel); }

private:
  explicit Envelope(const BasicMessage::ptr_t message, const std::string& consumer_tag,
    const uint64_t delivery_tag, const std::string& exchange, bool redelivered, const std::string& routing_key, const uint16_t delivery_channel);

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
    * The delivery tag is a unique tag for a given message assigned by the broker
    * This tag is used when Ack'ing a message
    *
    * @returns the delivery tag for a message
    */
  inline uint64_t DeliveryTag() const { return m_deliveryTag; }

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
    * @returns a string containing the routing key the message was published with
    */
  inline std::string RoutingKey() const { return m_routingKey; }

  inline uint16_t DeliveryChannel() const { return m_deliveryChannel; }
private:
  const BasicMessage::ptr_t m_message;
  const std::string m_consumerTag;
  const uint64_t m_deliveryTag;
  const std::string m_exchange;
  const bool m_redelivered;
  const std::string m_routingKey;
  const uint16_t m_deliveryChannel;
};

} // namespace AmqpClient

#ifdef _MSC_VER
# pragma warning ( pop )
#endif // _MSC_VER

#endif // ENVELOPE_H
