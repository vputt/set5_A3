#pragma once
#include <random>
#include <string>
#include <vector>


class RandomStreamGen {
public:
  RandomStreamGen(size_t n, uint64_t seed = 0);

  const std::vector<std::string> &getFullStream() const { return data; }

  // Префикс длиной ratio * n (ratio в (0,1])
  std::vector<std::string> prefixByRatio(double ratio) const;

  // Префикс по количеству элементов
  std::vector<std::string> prefixByCount(size_t k) const;

private:
  std::vector<std::string> data;
  std::mt19937_64 rng;

  std::string genString();
};