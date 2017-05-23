#ifndef SIMPLEAMQPCLIENT_TABLE_H
#define SIMPLEAMQPCLIENT_TABLE_H
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

#include "SimpleAmqpClient/Util.h"

#include <boost/cstdint.hpp>
#include <boost/scoped_ptr.hpp>

#include <map>
#include <string>
#include <vector>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

namespace AmqpClient {

namespace Detail {
class TableValueImpl;
}  // namespace Detail

/**
 * Table key
 *
 * Note this must be less than 128 bytes long
 */
typedef std::string TableKey;

class TableValue;

/**
 * Array field value
 */
typedef std::vector<TableValue> Array;

/**
 * Field table
 *
 * Is just an STL map
 */
typedef std::map<TableKey, TableValue> Table;

typedef Table::value_type TableEntry;

/**
 * A variant type for the Table Value
 */
class SIMPLEAMQPCLIENT_EXPORT TableValue {
 public:
  friend class Detail::TableValueImpl;

  /** Types enumeration */
  enum ValueType {
    VT_void = 0,    //< void type
    VT_bool = 1,    //< boolean type
    VT_int8 = 2,    //< 1-byte/char signed type
    VT_int16 = 3,   //< 2-byte/short signed type
    VT_int32 = 4,   //< 4-byte/int signed type
    VT_int64 = 5,   //< 8-byte/long long int signed type
    VT_float = 6,   //< single-precision floating point type
    VT_double = 7,  //< double-precision floating point type
    VT_string = 8,  //< string type
    VT_array = 9,   //< array of TableValues type
    VT_table = 10,  //< a table type
    VT_uint8 = 11,  //< 1-byte/char unsigned type
    VT_uint16 = 12, //< 2-byte/short unsigned type
    VT_uint32 = 13, //< 4-byte/int unsigned type
    VT_uint64 = 14  //< 8-byte/long long int unsigned type
  };

  /**
   * Construct void table value
   *
   * A table value that doesn't have any value associated with it
   */
  TableValue();

  /**
   * Construct a boolean table value
   *
   * @param value [in] the value
   */
  TableValue(bool value);

  /**
   * Construct a 1-byte unsigned integer value
   *
   * @param value [in] the value
   */
  TableValue(boost::uint8_t value);

  /**
   * Construct a 1-byte signed integer value
   *
   * @param value [in] the value
   */
  TableValue(boost::int8_t value);

  /**
   * Construct a 2-byte unsigned integer value
   *
   * @param value [in] the value
   */
  TableValue(boost::uint16_t value);

  /**
   * Construct a 2-byte signed integer value
   *
   * @param value [in] the value
   */
  TableValue(boost::int16_t value);

  /**
   * Construct a 4-byte unsigned integer value
   *
   * @param value [in] the value
   */
  TableValue(boost::uint32_t value);

  /**
   * Construct a 4-byte signed integer value
   *
   * @param value [in] the value
   */
  TableValue(boost::int32_t value);

  /**
   * Construct a 8-byte unsigned integer value
   *
   * @param value [in] the value
   */
  TableValue(boost::uint64_t value);

  /**
   * Construct a 8-byte signed integer value
   *
   * @param value [in] the value
   */
  TableValue(boost::int64_t value);

  /**
   * Construct a single-precision floating point value
   *
   * @param value [in] the value
   */
  TableValue(float value);

  /**
   * Construct a double-precision floating point value
   *
   * @param value [in] the value
   */
  TableValue(double value);

  /**
   * Construct a character string value
   *
   * @param value [in] the value
   */
  TableValue(const char *value);

  /**
   * Construct a character string value
   *
   * @param value [in] the value
   */
  TableValue(const std::string &value);

  /**
   * Construct an array value
   *
   * @param values [in] the value
   */
  TableValue(const std::vector<TableValue> &values);

  /**
   * Construct a Table value
   *
   * @param value [in] the value
   */
  TableValue(const Table &value);

  /**
   * Copy-constructor
   */
  TableValue(const TableValue &l);

  /**
   * Assignment operator
   */
  TableValue &operator=(const TableValue &l);

  /**
   * Equality operator
   */
  bool operator==(const TableValue &l) const;

  /**
   * In-equality operator
   */
  bool operator!=(const TableValue &l) const;

  /**
   * Destructor
   */
  virtual ~TableValue();

  /**
   * Get the type
   */
  ValueType GetType() const;

  /**
   * Get the boolean value
   *
   * @returns the value if its a VT_bool type, false otherwise
   */
  bool GetBool() const;

  /**
   * Get the uint8 value
   *
   * @returns the value if its a VT_uint8 type, 0 otherwise
   */
  boost::uint8_t GetUint8() const;

  /**
   * Get the int8 value
   *
   * @returns the value if its a VT_int8 type, 0 otherwise
   */
  boost::int8_t GetInt8() const;

  /**
   * Get the uint16 value
   *
   * @returns the value if its a VT_uint16 type, 0 otherwise
   */
  boost::uint16_t GetUint16() const;

  /**
   * Get the int16 value
   *
   * @returns the value if its a VT_int16 type, 0 otherwise
   */
  boost::int16_t GetInt16() const;

  /**
   * Get the uint32 value
   *
   * @returns the value if its a VT_uint32 type, 0 otherwise
   */
  boost::uint32_t GetUint32() const;

  /**
   * Get the int32 value
   *
   * @returns the value if its a VT_int32 type, 0 otherwise
   */
  boost::int32_t GetInt32() const;

  /**
   * Get the uint64 value
   *
   * @returns the value if its a VT_uint64 type, 0 otherwise
   */
  boost::uint64_t GetUint64() const;

  /**
   * Get the int64 value
   *
   * @returns the value if its a VT_int64 type, 0 otherwise
   */
  boost::int64_t GetInt64() const;

  /**
   * Get an integral number
   *
   * Works for uint64 up to std::numeric_limits<int64_t>::max(),
   * will throw a std::overflow_error otherwise. If the entire range
   * of uint64_t is possible, please use GetUint64()
   *
   * @returns an integer number if the ValueType is VT_uint8, VT_int8,
   * VT_uint16, VT_int16, VT_uint32, VT_int32, VT_uint64
   * or VT_int64 type, 0 otherwise
   */
  boost::int64_t GetInteger() const;

  /**
   * Get a float value
   *
   * @returns the value if its a VT_float type, 0. otherwise
   */
  float GetFloat() const;

  /**
   * Get a double value
   *
   * @returns the value if its a VT_double type, 0. otherwise
   */
  double GetDouble() const;

  /**
   * Get a floating-point value
   *
   * @returns the value if its a VT_float or VT_double type, 0. otherwise
   */
  double GetReal() const;

  /**
   * Get a string value
   *
   * @returns the value if its a VT_string type, an empty string otherwise
   */
  std::string GetString() const;

  /**
   * Gets an array
   *
   * @returns the value if its a VT_array type, an empty array otherwise
   */
  std::vector<TableValue> GetArray() const;

  /**
   * Gets a table
   *
   * @returns the value if its a VT_table type, an empty table otherwise
   */
  Table GetTable() const;

  /**
   * Sets the value as a void value
   */
  void Set();

  /**
   * Set the value as a boolean
   *
   * @param value [in] the value
   */
  void Set(bool value);

  /**
   * Set the value as a uint8_t
   *
   * @param value [in] the value
   */
  void Set(boost::uint8_t value);

  /**
   * Set the value as a int8_t
   *
   * @param value [in] the value
   */
  void Set(boost::int8_t value);

  /**
   * Set the value as a uint16_t
   *
   * @param value [in] the value
   */
  void Set(boost::uint16_t value);

  /**
   * Set the value as a int16_t
   *
   * @param value [in] the value
   */
  void Set(boost::int16_t value);

  /**
   * Set the value as a uint32_t
   *
   * @param value [in] the value
   */
  void Set(boost::uint32_t value);

  /**
   * Set the value as a int32_t
   *
   * @param value [in] the value
   */
  void Set(boost::int32_t value);

  /**
   * Set teh value as a uint64_t
   *
   * @param value [in] the value
   */
  void Set(boost::uint64_t value);

  /**
   * Set teh value as a int64_t
   *
   * @param value [in] the value
   */
  void Set(boost::int64_t value);

  /**
   * Set the value as a float
   *
   * @param value [in] the value
   */
  void Set(float value);

  /**
   * Set the value as a double
   *
   * @param value [in] the value
   */
  void Set(double value);

  /**
   * Set the value as a string
   *
   * @param value [in] the value
   */
  void Set(const char *value);

  /**
   * Set the value as a string
   *
   * @param value [in] the value
   */
  void Set(const std::string &value);

  /**
   * Set the value as an array
   *
   * @param value [in] the value
   */
  void Set(const std::vector<TableValue> &value);

  /**
   * Set the value as a table
   *
   * @param value [in] the value
   */
  void Set(const Table &value);

 private:
  boost::scoped_ptr<Detail::TableValueImpl> m_impl;
};

}  // namespace AmqpClient

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif  // SIMPLEAMQPCLIENT_TABLE_H
