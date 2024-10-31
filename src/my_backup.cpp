#include "options/options.hpp"

#include <fmt/color.h>

#include <boost/program_options/variables_map.hpp>
#include <boost/filesystem.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

void PerformFullBackup(const fs::path& from, const fs::path& to);

int main(int argc, char* argv[]) {
  po::variables_map opt_map;
  options::ParseCmdOptions(argc, argv, opt_map);

  po::notify(opt_map);

  const bool kIsFull = opt_map.count(options::kFull) == 1;
  const bool kIsIncrement = opt_map.count(options::kIncrement) == 0;

  if (kIsFull == kIsIncrement) {
    if (kIsFull) {
      fmt::print(fmt::fg(fmt::color::red) | fmt::emphasis::bold, "You should use --full or --increment, not both");
      return 1;
    }

    fmt::print(fmt::fg(fmt::color::sky_blue), "Performing full backup due to unspecified options");
    // PerformFullBackup(from, to);
  } else if (kIsFull) {
    // PerformFullBackup(from, to);
  } else {
    // PerformIncrementalBackup(from, to);
  }

  return 0;
}