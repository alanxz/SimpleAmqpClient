#ifndef SIMPLEAMQPCLIENT_TABLEIMPL_H
#define SIMPLEAMQPCLIENT_TABLEIMPL_H
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

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/variant/variant.hpp>

#include <string>
#include <vector>

#include <amqp.h>

namespace AmqpClient {
namespace Detail {

typedef boost::shared_ptr<amqp_pool_t> amqp_pool_ptr_t;

struct void_t {};

inline bool operator==(const void_t &, const void_t &) { return true; }

typedef std::vector<TableValue> array_t;

typedef boost::variant<void_t, bool, boost::int8_t, boost::int16_t,
                       boost::int32_t, boost::int64_t, float, double,
                       std::string, array_t, Table, boost::uint8_t,
                       boost::uint16_t, boost::uint32_t, boost::uint64_t>
    value_t;

class TableValueImpl {
 public:
  explicit TableValueImpl(const value_t &v) : m_value(v) {}
  virtual ~TableValueImpl() {}

  value_t m_value;

  static amqp_table_t CreateAmqpTable(const Table &table,
                                      amqp_pool_ptr_t &pool);

  static Table CreateTable(const amqp_table_t &table);

  static amqp_table_t CopyTable(const amqp_table_t &table,
                                amqp_pool_ptr_t &pool);

 private:
  static amqp_table_t CreateAmqpTableInner(const Table &table,
                                           amqp_pool_t &pool);
  static TableValue CreateTableValue(const amqp_field_value_t &entry);
  static amqp_table_t CopyTableInner(const amqp_table_t &table,
                                     amqp_pool_t &pool);
  static amqp_field_value_t CopyValue(const amqp_field_value_t value,
                                      amqp_pool_t &pool);

 public:
  class generate_field_value
      : public boost::static_visitor<amqp_field_value_t> {
   public:
    explicit generate_field_value(amqp_pool_t &p) : pool(p) {}
    virtual ~generate_field_value() {}

    amqp_field_value_t operator()(const void_t) const;
    amqp_field_value_t operator()(const bool value) const;
    amqp_field_value_t operator()(const boost::uint8_t value) const;
    amqp_field_value_t operator()(const boost::int8_t value) const;
    amqp_field_value_t operator()(const boost::uint16_t value) const;
    amqp_field_value_t operator()(const boost::int16_t value) const;
    amqp_field_value_t operator()(const boost::uint32_t value) const;
    amqp_field_value_t operator()(const boost::int32_t value) const;
    amqp_field_value_t operator()(const boost::uint64_t value) const;
    amqp_field_value_t operator()(const boost::int64_t value) const;
    amqp_field_value_t operator()(const float value) const;
    amqp_field_value_t operator()(const double value) const;
    amqp_field_value_t operator()(const std::string &value) const;
    amqp_field_value_t operator()(const array_t &value) const;
    amqp_field_value_t operator()(const Table &value) const;

   private:
    amqp_pool_t &pool;
  };
};

}  // namespace Detail
}  // namespace AmqpClient
#endif  // SIMPLEAMQPCLIENT_TABLEIMPL_H
