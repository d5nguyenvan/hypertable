/** -*- c++ -*-
 * Copyright (C) 2011 Hypertable, Inc.
 *
 * This file is part of Hypertable.
 *
 * Hypertable is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 of the
 * License, or any later version.
 *
 * Hypertable is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "Common/Compat.h"
#include "Common/Error.h"
#include "Common/FailureInducer.h"
#include "Common/ScopeGuard.h"
#include "Common/Serialization.h"
#include "Common/System.h"
#include "Common/md5.h"

#include "AsyncComm/ResponseCallback.h"

#include "Hypertable/Lib/Key.h"
#include "Hypertable/Lib/TableScanner.h"

#include "OperationBalance.h"
#include "OperationProcessor.h"
#include "Utility.h"

using namespace Hypertable;
using namespace Hyperspace;

OperationBalance::OperationBalance(ContextPtr &context, BalancePlan &plan)
  : Operation(context, MetaLog::EntityType::OPERATION_BALANCE),  m_plan(plan) {
  initialize_dependencies();
}

OperationBalance::OperationBalance(ContextPtr &context,
                                   const MetaLog::EntityHeader &header_)
  : Operation(context, header_) {
  m_hash_code = md5_hash("OperationBalance");
}

OperationBalance::OperationBalance(ContextPtr &context, EventPtr &event)
  : Operation(context, event, MetaLog::EntityType::OPERATION_BALANCE) {
  const uint8_t *ptr = event->payload;
  size_t remaining = event->payload_len;
  decode_request(&ptr, &remaining);
  initialize_dependencies();
}

void OperationBalance::initialize_dependencies() {
  m_dependencies.insert(Dependency::INIT);
  m_hash_code = md5_hash("OperationBalance");
}


void OperationBalance::execute() {
#if 0
  int32_t state = get_state();

  HT_INFOF("Entering MoveRange-%lld %s state=%s",
           (Lld)header.id, m_range_name.c_str(), OperationState::get_text(state));

  switch (state) {

  case OperationState::INITIAL:
    {
      ScanSpec scan_spec;
      RowInterval ri;
      TableScannerPtr scanner;
      Cell cell;
      String location;
      String row = format("%s:%s", m_table.id, m_range.end_row);

      scan_spec.max_versions = 1;
      scan_spec.columns.clear();
      scan_spec.columns.push_back("Location");
      scan_spec.columns.push_back("StartRow");

      ri.start = row.c_str();
      ri.end = row.c_str();

      scanner = m_context->metadata_table->create_scanner(scan_spec);

      while (scanner->next(cell)) {
        if (!strcmp(cell.column_family, "Location")) {
          m_location_source = String((const char *)cell.value, cell.value_len);
          boost::trim(m_location_source);
        }
        else if (!strcmp(cell.column_family, "StartRow")) {
          if (strncmp(m_range.start_row, (const char *)cell.value, cell.value_len)) {
            complete_error(Error::RANGESERVER_RANGE_NOT_FOUND, "start row mismatch");
            return;
          }
        }
      }
    }

    if (m_location_source == "") {
      complete_error(Error::LOCATION_UNASSIGNED, m_range_name);
      return;
    }

    {
      ScopedLock lock(m_mutex);
      m_dependencies.insert(m_location_source);
      m_state = OperationState::STARTED;
    }
    return;

  case OperationState::STARTED:

    /**
     *  TODO: Propagate destination server info to move_range() operation
     */

    /**
     *  Relinquish Range
     */
    
    try {
      RangeServerClient rsc(m_context->comm);
      CommAddress addr;
      TableIdentifier *table = &m_table;
      RangeSpec *range = &m_range;

      addr.set_proxy(m_location_source);
      rsc.relinquish_range(addr, *table, *range);
    }
    catch (Exception &e) {
      complete_error(e);
      return;
    }
    complete_ok();
    break;

  default:
    HT_FATALF("Unrecognized state %d", state);
  }

  HT_INFOF("Leaving MoveRange-%lld %s (%s -> %s)",
           (Lld)header.id, m_range_name.c_str(),
           m_location_source.c_str(), m_location_dest.c_str());
#endif
}


void OperationBalance::display_state(std::ostream &os) {
}

size_t OperationBalance::encoded_state_length() const {
  return m_plan.encoded_length();
}

void OperationBalance::encode_state(uint8_t **bufp) const {
  m_plan.encode(bufp);
}

void OperationBalance::decode_state(const uint8_t **bufp, size_t *remainp) {
  decode_request(bufp, remainp);
}

void OperationBalance::decode_request(const uint8_t **bufp, size_t *remainp) {
  m_plan.decode(bufp, remainp);
}

const String OperationBalance::name() {
  return "OperationBalance";
}

const String OperationBalance::label() {
  return format("Balance (%u moves)", (unsigned)m_plan.moves.size());
}
