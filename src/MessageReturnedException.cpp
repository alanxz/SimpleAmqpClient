#include "SimpleAmqpClient/MessageReturnedException.h"

#include <sstream>

namespace AmqpClient
{
MessageReturnedException::MessageReturnedException(BasicMessage::ptr_t message, uint32_t reply_code, const std::string& reply_text,
    const std::string& exchange, const std::string& routing_key) throw() :
  m_message(message), m_reply_code(reply_code), m_reply_text(reply_text), m_exchange(exchange), m_routing_key(routing_key)
{
}

MessageReturnedException::MessageReturnedException(const MessageReturnedException& e) throw() :
  m_message(e.m_message), m_reply_code(e.m_reply_code), m_reply_text(e.m_reply_text), m_exchange(e.m_exchange), m_routing_key(e.m_routing_key)

{
}

MessageReturnedException& MessageReturnedException::operator=(const MessageReturnedException& e) throw()
{
  if (this == &e)
  {
    return *this;
  }

  m_message = e.m_message;
  m_reply_code = e.m_reply_code;
  m_reply_text = e.m_reply_text;
  m_exchange = e.m_exchange;
  m_routing_key = e.m_routing_key;
  
  return *this;
}

const char* MessageReturnedException::what() const throw()
{
  std::ostringstream oss;
  
  oss << "Message returned. Reply Code: " << m_reply_code << " " << m_reply_text;
  m_what = oss.str();

  return m_what.c_str();
}
} // namespace AmqpClient