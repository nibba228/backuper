#include "backup/backup.hpp"
#include "options/options.hpp"
#include "util/color.hpp"

#include <fmt/color.h>

#include <boost/program_options/variables_map.hpp>
#include <boost/filesystem.hpp>
#include <boost/system/errc.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* argv[]) {
  po::variables_map opt_map;
  options::ParseCmdOptions(argc, argv, opt_map);

  po::notify(opt_map);

  const bool kIsFull = opt_map.count(options::kFull) == 1;
  const bool kIsIncrement = opt_map.count(options::kIncrement) == 1;
  std::string from = opt_map[options::kFrom].as<std::string>();
  std::string to = opt_map[options::kTo].as<std::string>();

  boost::system::error_code error;
  if (kIsFull == kIsIncrement) {
    if (kIsFull) {
      fmt::print(util::color::kBoldRed, "You should use --full or --increment, not both\n");
      return 1;
    }

    fmt::print(fmt::fg(fmt::color::sky_blue), "Performing full backup due to unspecified options\n");
    backup::PerformFullBackup(std::move(from), std::move(to), error);
  } else if (kIsFull) {
    backup::PerformFullBackup(std::move(from), std::move(to), error);
  } else {
    backup::PerformIncrementalBackup(std::move(from), std::move(to), error);
  }

  if (error) {
    fmt::print(util::color::kBoldRed, "Error: {}", error.message());
    return 1;
  }

  return 0;
}