#include "RandomStreamGen.h"

RandomStreamGen::RandomStreamGen(size_t n, uint64_t seed) : rng(seed) {
  data.reserve(n);
  for (size_t i = 0; i < n; ++i) {
    data.push_back(genString());
  }
}

std::string RandomStreamGen::genString() {
  static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz"
                                 "0123456789-";
  static const size_t ALPH_LEN = sizeof(alphabet) - 1;

  std::uniform_int_distribution<int> len_dist(1, 30);
  std::uniform_int_distribution<int> char_dist(0, (int)ALPH_LEN - 1);

  int len = len_dist(rng);
  std::string s;
  s.reserve(len);
  for (int i = 0; i < len; ++i) {
    s.push_back(alphabet[char_dist(rng)]);
  }
  return s;
}

std::vector<std::string> RandomStreamGen::prefixByRatio(double ratio) const {
  if (ratio <= 0.0)
    return {};
  if (ratio >= 1.0)
    return data;
  size_t k = static_cast<size_t>(ratio * data.size());
  if (k == 0)
    k = 1;
  return prefixByCount(k);
}

std::vector<std::string> RandomStreamGen::prefixByCount(size_t k) const {
  if (k > data.size())
    k = data.size();
  return std::vector<std::string>(data.begin(), data.begin() + k);
}
