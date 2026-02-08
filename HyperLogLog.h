#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include "HashFuncGen.h"

class HyperLogLog {
public:
  explicit HyperLogLog(uint8_t B);

  void add(uint32_t x);
  void add(const std::string& s, const HashFuncGen& h);

  double estimate() const;

  void reset();

  uint32_t getM() const { return m; }
  uint8_t  getB() const { return B; }

private:
  uint8_t B;
  uint32_t m;
  std::vector<uint8_t> M;

  double alpha_m() const;
  static uint8_t leadingZeros(uint32_t x, uint8_t maxBits);
};
