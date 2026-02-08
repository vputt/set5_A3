#include "HyperLogLogImproved.h"
#include <cmath>
#include <algorithm>

HyperLogLogImproved::HyperLogLogImproved(uint8_t B_) : B(B_) {
  m = 1u << B;
  mask = (BITS_PER_REG == 64) ? ~0ull : ((1ull << BITS_PER_REG) - 1ull);

  const uint64_t totalBits = static_cast<uint64_t>(m) * BITS_PER_REG;
  const uint64_t nWords = (totalBits + 63ull) / 64ull;
  words.assign(static_cast<size_t>(nWords), 0ull);
}

void HyperLogLogImproved::reset() {
  std::fill(words.begin(), words.end(), 0ull);
}

double HyperLogLogImproved::alpha_m() const {
  if (m == 16)  return 0.673;
  if (m == 32)  return 0.697;
  if (m == 64)  return 0.709;
  return 0.7213 / (1.0 + 1.079 / m);
}

uint8_t HyperLogLogImproved::getReg(uint32_t idx) const {
  const uint64_t bitPos = static_cast<uint64_t>(idx) * BITS_PER_REG;
  const uint64_t w = bitPos / 64ull;
  const uint32_t off = static_cast<uint32_t>(bitPos % 64ull);

  if (off + BITS_PER_REG <= 64) {
    return static_cast<uint8_t>((words[w] >> off) & mask);
  } else {
    // пересечение границы 64-bit слова
    const uint32_t lowBits = 64u - off;
    const uint32_t highBits = BITS_PER_REG - lowBits;

    const uint64_t lowPart = (words[w] >> off) & ((1ull << lowBits) - 1ull);
    const uint64_t highPart = (words[w + 1] & ((1ull << highBits) - 1ull)) << lowBits;
    return static_cast<uint8_t>((lowPart | highPart) & mask);
  }
}

void HyperLogLogImproved::setReg(uint32_t idx, uint8_t val) {
  val = static_cast<uint8_t>(val & static_cast<uint8_t>(mask));

  const uint64_t bitPos = static_cast<uint64_t>(idx) * BITS_PER_REG;
  const uint64_t w = bitPos / 64ull;
  const uint32_t off = static_cast<uint32_t>(bitPos % 64ull);

  if (off + BITS_PER_REG <= 64) {
    const uint64_t clearMask = ~(mask << off);
    words[w] = (words[w] & clearMask) | (static_cast<uint64_t>(val) << off);
  } else {
    const uint32_t lowBits = 64u - off;
    const uint32_t highBits = BITS_PER_REG - lowBits;

    const uint64_t lowMask = (1ull << lowBits) - 1ull;
    const uint64_t highMask = (1ull << highBits) - 1ull;

    // low part
    {
      const uint64_t clearMask = ~(lowMask << off);
      const uint64_t lowVal = static_cast<uint64_t>(val) & lowMask;
      words[w] = (words[w] & clearMask) | (lowVal << off);
    }
    // high part
    {
      const uint64_t clearMask = ~highMask;
      const uint64_t highVal = (static_cast<uint64_t>(val) >> lowBits) & highMask;
      words[w + 1] = (words[w + 1] & clearMask) | highVal;
    }
  }
}

uint8_t HyperLogLogImproved::leadingZeros(uint32_t x, uint8_t maxBits) {
  if (maxBits == 0) return 0;
  if (x == 0) return maxBits;

  uint8_t cnt = 0;
  // классический переносимый подсчёт: смотрим maxBits старших бит
  while (cnt < maxBits && (x & 0x80000000u) == 0) {
    ++cnt;
    x <<= 1;
  }
  return cnt;
}

void HyperLogLogImproved::add(uint32_t x) {
  const uint32_t idx = x >> (32 - B);
  const uint32_t w = x << B;
  const uint8_t maxBits = static_cast<uint8_t>(32 - B);

  const uint8_t rho = static_cast<uint8_t>(leadingZeros(w, maxBits) + 1);

  const uint8_t cur = getReg(idx);
  if (rho > cur) setReg(idx, rho);
}

void HyperLogLogImproved::add(const std::string& s, const HashFuncGen& h) {
  add(h(s));
}

double HyperLogLogImproved::estimate() const {
  const double alpha = alpha_m();

  double sum = 0.0;
  uint32_t V = 0; // число нулевых регистров

  for (uint32_t j = 0; j < m; ++j) {
    const uint8_t r = getReg(j);
    if (r == 0) ++V;
    sum += std::pow(2.0, -static_cast<int>(r));
  }

  const double Z = 1.0 / sum;
  const double E = alpha * static_cast<double>(m) * static_cast<double>(m) * Z;

  // если E <= 2.5m и V>0 -> LinearCounting(m,V)=m ln(m/V)
  if (E <= 2.5 * static_cast<double>(m)) {
    if (V != 0) {
      return static_cast<double>(m) * std::log(static_cast<double>(m) / static_cast<double>(V));
    }
    return E;
  }

  // Large range correction (для 32-bit hash): если E > 2^32/30 -> -2^32 ln(1 - E/2^32)
  const double TWO_32 = 4294967296.0; // 2^32
  if (E > (TWO_32 / 30.0)) {
    const double ratio = E / TWO_32;
    if (ratio >= 1.0) return TWO_32;
    return -TWO_32 * std::log(1.0 - ratio);
  }

  return E;
}
