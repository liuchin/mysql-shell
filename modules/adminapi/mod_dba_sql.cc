/*
 * Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
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

#include "modules/adminapi/mod_dba_sql.h"
#include "utils/utils_sqlstring.h"

namespace mysh {
namespace dba {

  GRInstanceType get_gr_instance_type(mysh::mysql::Connection* connection) {
    GRInstanceType ret_val = GRInstanceType::Standalone;

    std::string query("select count(*) "
                      "from performance_schema.replication_group_members "
                      "where MEMBER_ID = @@server_uuid AND MEMBER_STATE IS NOT NULL AND MEMBER_STATE <> 'OFFLINE';");

    try {
      auto result = connection->run_sql(query);
      auto row = result->fetch_one();

      if (row) {
        if (row->get_value(0).as_int() != 0)
          ret_val = GRInstanceType::GroupReplication;
      }

    } catch (shcore::Exception& error) {
      if (error.code() != 1146) // Tables doesn't exist
        throw;
    }

    // The server is part of a Replication Group
    // Let's see if it is registered in the Metadata Store
    if (ret_val == GRInstanceType::GroupReplication) {
      query = "select count(*) from mysql_innodb_cluster_metadata.instances where mysql_server_uuid = @@server_uuid";
      try {
        auto result = connection->run_sql(query);
        auto row = result->fetch_one();

        if (row) {
          if (row->get_value(0).as_int() != 0)
            ret_val = GRInstanceType::InnoDBCluster;
        }
      }
      catch (shcore::Exception& error) {
        if (error.code() != 1146) // Tables doesn't exist
          throw;
      }
    }

    return ret_val;
  }

  void get_port_and_datadir(mysh::mysql::Connection* connection, int &port, std::string& datadir) {

    std::string query("select @@port, @@datadir;");

    // Any error will bubble up right away
    auto result = connection->run_sql(query);
    auto row = result->fetch_one();

    port = row->get_value(0).as_int();
    datadir = row->get_value(1).as_string();
  }

  void get_gtid_state_variables(mysh::mysql::Connection* connection, std::string &executed, std::string &purged) {
    // Retrieves the
    std::string query("show global variables where Variable_name in ('gtid_purged', 'gtid_executed')");

    // Any error will bubble up right away
    auto result = connection->run_sql(query);

    // Selects the gtid_executed value
    auto row = result->fetch_one();
    executed = row->get_value(1).as_string();

    // Selects the gtid_purged value
    row = result->fetch_one();
    purged = row->get_value(1).as_string();
  }

  SlaveReplicationState get_slave_replication_state(mysh::mysql::Connection* connection, std::string &slave_executed) {
    SlaveReplicationState ret_val;

    if (slave_executed.empty())
      ret_val = SlaveReplicationState::New;
    else {
      std::string query;
      query = shcore::sqlstring("select gtid_subset(?, @@global.gtid_executed)", 0) << slave_executed;

      auto result = connection->run_sql(query);
      auto row = result->fetch_one();

      int slave_is_subset = row->get_value(0).as_int();

      if (1 == slave_is_subset) {
        // If purged has more gtids than the executed on the slave
        // it means some data will not be recoverable
        query = shcore::sqlstring("select gtid_subtract(@@global.gtid_purged, ?)", 0) << slave_executed;

        result = connection->run_sql(query);
        row = result->fetch_one();

        std::string missed = row->get_value(0).as_string();

        if (missed.empty())
          ret_val = SlaveReplicationState::Recoverable;
        else
          ret_val = SlaveReplicationState::Irrecoverable;
      }
      else
        // If slave executed gtids are not a subset of master's it means the slave has
        // own data not valid for the cluster where it is being checked
        ret_val = SlaveReplicationState::Diverged;
    }

    return ret_val;
  }

}
}