#include "HyperLogLog.h"
#include <cmath>
#include <algorithm>

// Нормировочная константа alpha_m
double HyperLogLog::alpha_m() const {
  if (m == 16)  return 0.673;
  if (m == 32)  return 0.697;
  if (m == 64)  return 0.709;
  return 0.7213 / (1.0 + 1.079 / m); // для m >= 128
}

HyperLogLog::HyperLogLog(uint8_t B_) : B(B_) {
  m = 1u << B;
  M.assign(m, 0);
}

void HyperLogLog::reset() {
  std::fill(M.begin(), M.end(), 0);
}

// Подсчёт количества ведущих нулей в старших maxBits битах числа x
static uint8_t leadingZeros(uint32_t x, uint8_t maxBits) {
    if (maxBits == 0) return 0;
    if (x == 0) return maxBits;

    uint8_t cnt = 0;
    while (cnt < maxBits && (x & 0x80000000u) == 0) {
        ++cnt;
        x <<= 1;
    }
    return cnt;
}

void HyperLogLog::add(uint32_t x) {
  // Индекс регистра B старших бит 32-битного хеша
  uint32_t idx = x >> (32 - B);

  // Хвост оставшиеся 32-B бит, сдвигаем влево
  uint32_t w = x << B;
  uint8_t maxBits = 32 - B;

  // rho = 1 + количество ведущих нулей в хвосте (на первых maxBits битах)
  uint8_t rho = leadingZeros(w, maxBits) + 1;

  if (rho > M[idx]) {
    M[idx] = rho;
  }
}

void HyperLogLog::add(const std::string& s, const HashFuncGen& h) {
  uint32_t x = h(s);
  add(x);
}

double HyperLogLog::estimate() const {
  double alpha = alpha_m();
  double sum = 0.0;
  for (uint32_t j = 0; j < m; ++j) {
    sum += std::pow(2.0, -static_cast<int>(M[j]));
  }
  double Z = 1.0 / sum;
  double E = alpha * m * m * Z;
  return E;
}
