#pragma once
#include <string>
#include <unordered_set>
#include <vector>


inline size_t exactDistinctCount(const std::vector<std::string> &part) {
  std::unordered_set<std::string> s(part.begin(), part.end());
  return s.size();
}
