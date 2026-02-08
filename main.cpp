#include "ExactDistinct.h"
#include "HashFuncGen.h"
#include "HyperLogLog.h"
#include "RandomStreamGen.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>


struct MeanStd {
  double mean;
  double std;
};

MeanStd computeMeanStd(const std::vector<double> &v) {
  double sum = 0.0;
  for (double x : v)
    sum += x;
  double mean = sum / v.size();
  double var = 0.0;
  for (double x : v) {
    double d = x - mean;
    var += d * d;
  }
  var /= v.size();
  return {mean, std::sqrt(var)};
}

int main() {
  uint8_t B = 10; // m = 2^10 = 1024, теор. ошибка ≈ 1.04/sqrt(1024) ~ 3.25%
  size_t numStreams = 5;
  size_t streamSize = 100000;

  HashFuncGen h;

  std::vector<double> ratios;
  for (int p = 5; p <= 100; p += 5) {
    ratios.push_back(p / 100.0);
  }

  std::ofstream out1("results_streams.csv");
  out1 << "stream_id,ratio,t,F0,Nt\n";

  std::map<double, std::vector<double>> estimatesByRatio;
  std::map<double, std::vector<double>> exactByRatio;

  for (size_t sId = 0; sId < numStreams; ++sId) {
    std::cout << "Stream " << sId << "...\n";
    RandomStreamGen gen(streamSize, 1234 + sId);
    const auto &full = gen.getFullStream();

    for (double r : ratios) {
      size_t k = static_cast<size_t>(r * streamSize);
      if (k == 0)
        k = 1;
      std::vector<std::string> prefix(full.begin(), full.begin() + k);

      size_t F0 = exactDistinctCount(prefix);

      HyperLogLog hll(B);
      for (const auto &str : prefix) {
        hll.add(str, h);
      }
      double Nt = hll.estimate();

      out1 << sId << "," << r << "," << k << "," << F0 << "," << Nt << "\n";

      estimatesByRatio[r].push_back(Nt);
      exactByRatio[r].push_back(static_cast<double>(F0));
    }
  }

  // Статистики по Nt
  std::ofstream out2("stats_by_ratio.csv");
  out2 << "ratio,mean_Nt,std_Nt,mean_F0\n";

  for (auto &[r, vals] : estimatesByRatio) {
    auto ms = computeMeanStd(vals);
    double meanF0 = 0.0;
    const auto &vF0 = exactByRatio[r];
    for (double x : vF0)
      meanF0 += x;
    meanF0 /= vF0.size();
    out2 << r << "," << ms.mean << "," << ms.std << "," << meanF0 << "\n";
  }

  std::cout << "Done. CSV: results_streams.csv, stats_by_ratio.csv\n";
  return 0;
}
