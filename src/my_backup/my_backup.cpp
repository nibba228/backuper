#include "backup/backup.hpp"
#include "../options/options.hpp"
#include "../util/format.hpp"

#include <iostream>

#include <fmt/color.h>

#include <boost/program_options/errors.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/system/error_code.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  po::variables_map opt_map;
  po::options_description help;
  try {
    const bool kIsBackup = true;
    help.add(options::ParseCmdOptions(argc, argv, opt_map, kIsBackup));
    po::notify(opt_map);
  } catch (const po::required_option& e) {
    if (opt_map.count(options::kHelp)) {
      std::cout << std::move(help);
      return 0;
    }
    util::format::PrintError("Error while parsing command: {}\n", e.what());
    return 1;
  } catch (const std::exception& e) {
    util::format::PrintError("Error while parsing command: {}\n", e.what());
    return 1;
  }

  const bool kIsFull = opt_map.count(options::kFull) == 1;
  const bool kIsIncrement = opt_map.count(options::kIncrement) == 1;
  std::string from = opt_map[options::kFrom].as<std::string>();
  std::string to = opt_map[options::kTo].as<std::string>();

  boost::system::error_code error;
  if (kIsFull == kIsIncrement) {
    if (kIsFull) {
      util::format::PrintError(
          "You should use --full or --increment, not both\n");
      return 1;
    }

    fmt::print(fmt::fg(fmt::color::sky_blue),
               "Performing full backup due to unspecified options\n");
    backup::PerformFullBackup(from, std::move(to), error);
  } else if (kIsFull) {
    backup::PerformFullBackup(from, std::move(to), error);
  } else {
    backup::PerformIncrementalBackup(from, std::move(to), error);
  }

  if (error) {
    util::format::PrintError("Error: {}\n", error.message());
    return 1;
  }

  return 0;
}