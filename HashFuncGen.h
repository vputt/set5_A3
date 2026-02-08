#pragma once
#include <cstdint>
#include <string>


class HashFuncGen {
public:
  HashFuncGen();

  uint32_t operator()(const std::string &s) const;
};
