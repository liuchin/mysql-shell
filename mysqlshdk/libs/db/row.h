/*
 * Copyright (c) 2017, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

// MySQL DB access module, for use by plugins and others
// For the module that implements interactive DB functionality see mod_db

#ifndef MYSQLSHDK_LIBS_DB_ROW_H_
#define MYSQLSHDK_LIBS_DB_ROW_H_

#include <cstdint>
#include <set>
#include <string>
#include <utility>
#include <mysqlxclient/xdatetime.h>

#include "mysqlshdk_export.h"
#include "mysqlshdk/libs/db/column.h"

namespace mysqlshdk {
namespace db {

class SHCORE_PUBLIC IRow {
 public:
  IRow() {
  }
  // non-copiable
  IRow(const IRow &) = delete;
  void operator=(const IRow &) = delete;

  virtual uint32_t num_fields() const = 0;

  virtual Type get_type(uint32_t index) const = 0;
  virtual bool is_null(uint32_t index) const = 0;
  virtual std::string get_as_string(uint32_t index) const = 0;

  virtual std::string get_string(uint32_t index) const = 0;
  virtual int64_t get_int(uint32_t index) const = 0;
  virtual uint64_t get_uint(uint32_t index) const = 0;
  virtual float get_float(uint32_t index) const = 0;
  virtual double get_double(uint32_t index) const = 0;
  virtual std::pair<const char *, size_t> get_string_data(
      uint32_t index) const = 0;
  virtual uint64_t get_bit(uint32_t index) const = 0;

  virtual ~IRow() {
  }
};
}  // namespace db
}  // namespace mysqlshdk
#endif  // MYSQLSHDK_LIBS_DB_ROW_H_
