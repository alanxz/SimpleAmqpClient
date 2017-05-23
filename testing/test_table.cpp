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

#include "connected_test.h"

#include <boost/variant/get.hpp>

#include <algorithm>

using namespace AmqpClient;

TEST(table_value, void_value) {
  TableValue value;
  EXPECT_EQ(TableValue::VT_void, value.GetType());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetInteger(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);

  value.Set();
  EXPECT_EQ(TableValue::VT_void, value.GetType());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetInteger(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);
}

TEST(table_value, bool_value) {
  bool v1 = true;
  bool v2 = false;

  TableValue value(v1);
  EXPECT_EQ(TableValue::VT_bool, value.GetType());

  EXPECT_EQ(v1, value.GetBool());
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetInteger(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);

  value.Set(v2);
  EXPECT_EQ(TableValue::VT_bool, value.GetType());

  EXPECT_EQ(v2, value.GetBool());
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetInteger(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);
}

TEST(table_value, uint8_value) {
  boost::uint8_t v1 = 1;
  boost::uint8_t v2 = 2;

  TableValue value(v1);
  EXPECT_EQ(TableValue::VT_uint8, value.GetType());
  EXPECT_EQ(v1, value.GetUint8());
  EXPECT_EQ(v1, value.GetInteger());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);

  value.Set(v2);
  EXPECT_EQ(TableValue::VT_uint8, value.GetType());
  EXPECT_EQ(v2, value.GetUint8());
  EXPECT_EQ(v2, value.GetInteger());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);
}

TEST(table_value, int8_value) {
  boost::int8_t v1 = 1;
  boost::int8_t v2 = 2;

  TableValue value(v1);
  EXPECT_EQ(TableValue::VT_int8, value.GetType());
  EXPECT_EQ(v1, value.GetInt8());
  EXPECT_EQ(v1, value.GetInteger());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);

  value.Set(v2);
  EXPECT_EQ(TableValue::VT_int8, value.GetType());
  EXPECT_EQ(v2, value.GetInt8());
  EXPECT_EQ(v2, value.GetInteger());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);
}

TEST(table_value, uint16_value) {
  boost::uint16_t v1 = 1;
  boost::uint16_t v2 = 2;

  TableValue value(v1);
  EXPECT_EQ(TableValue::VT_uint16, value.GetType());
  EXPECT_EQ(v1, value.GetUint16());
  EXPECT_EQ(v1, value.GetInteger());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);

  value.Set(v2);
  EXPECT_EQ(TableValue::VT_uint16, value.GetType());
  EXPECT_EQ(v2, value.GetUint16());
  EXPECT_EQ(v2, value.GetInteger());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);
}

TEST(table_value, int16_value) {
  boost::int16_t v1 = 1;
  boost::int16_t v2 = 2;

  TableValue value(v1);
  EXPECT_EQ(TableValue::VT_int16, value.GetType());
  EXPECT_EQ(v1, value.GetInt16());
  EXPECT_EQ(v1, value.GetInteger());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);

  value.Set(v2);
  EXPECT_EQ(TableValue::VT_int16, value.GetType());
  EXPECT_EQ(v2, value.GetInt16());
  EXPECT_EQ(v2, value.GetInteger());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);
}

TEST(table_value, uint32_value) {
  boost::uint32_t v1 = 1;
  boost::uint32_t v2 = 2;

  TableValue value(v1);
  EXPECT_EQ(TableValue::VT_uint32, value.GetType());
  EXPECT_EQ(v1, value.GetUint32());
  EXPECT_EQ(v1, value.GetInteger());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);

  value.Set(v2);
  EXPECT_EQ(TableValue::VT_uint32, value.GetType());
  EXPECT_EQ(v2, value.GetUint32());
  EXPECT_EQ(v2, value.GetInteger());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);
}

TEST(table_value, int32_value) {
  boost::int32_t v1 = 1;
  boost::int32_t v2 = 2;

  TableValue value(v1);
  EXPECT_EQ(TableValue::VT_int32, value.GetType());
  EXPECT_EQ(v1, value.GetInt32());
  EXPECT_EQ(v1, value.GetInteger());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);

  value.Set(v2);
  EXPECT_EQ(TableValue::VT_int32, value.GetType());
  EXPECT_EQ(v2, value.GetInt32());
  EXPECT_EQ(v2, value.GetInteger());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);
}

TEST(table_value, uint64_value) {
  boost::uint64_t v1 = 1;
  boost::uint64_t v2 = 2;

  TableValue value(v1);
  EXPECT_EQ(TableValue::VT_uint64, value.GetType());
  EXPECT_EQ(v1, value.GetUint64());
  EXPECT_EQ(v1, value.GetInteger());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);

  value.Set(v2);
  EXPECT_EQ(TableValue::VT_uint64, value.GetType());
  EXPECT_EQ(v2, value.GetUint64());
  EXPECT_EQ(v2, value.GetInteger());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);
}

TEST(table_value, uint64_value_larger_than_int64_max) {
  const boost::uint64_t maxInt64 = static_cast<uint64_t>(std::numeric_limits<int64_t>::max());
  boost::uint64_t v1 = maxInt64 + 1;
  boost::uint64_t v2 = maxInt64 + 2;

  TableValue value(v1);
  EXPECT_EQ(TableValue::VT_uint64, value.GetType());
  EXPECT_EQ(v1, value.GetUint64());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetInteger(), std::overflow_error);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);

  value.Set(v2);
  EXPECT_EQ(TableValue::VT_uint64, value.GetType());
  EXPECT_EQ(v2, value.GetUint64());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetInteger(), std::overflow_error);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);
}

TEST(table_value, int64_value) {
  boost::int64_t v1 = 1;
  boost::int64_t v2 = 2;

  TableValue value(v1);
  EXPECT_EQ(TableValue::VT_int64, value.GetType());
  EXPECT_EQ(v1, value.GetInt64());
  EXPECT_EQ(v1, value.GetInteger());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);

  value.Set(v2);
  EXPECT_EQ(TableValue::VT_int64, value.GetType());
  EXPECT_EQ(v2, value.GetInt64());
  EXPECT_EQ(v2, value.GetInteger());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);
}

TEST(table_value, float_value) {
  float v1 = 1.;
  float v2 = 2.;

  TableValue value(v1);
  EXPECT_EQ(TableValue::VT_float, value.GetType());
  EXPECT_EQ(v1, value.GetFloat());
  EXPECT_EQ(v1, value.GetReal());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetInteger(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);

  value.Set(v2);
  EXPECT_EQ(TableValue::VT_float, value.GetType());
  EXPECT_EQ(v2, value.GetFloat());
  EXPECT_EQ(v2, value.GetReal());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetInteger(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);
}

TEST(table_value, double_value) {
  double v1 = 1;
  double v2 = 2;

  TableValue value(v1);
  EXPECT_EQ(TableValue::VT_double, value.GetType());
  EXPECT_EQ(v1, value.GetDouble());
  EXPECT_EQ(v1, value.GetReal());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetInteger(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);

  value.Set(v2);
  EXPECT_EQ(TableValue::VT_double, value.GetType());
  EXPECT_EQ(v2, value.GetDouble());
  EXPECT_EQ(v2, value.GetReal());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetInteger(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);
}

TEST(table_value, string_value) {
  std::string v1 = "1";
  std::string v2 = "2";

  TableValue value(v1);
  EXPECT_EQ(TableValue::VT_string, value.GetType());
  EXPECT_EQ(v1, value.GetString());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetInteger(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);

  value.Set(v2);
  EXPECT_EQ(TableValue::VT_string, value.GetType());
  EXPECT_EQ(v2, value.GetString());

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetInteger(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);
}

TEST(table_value, array_value) {
  Array v1;
  v1.push_back(TableValue("first"));

  Array v2;
  v2.push_back(TableValue((int32_t)2));

  TableValue value(v1);
  EXPECT_EQ(TableValue::VT_array, value.GetType());
  Array v1a = value.GetArray();
  EXPECT_TRUE(v1.size() == v1a.size());
  EXPECT_TRUE(std::equal(v1.begin(), v1.end(), v1a.begin()));

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetInteger(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);

  value.Set(v2);
  EXPECT_EQ(TableValue::VT_array, value.GetType());
  Array v2a = value.GetArray();
  EXPECT_TRUE(v2.size() == v2a.size());
  EXPECT_TRUE(std::equal(v2.begin(), v2.end(), v2a.begin()));

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetInteger(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetTable(), boost::bad_get);
}

TEST(table_value, table_value) {
  Table v1;
  v1.insert(TableEntry("one", 10));

  Table v2;
  v2.insert(TableEntry("two", 22.2));

  TableValue value(v1);
  EXPECT_EQ(TableValue::VT_table, value.GetType());
  Table v1a = value.GetTable();
  EXPECT_TRUE(v1.size() == v1a.size());
  EXPECT_TRUE(std::equal(v1.begin(), v1.end(), v1a.begin()));

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetInteger(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);

  value.Set(v2);
  EXPECT_EQ(TableValue::VT_table, value.GetType());
  Table v2a = value.GetTable();
  EXPECT_TRUE(v2.size() == v2a.size());
  EXPECT_TRUE(std::equal(v2.begin(), v2.end(), v2a.begin()));

  EXPECT_THROW(value.GetBool(), boost::bad_get);
  EXPECT_THROW(value.GetUint8(), boost::bad_get);
  EXPECT_THROW(value.GetInt8(), boost::bad_get);
  EXPECT_THROW(value.GetUint16(), boost::bad_get);
  EXPECT_THROW(value.GetInt16(), boost::bad_get);
  EXPECT_THROW(value.GetUint32(), boost::bad_get);
  EXPECT_THROW(value.GetInt32(), boost::bad_get);
  EXPECT_THROW(value.GetUint64(), boost::bad_get);
  EXPECT_THROW(value.GetInt64(), boost::bad_get);
  EXPECT_THROW(value.GetInteger(), boost::bad_get);
  EXPECT_THROW(value.GetFloat(), boost::bad_get);
  EXPECT_THROW(value.GetDouble(), boost::bad_get);
  EXPECT_THROW(value.GetReal(), boost::bad_get);
  EXPECT_THROW(value.GetString(), boost::bad_get);
  EXPECT_THROW(value.GetArray(), boost::bad_get);
}

TEST(table_value, equality) {
  TableValue void_val1;
  TableValue void_val2;
  EXPECT_EQ(void_val1, void_val1);
  EXPECT_EQ(void_val1, void_val2);

  TableValue bool_val1(true);
  TableValue bool_val2(bool_val1);
  TableValue bool_val3(false);
  TableValue bool_val4;
  bool_val4.Set(bool_val3.GetBool());
  EXPECT_EQ(bool_val1, bool_val1);
  EXPECT_EQ(bool_val1, bool_val2);
  EXPECT_NE(bool_val1, bool_val3);
  EXPECT_NE(void_val1, bool_val1);
  EXPECT_EQ(bool_val3, bool_val4);

  TableValue int8_val1(int8_t(8));
  TableValue int8_val2(int8_val1);
  TableValue int8_val3(int8_t(9));
  EXPECT_EQ(int8_val1, int8_val1);
  EXPECT_EQ(int8_val1, int8_val2);
  EXPECT_NE(int8_val1, int8_val3);

  TableValue string_val1("one");
  TableValue string_val2(std::string("one"));
  std::string empty;
  TableValue string_val3(empty);
  EXPECT_EQ(string_val1, string_val1);
  EXPECT_EQ(string_val1, string_val2);
  EXPECT_NE(string_val1, string_val3);

  std::vector<TableValue> vec_val1;
  vec_val1.push_back(void_val1);
  vec_val1.push_back(int8_val1);
  std::vector<TableValue> vec_val2;
  vec_val2.push_back(bool_val1);
  vec_val2.push_back(void_val1);
  TableValue array_val1(vec_val1);
  TableValue array_val2(array_val1);
  TableValue array_val3(vec_val2);

  EXPECT_EQ(array_val1, array_val1);
  EXPECT_EQ(array_val1, array_val2);
  EXPECT_NE(array_val1, array_val3);

  Table tbl1;
  tbl1.insert(TableEntry("key1", int8_val1));
  tbl1.insert(TableEntry("key2", "string"));
  Table tbl2;
  tbl2.insert(TableEntry("key1", void_val1));
  tbl2.insert(TableEntry("array", array_val1));

  TableValue table_val1(tbl1);
  TableValue table_val2(tbl1);
  TableValue table_val3(tbl2);

  EXPECT_EQ(table_val1, table_val1);
  EXPECT_EQ(table_val1, table_val2);
  EXPECT_NE(table_val1, table_val3);
}

TEST(table, convert_to_rabbitmq) {
  Table table_in;
  table_in.insert(TableEntry("void_key", TableValue()));
  table_in.insert(TableEntry("bool_key", true));
  table_in.insert(TableEntry("uint8_key", uint8_t(8)));
  table_in.insert(TableEntry("int8_key", int8_t(8)));
  table_in.insert(TableEntry("uint16_key", uint16_t(16)));
  table_in.insert(TableEntry("int16_key", int16_t(16)));
  table_in.insert(TableEntry("uint32_key", uint32_t(32)));
  table_in.insert(TableEntry("int32_key", int32_t(32)));
  table_in.insert(TableEntry("uint64_key", uint64_t(64)));
  table_in.insert(TableEntry("int64_key", int64_t(64)));
  table_in.insert(TableEntry("float_key", float(1.5)));
  table_in.insert(TableEntry("double_key", double(2.25)));
  table_in.insert(TableEntry("string_key", "A string!"));

  std::vector<TableValue> array_in;
  array_in.push_back(TableValue(false));
  array_in.push_back(TableValue(int32_t(10)));
  array_in.push_back(TableValue(std::string("Another string")));

  table_in.insert(TableEntry("array_key", array_in));

  Table table_inner;
  table_inner.insert(TableEntry("inner_string", "An inner table"));
  table_inner.insert(TableEntry("inner array", array_in));

  table_in.insert(TableEntry("table_key", table_inner));

  BasicMessage::ptr_t message = BasicMessage::Create();
  message->HeaderTable(table_in);

  EXPECT_TRUE(message->HeaderTableIsSet());
  Table table_out = message->HeaderTable();
  EXPECT_EQ(table_in.size(), table_out.size());
  EXPECT_TRUE(std::equal(table_in.begin(), table_in.end(), table_out.begin()));
}

TEST(table, convert_to_rabbitmq_empty) {
  Table table_in;

  BasicMessage::ptr_t message = BasicMessage::Create();
  message->HeaderTable(table_in);

  Table table_out = message->HeaderTable();
  EXPECT_EQ(0, table_out.size());
}

TEST_F(connected_test, basic_message_header_roundtrip) {
  Table table_in;
  table_in.insert(TableEntry("void_key", TableValue()));
  table_in.insert(TableEntry("bool_key", true));
  table_in.insert(TableEntry("uint8_key", uint8_t(8)));
  table_in.insert(TableEntry("int8_key", int8_t(8)));
  table_in.insert(TableEntry("uint16_key", uint16_t(16)));
  table_in.insert(TableEntry("int16_key", int16_t(16)));
  table_in.insert(TableEntry("uint32_key", uint32_t(32)));
  table_in.insert(TableEntry("int32_key", int32_t(32)));
  table_in.insert(TableEntry("uint64_key", uint64_t(64)));
  table_in.insert(TableEntry("int64_key", int64_t(64)));
  table_in.insert(TableEntry("float_key", float(1.5)));
  table_in.insert(TableEntry("double_key", double(2.25)));
  table_in.insert(TableEntry("string_key", "A string!"));

  std::vector<TableValue> array_in;
  array_in.push_back(TableValue(false));
  array_in.push_back(TableValue(int32_t(10)));
  array_in.push_back(TableValue(std::string("Another string")));

  table_in.insert(TableEntry("array_key", array_in));

  Table table_inner;
  table_inner.insert(TableEntry("inner_string", "An inner table"));
  table_inner.insert(TableEntry("inner array", array_in));

  table_in.insert(TableEntry("table_key", table_inner));

  std::string queue = channel->DeclareQueue("");
  std::string tag = channel->BasicConsume(queue, "");

  BasicMessage::ptr_t message_in = BasicMessage::Create("Body");
  message_in->HeaderTable(table_in);

  channel->BasicPublish("", queue, message_in);

  Envelope::ptr_t envelope = channel->BasicConsumeMessage(tag);
  BasicMessage::ptr_t message_out = envelope->Message();
  Table table_out = message_out->HeaderTable();

  EXPECT_EQ(table_in.size(), table_out.size());
  EXPECT_TRUE(std::equal(table_in.begin(), table_in.end(), table_out.begin()));
}

TEST_F(connected_test, basic_message_empty_table_roundtrip) {
  std::string queue = channel->DeclareQueue("");
  std::string tag = channel->BasicConsume(queue, "");

  Table table_in;

  BasicMessage::ptr_t message_in = BasicMessage::Create("Body");
  message_in->HeaderTable(table_in);

  channel->BasicPublish("", queue, message_in);

  Envelope::ptr_t envelope = channel->BasicConsumeMessage(tag);
  BasicMessage::ptr_t message_out = envelope->Message();
  Table table_out = message_out->HeaderTable();

  EXPECT_EQ(table_in.size(), table_out.size());
  EXPECT_TRUE(std::equal(table_in.begin(), table_in.end(), table_out.begin()));
}
