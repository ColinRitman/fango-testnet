// Copyright (c) 2019-2021 Fango Developers
// Copyright (c) 2018-2021 Fandom Gold Society
// Copyright (c) 2018-2019 Conceal Network & Conceal Devs
// Copyright (c) 2016-2019 The Karbowanec developers
// Copyright (c) 2012-2018 The CryptoNote developers
//
// This file is part of Fango.
//
// Fango is free & open source software distributed in the hope 
// that it will be useful, but WITHOUT ANY WARRANTY; without even
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE. You may redistribute it and/or modify it under the terms
// of the GNU General Public License v3 or later versions as published
// by the Free Software Foundation. Fango includes elements written 
// by third parties. See file labeled LICENSE for more details.
// You should have received a copy of the GNU General Public License
// along with Fango. If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <limits>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

namespace Tools {

    template<typename OutputIt, typename T>
    typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, void>::type
    write_varint(OutputIt &&dest, T i) {
        while (i >= 0x80) {
            *dest++ = (static_cast<char>(i) & 0x7f) | 0x80;
            i >>= 7;
        }
        *dest++ = static_cast<char>(i);
    }

    template<typename t_type>
    std::string get_varint_data(const t_type& v)
    {
      std::stringstream ss;
      write_varint(std::ostreambuf_iterator<char>(ss), v);
      return ss.str();
    }

    template<int bits, typename InputIt, typename T>
    typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value && 0 <= bits && bits <= std::numeric_limits<T>::digits, int>::type
    read_varint(InputIt &&first, InputIt &&last, T &i) {
        int read = 0;
        i = 0;
        for (int shift = 0;; shift += 7) {
            if (first == last) {
                return read; // End of input.
            }
            unsigned char byte = *first++;
            ++read;
            if (shift + 7 >= bits && byte >= 1 << (bits - shift)) {
                return -1; // Overflow.
            }
            if (byte == 0 && shift != 0) {
                return -2; // Non-canonical representation.
            }
            i |= static_cast<T>(byte & 0x7f) << shift;
            if ((byte & 0x80) == 0) {
                break;
            }
        }
        return read;
    }

    template<typename InputIt, typename T>
    int read_varint(InputIt &&first, InputIt &&last, T &i) {
        return read_varint<std::numeric_limits<T>::digits, InputIt, T>(std::move(first), std::move(last), i);
    }
}
