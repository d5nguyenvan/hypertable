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
#include "Common/Serialization.h"

#include <cstring>
#include <iostream>

#include "KeySpec.h"
#include "RangeMoveSpec.h"

using namespace std;
using namespace Hypertable;
using namespace Serialization;

size_t RangeMoveSpec::encoded_length() const {
  return table.encoded_length() + range.encoded_length() +
    encoded_length_vstr(source_location) + encoded_length_vstr(dest_location);
}

void RangeMoveSpec::encode(uint8_t **bufp) const {
  table.encode(bufp);
  range.encode(bufp);
  encode_vstr(bufp, source_location);
  encode_vstr(bufp, dest_location);
}

void RangeMoveSpec::decode(const uint8_t **bufp, size_t *remainp) {
  table.decode(bufp, remainp);
  range.decode(bufp, remainp);
  source_location = decode_vstr(bufp, remainp);
  dest_location = decode_vstr(bufp, remainp);
}

ostream &Hypertable::operator<<(ostream &os, const RangeMoveSpec &move_spec) {
  os <<"{RangeMoveSpec: " << move_spec.table << " " << move_spec.range
     <<" source_location="<< move_spec.source_location
     <<" dest_location="<< move_spec.dest_location << "}";

  return os;
}
