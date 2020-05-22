/**
 * Copyright (C) 2019 leoetlino <leo@leolam.fr>
 *
 * This file is part of nlzss11.
 *
 * nlzss11 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * nlzss11 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with nlzss11.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <array>
#include <optional>
#include <vector>

#include <span.hpp>

#include "common/binary_reader.h"
#include "common/types.h"

namespace nlzss11 {

/// Reads the header of the data and returns the uncompressed size of the file,
/// if the header is correct
/// @param data Source data
std::optional<u32> GetUncompressedFilesize(tcb::span<const u8> data);

/// @param src  Source data
/// @param level  Compression level (6 to 9; 6 is fastest and 9 is slowest)
std::vector<u8> Compress(tcb::span<const u8> src, int level = 7);

std::vector<u8> Decompress(tcb::span<const u8> src);
// For increased flexibility, allocating the destination buffer can be done manually.
// In that case, the header is assumed to be valid, and the buffer size
// must be equal to the uncompressed data size.
void Decompress(tcb::span<const u8> src, tcb::span<u8> dst);
// Same, but additionally assumes that the source is well-formed.
// DO NOT USE THIS FOR UNTRUSTED SOURCES.
void DecompressUnsafe(tcb::span<const u8> src, tcb::span<u8> dst);

}  // namespace nlzss11
