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

#include "nlzss11.h"

#include <algorithm>
#include <bitset>
#include <cstring>

#include <zlib-ng.h>

#include "common/binary_reader.h"

namespace nlzss11 {

constexpr char Magic = 0x11;
const constexpr size_t ChunksPerGroup = 8;
constexpr size_t MaximumMatchLength = 0x111 + 0xFFFF;
constexpr size_t WindowSize = 0x1000;

std::optional<u32> GetUncompressedFilesize(tcb::span<const u8> data) {
  if (data.size() < 4) {
    return std::nullopt;
  }
  const auto magic = data[0];
  u32 file_size = data[1] | (data[2] << 8) | (data[3] << 16);
  // very big files have the size in bytes 4-8
  if (file_size == 0) {
    file_size = data[4] | (data[5] << 8) | (data[6] << 16) | (data[7] << 24);
  }
  if (magic != Magic)
    return std::nullopt;
  return file_size;
}

namespace {
class GroupWriter {
public:
  GroupWriter(std::vector<u8>& result) : m_result{result} { Reset(); }

  void HandleZlibMatch(u32 dist, u32 lc) {
    if (dist == 0) {
      // Literal.
      m_result.push_back(u8(lc));
    } else {
      // Back reference.
      m_group_header.set(7 - m_pending_chunks);
      constexpr u32 ZlibMinMatch = 3;
      WriteMatch(dist - 1, lc + ZlibMinMatch);
    }

    ++m_pending_chunks;
    if (m_pending_chunks == ChunksPerGroup) {
      m_result[m_group_header_offset] = u8(m_group_header.to_ulong());
      Reset();
    }
  }

  // Must be called after zlib has completed to ensure the last group is written.
  void Finalise() {
    if (m_pending_chunks != 0)
      m_result[m_group_header_offset] = u8(m_group_header.to_ulong());
  }

private:
  void Reset() {
    m_pending_chunks = 0;
    m_group_header.reset();
    m_group_header_offset = m_result.size();
    m_result.push_back(0xFF);
  }

  void WriteMatch(u32 distance, u32 length) {
    if (length <= 0xF + 1) {
      m_result.push_back((length - 1) << 4 | distance >> 8);
      m_result.push_back(distance & 0xFF);
    } else if (length <= 0x11 + 0xFF) {
      m_result.push_back((length - 0x11) >> 4);
      m_result.push_back(((length - 0x11) & 0xF) << 4 | distance >> 8);
      m_result.push_back(distance & 0xFF);
    } else {
      const size_t actual_length = std::min<size_t>(MaximumMatchLength, length) - 0x111;
      m_result.push_back(0x10 | actual_length >> 12);
      m_result.push_back((actual_length >> 4) & 0xFF);
      m_result.push_back((actual_length & 0xF) << 4 | distance >> 8);
      m_result.push_back(distance & 0xFF);
    }
  }

  std::vector<u8>& m_result;
  size_t m_pending_chunks;
  std::bitset<8> m_group_header;
  std::size_t m_group_header_offset;
};
}  // namespace

std::vector<u8> Compress(tcb::span<const u8> src, int level) {
  std::vector<u8> result(4);  // Header size
  result.reserve(src.size());

  // Write the header.
  result[0] = 0x11;
  result[1] = src.size() & 0xFF;
  result[2] = (src.size() >> 8) & 0xFF;
  result[3] = (src.size() >> 16) & 0xFF;

  GroupWriter writer{result};

  // Let zlib do the heavy lifting.
  std::array<u8, 8> dummy{};
  size_t dummy_size = dummy.size();
  const int ret = zng_compress2(
      dummy.data(), &dummy_size, src.data(), src.size(), std::clamp<int>(level, 6, 9),
      [](void* w, u32 dist, u32 lc) { static_cast<GroupWriter*>(w)->HandleZlibMatch(dist, lc); },
      &writer);
  if (ret != Z_OK)
    throw std::runtime_error("zng_compress failed");

  writer.Finalise();
  return result;
}

std::vector<u8> Decompress(tcb::span<const u8> src) {
  const auto uncompressed_size = GetUncompressedFilesize(src);
  if (!uncompressed_size)
    return {};
  std::vector<u8> result(uncompressed_size.value());
  Decompress(src, result);
  return result;
}

template <bool Safe>
static void Decompress(tcb::span<const u8> src, tcb::span<u8> dst) {
  common::BinaryReader reader{src, common::Endianness::Big};
  if (dst.size() > 0xFFFFFF) {
    reader.Seek(8);
  } else {
    reader.Seek(4);
  }

  u8 group_header = 0;
  size_t remaining_chunks = 0;
  for (auto dst_it = dst.begin(); dst_it < dst.end();) {
    if (remaining_chunks == 0) {
      group_header = reader.Read<u8, Safe>().value();
      remaining_chunks = ChunksPerGroup;
    }

    if (!(group_header & 0x80)) {
      *dst_it++ = reader.Read<u8, Safe>().value();
    } else {
      const u16 pair = reader.Read<u16, Safe>().value();
      size_t distance, length;
      // check for 4 indication bits
      switch (pair & 0xF000) {
      case 0:
        // uses 3 bytes
        length = (pair >> 4) + 0x11;
        distance = ((pair & 0xF) << 8 | reader.Read<u8, Safe>().value()) + 1;
        break;
      case 0x1000:
        // uses 4 bytes
        {
          const u16 ext_pair = reader.Read<u16, Safe>().value();
          length = ((pair & 0xFFF) << 4 | ext_pair >> 12) + 0x111;
          distance = (ext_pair & 0xFFF) + 1;
        }
        break;
      default:
        // uses 2 bytes
        length = (pair >> 12) + 1;
        distance = (pair & 0xFFF) + 1;
      }

      // printf("length: %d, distance: %d\n", length, distance);

      const u8* base = dst_it - distance;
      if (base < dst.begin() || dst_it + length > dst.end()) {
        throw std::invalid_argument("Copy is out of bounds");
      }
#pragma GCC unroll 0
      for (size_t i = 0; i < length; ++i)
        *dst_it++ = base[i];
    }

    group_header <<= 1;
    remaining_chunks -= 1;
  }
}

void Decompress(tcb::span<const u8> src, tcb::span<u8> dst) {
  Decompress<true>(src, dst);
}

void DecompressUnsafe(tcb::span<const u8> src, tcb::span<u8> dst) {
  Decompress<false>(src, dst);
}
}  // namespace nlzss11
