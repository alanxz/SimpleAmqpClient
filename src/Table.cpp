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

#include "SimpleAmqpClient/Table.h"
#include "SimpleAmqpClient/TableImpl.h"

#include <boost/variant/get.hpp>

#include <algorithm>
#include <iterator>
#include <limits>
#include <stdexcept>

namespace AmqpClient {
TableValue::TableValue()
    : m_impl(new Detail::TableValueImpl(Detail::void_t())) {}

TableValue::TableValue(bool value)
    : m_impl(new Detail::TableValueImpl(value)) {}

TableValue::TableValue(boost::uint8_t value)
    : m_impl(new Detail::TableValueImpl(value)) {}

TableValue::TableValue(boost::int8_t value)
    : m_impl(new Detail::TableValueImpl(value)) {}

TableValue::TableValue(boost::uint16_t value)
    : m_impl(new Detail::TableValueImpl(value)) {}

TableValue::TableValue(boost::int16_t value)
    : m_impl(new Detail::TableValueImpl(value)) {}

TableValue::TableValue(boost::uint32_t value)
    : m_impl(new Detail::TableValueImpl(value)) {}

TableValue::TableValue(boost::int32_t value)
    : m_impl(new Detail::TableValueImpl(value)) {}

TableValue::TableValue(boost::uint64_t value)
    : m_impl(new Detail::TableValueImpl(value)) {}

TableValue::TableValue(boost::int64_t value)
    : m_impl(new Detail::TableValueImpl(value)) {}

TableValue::TableValue(float value)
    : m_impl(new Detail::TableValueImpl(value)) {}

TableValue::TableValue(double value)
    : m_impl(new Detail::TableValueImpl(value)) {}

TableValue::TableValue(const char *value)
    : m_impl(new Detail::TableValueImpl(std::string(value))) {}

TableValue::TableValue(const std::string &value)
    : m_impl(new Detail::TableValueImpl(value)) {}

TableValue::TableValue(const std::vector<TableValue> &values)
    : m_impl(new Detail::TableValueImpl(values)) {}

TableValue::TableValue(const Table &value)
    : m_impl(new Detail::TableValueImpl(value)) {}

TableValue::TableValue(const TableValue &l)
    : m_impl(new Detail::TableValueImpl(l.m_impl->m_value)) {}

TableValue &TableValue::operator=(const TableValue &l) {
  if (this != &l) {
    m_impl->m_value = l.m_impl->m_value;
  }
  return *this;
}

bool operator==(const Array &l, const Array &r) {
  if (l.size() == r.size()) {
    return std::equal(l.begin(), l.end(), r.begin());
  }
  return false;
}

bool operator==(const Table &l, const Table &r) {
  if (l.size() == r.size()) {
    return std::equal(l.begin(), l.end(), r.begin());
  }
  return false;
}

bool TableValue::operator==(const TableValue &l) const {
  if (this == &l) {
    return true;
  }

  return m_impl->m_value == l.m_impl->m_value;
}

bool TableValue::operator!=(const TableValue &l) const {
  if (this == &l) {
    return false;
  }

  return !(m_impl->m_value == l.m_impl->m_value);
}

TableValue::~TableValue() {}

TableValue::ValueType TableValue::GetType() const {
  return static_cast<ValueType>(m_impl->m_value.which());
}

bool TableValue::GetBool() const {
  return boost::get<bool>(m_impl->m_value);
}


boost::uint8_t TableValue::GetUint8() const {
  return boost::get<boost::uint8_t>(m_impl->m_value);
}

boost::int8_t TableValue::GetInt8() const {
  return boost::get<boost::int8_t>(m_impl->m_value);
}

boost::uint16_t TableValue::GetUint16() const {
  return boost::get<boost::uint16_t>(m_impl->m_value);
}

boost::int16_t TableValue::GetInt16() const {
  return boost::get<boost::int16_t>(m_impl->m_value);
}

boost::uint32_t TableValue::GetUint32() const {
  return boost::get<boost::uint32_t>(m_impl->m_value);
}

boost::int32_t TableValue::GetInt32() const {
  return boost::get<boost::int32_t>(m_impl->m_value);
}

boost::uint64_t TableValue::GetUint64() const {
  return boost::get<boost::uint64_t>(m_impl->m_value);
}

boost::int64_t TableValue::GetInt64() const {
  return boost::get<boost::int64_t>(m_impl->m_value);
}

boost::int64_t TableValue::GetInteger() const {
  switch (m_impl->m_value.which()) {
    case VT_uint8:
      return GetUint8();
    case VT_int8:
      return GetInt8();
    case VT_uint16:
      return GetUint16();
    case VT_int16:
      return GetInt16();
    case VT_uint32:
      return GetUint32();
    case VT_int32:
      return GetInt32();
    case VT_uint64:
    {
      const boost::uint64_t value = GetUint64();
      if (value > std::numeric_limits<int64_t>::max())
        throw std::overflow_error(
          "Result of GetUint64() is out of range.");
      return value;
    }
    case VT_int64:
      return GetInt64();
    default:
      throw boost::bad_get();
  }
}

float TableValue::GetFloat() const {
  return boost::get<float>(m_impl->m_value);
}

double TableValue::GetDouble() const {
  return boost::get<double>(m_impl->m_value);
}

double TableValue::GetReal() const {
  switch (m_impl->m_value.which()) {
    case VT_float:
      return GetFloat();
    case VT_double:
      return GetDouble();
    default:
      throw boost::bad_get();
  }
}

std::string TableValue::GetString() const {
  return boost::get<std::string>(m_impl->m_value);
}

std::vector<TableValue> TableValue::GetArray() const {
  return boost::get<Detail::array_t>(m_impl->m_value);
}

Table TableValue::GetTable() const {
  return boost::get<Table>(m_impl->m_value);
}

void TableValue::Set() { m_impl->m_value = Detail::void_t(); }

void TableValue::Set(bool value) { m_impl->m_value = value; }

void TableValue::Set(boost::uint8_t value) { m_impl->m_value = value; }

void TableValue::Set(boost::int8_t value) { m_impl->m_value = value; }

void TableValue::Set(boost::uint16_t value) { m_impl->m_value = value; }

void TableValue::Set(boost::int16_t value) { m_impl->m_value = value; }

void TableValue::Set(boost::uint32_t value) { m_impl->m_value = value; }

void TableValue::Set(boost::int32_t value) { m_impl->m_value = value; }

void TableValue::Set(boost::uint64_t value) { m_impl->m_value = value; }

void TableValue::Set(boost::int64_t value) { m_impl->m_value = value; }

void TableValue::Set(float value) { m_impl->m_value = value; }

void TableValue::Set(double value) { m_impl->m_value = value; }

void TableValue::Set(const char *value) {
  m_impl->m_value = std::string(value);
}

void TableValue::Set(const std::string &value) { m_impl->m_value = value; }

void TableValue::Set(const std::vector<TableValue> &value) {
  m_impl->m_value = value;
}

void TableValue::Set(const Table &value) { m_impl->m_value = value; }

}  // namespace AmqpClient
