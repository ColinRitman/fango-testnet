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

#include <random>

namespace Randomize
{
    /* Used to obtain a random seed */
    static thread_local std::random_device device;

    /* Generator, seeded with the random device */
    static thread_local std::mt19937 gen(device());

    /* The distribution to get numbers for - in this case, uint8_t */
    static std::uniform_int_distribution<int> distribution{0, std::numeric_limits<uint8_t>::max()};

    /**
     * Generate n random bytes (uint8_t), and place them in *result. Result should be large
     * enough to contain the bytes.
     */
    inline void randomBytes(size_t n, uint8_t *result)
    {
        for (size_t i = 0; i < n; i++)
        {
            result[i] = distribution(gen);
        }
    }

    /**
     * Generate n random bytes (uint8_t), and return them in a vector.
     */
    inline std::vector<uint8_t> randomBytes(size_t n)
    {
        std::vector<uint8_t> result;

        result.reserve(n);

        for (size_t i = 0; i < n; i++)
        {
            result.push_back(distribution(gen));
        }

        return result;
    }

    /**
     * Generate a random value of the type specified, in the full range of the
     * type
     */
    template <typename T>
    T randomValue()
    {
        std::uniform_int_distribution<T> distribution{
            std::numeric_limits<T>::min(), std::numeric_limits<T>::max()
        };

        return distribution(gen);
    }

    /**
     * Generate a random value of the type specified, in the range [min, max]
     * Note that both min, and max, are included in the results. Therefore,
     * randomValue(0, 100), will generate numbers between 0 and 100.
     *
     * Note that min must be <= max, or undefined behaviour will occur.
     */
    template <typename T>
    T randomValue(T min, T max)
    {
        std::uniform_int_distribution<T> distribution{min, max};
        return distribution(gen);
    }

    /**
     * Obtain the generator used internally. Helpful for passing to functions
     * like std::shuffle.
     */
    inline std::mt19937 generator()
    {
        return gen;
    }
}
