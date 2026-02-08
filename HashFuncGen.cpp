#include "HashFuncGen.h"

HashFuncGen::HashFuncGen() {}

uint32_t HashFuncGen::operator()(const std::string &s) const {
  uint32_t h = 2166136261u;
  for (unsigned char c : s) {
    h ^= c;
    h *= 16777619u;
  }
  return h;
}
