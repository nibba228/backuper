#pragma once

#include <boost/program_options.hpp>
#include <boost/program_options/variables_map.hpp>

namespace options {

const std::string kFrom = "from";
const std::string kFull = "full";
const std::string kHelp = "help";
const std::string kIncrement = "increment";
const std::string kTo = "to";

namespace {
  namespace po = boost::program_options;
}

po::options_description ParseCmdOptions(int argc, char* argv[],
                     po::variables_map& opt_map, bool is_backup);

} // namespace options