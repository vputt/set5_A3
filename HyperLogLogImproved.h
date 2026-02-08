#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "HashFuncGen.h"

// Улучшенный HLL:
// 1) LinearCounting
// 2) large-range correction (для 32-bit hash)
// 3) 6 бит на регистр
class HyperLogLogImproved {
public:
  explicit HyperLogLogImproved(uint8_t B);

  void add(uint32_t x);
  void add(const std::string& s, const HashFuncGen& h);

  // Возвращает скорректированную оценку E*
  double estimate() const;

  void reset();

  uint32_t getM() const { return m; }
  uint8_t  getB() const { return B; }

  // Фактическое число байт под packed-регистры
  size_t registersBytes() const { return words.size() * sizeof(uint64_t); }

private:
  uint8_t B;
  uint32_t m;

  static constexpr uint8_t BITS_PER_REG = 6; // хранит значения 0..63
  std::vector<uint64_t> words;
  uint64_t mask;

  double alpha_m() const;

  uint8_t getReg(uint32_t idx) const;
  void setReg(uint32_t idx, uint8_t val);

  static uint8_t leadingZeros(uint32_t x, uint8_t maxBits);
};
