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

#include <boost/variant/variant.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <amqp.h>

namespace AmqpClient {
namespace Detail {

amqp_pool_t *make_pool(int block_size);

struct pool_deleter {
  void operator()(amqp_pool_t *p) {
    if (p != nullptr) {
      empty_amqp_pool(p);
      delete p;
    }
  }
};

using amqp_pool_ptr_t = std::unique_ptr<amqp_pool_t, pool_deleter>;

struct void_t {};

inline bool operator==(const void_t & /*unused*/, const void_t & /*unused*/) {
  return true;
}

using array_t = std::vector<TableValue>;

using value_t =
    boost::variant<void_t, bool, std::int8_t, std::int16_t, std::int32_t,
                   std::int64_t, float, double, std::string, array_t, Table>;

class TableValueImpl {
 public:
  explicit TableValueImpl(value_t v) : m_value(std::move(v)) {}
  virtual ~TableValueImpl() = default;

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
  static amqp_field_value_t CopyValue(amqp_field_value_t value,
                                      amqp_pool_t &pool);

 public:
  class generate_field_value
      : public boost::static_visitor<amqp_field_value_t> {
   public:
    explicit generate_field_value(amqp_pool_t &p) : pool(p) {}
    virtual ~generate_field_value() = default;

    amqp_field_value_t operator()(void_t /*unused*/) const;
    amqp_field_value_t operator()(bool value) const;
    amqp_field_value_t operator()(std::int8_t value) const;
    amqp_field_value_t operator()(std::int16_t value) const;
    amqp_field_value_t operator()(std::int32_t value) const;
    amqp_field_value_t operator()(std::int64_t value) const;
    amqp_field_value_t operator()(float value) const;
    amqp_field_value_t operator()(double value) const;
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
