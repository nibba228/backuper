#include "../options/options.hpp"
#include "../util/format.hpp"
#include "restore/restore.hpp"

#include <iostream>

#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/system/error_code.hpp>
#include <stdexcept>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  po::variables_map opt_map;
  po::options_description help;

  try {
    const bool kIsBackup = false;
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

  std::string from = opt_map[options::kFrom].as<std::string>();
  std::string to = opt_map[options::kTo].as<std::string>();

  boost::system::error_code error;
  try {
    restore::Restore(std::move(from), to, error);
  } catch (const std::logic_error& e) {
    util::format::PrintError("{}\n", e.what());
    return 1;
  }

  if (error) {
    util::format::PrintError("{}\n", error.what());
    return 1;
  }
}