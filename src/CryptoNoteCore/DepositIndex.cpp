// Copyright (c) 2019-2021 Fango Developers
// Copyright (c) 2018-2021 Fandom Gold Society
// Copyright (c) 2018-2019 Conceal Network & Conceal Devs
// Copyright (c) 2014-2016 The XDN developers
// Copyright (c) 2012-2018 The CryptoNote developers
//
// This file is part of Fango.
//
// Fango is free software distributed in the hope that it
// will be useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE. You can redistribute it and/or modify it under the terms
// of the GNU General Public License v3 or later versions as published
// by the Free Software Foundation. Fango includes elements written 
// by third parties. See file labeled LICENSE for more details.
// You should have received a copy of the GNU General Public License
// along with Fango. If not, see <https://www.gnu.org/licenses/>.

#include <CryptoNoteCore/DepositIndex.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <limits>

#include "CryptoNoteSerialization.h"
#include "Serialization/SerializationOverloads.h"

namespace CryptoNote {

DepositIndex::DepositIndex() : blockCount(0) {
}

DepositIndex::DepositIndex(DepositHeight expectedHeight) : blockCount(0) {
  index.reserve(expectedHeight + 1);
}

void DepositIndex::reserve(DepositHeight expectedHeight) {
  index.reserve(expectedHeight + 1);
}

auto DepositIndex::fullDepositAmount() const -> DepositAmount {
  return index.empty() ? 0 : index.back().amount;
}

auto DepositIndex::fullInterestAmount() const -> DepositInterest {
  return index.empty() ? 0 : index.back().interest;
}

static inline bool sumWillOverflow(int64_t x, int64_t y) {
  if (y > 0 && x > std::numeric_limits<int64_t>::max() - y) {
    return true;
  }

  if (y < 0 && x < std::numeric_limits<int64_t>::min() - y) {
    return true;
  }
  
  return false;
}

static inline bool sumWillOverflow(uint64_t x, uint64_t y) {
  if (x > std::numeric_limits<uint64_t>::max() - y) {
    return true;
  }
 
  return false;
}

void DepositIndex::pushBlock(DepositAmount amount, DepositInterest interest) {
  DepositAmount lastAmount;
  DepositInterest lastInterest;
  if (index.empty()) {
    lastAmount = 0;
    lastInterest = 0;
  } else {
    lastAmount = index.back().amount;
    lastInterest = index.back().interest;
  }

  assert(!sumWillOverflow(amount, lastAmount));
  assert(!sumWillOverflow(interest, lastInterest));
  assert(amount + lastAmount >= 0);
  if (amount != 0) {
    index.push_back({blockCount, amount + lastAmount, interest + lastInterest});
  }

  ++blockCount;
}

void DepositIndex::popBlock() {
  assert(blockCount > 0);
  --blockCount;
  if (!index.empty() && index.back().height == blockCount) {
    index.pop_back();
  }
}
  
auto DepositIndex::size() const -> DepositHeight {
  return blockCount;
}

auto DepositIndex::upperBound(DepositHeight height) const -> IndexType::const_iterator {
  return std::upper_bound(
      index.cbegin(), index.cend(), height,
      [] (DepositHeight height, const DepositIndexEntry& left) { return height < left.height; });
}

size_t DepositIndex::popBlocks(DepositHeight from) {
  if (from >= blockCount) {
    return 0;
  }

  IndexType::iterator it = index.begin();
  std::advance(it, std::distance(index.cbegin(), upperBound(from)));
  if (it != index.begin()) {
    --it;
    if (it->height != from) {
      ++it;
    }
  }

  index.erase(it, index.end());
  auto diff = blockCount - from;
  blockCount -= diff;
  return diff;
}

auto DepositIndex::depositAmountAtHeight(DepositHeight height) const -> DepositAmount {
  if (blockCount == 0) {
    return 0;
  } else {
    auto it = upperBound(height);
    return it == index.cbegin() ? 0 : (--it)->amount;
  }
}

auto DepositIndex::depositInterestAtHeight(DepositHeight height) const -> DepositInterest {
  if (blockCount == 0) {
    return 0;
  } else {
    auto it = upperBound(height);
    return it == index.cbegin() ? 0 : (--it)->interest;
  }
}

void DepositIndex::serialize(ISerializer& s) {
  s(blockCount, "blockCount");
  if (s.type() == ISerializer::INPUT) {
    readSequence<DepositIndexEntry>(std::back_inserter(index), "index", s);
  } else {
    writeSequence<DepositIndexEntry>(index.begin(), index.end(), "index", s);
  }
}

void DepositIndex::DepositIndexEntry::serialize(ISerializer& s) {
  s(height, "height");
  s(amount, "amount");
  s(interest, "interest");
}

}
