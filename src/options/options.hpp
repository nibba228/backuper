#pragma once

#include <boost/program_options/variables_map.hpp>

namespace options {

constexpr std::string kFrom = "from";
constexpr std::string kFull = "full";
constexpr std::string kHelp = "help";
constexpr std::string kIncrement = "increment";
constexpr std::string kTo = "to";

void ParseCmdOptions(int argc, char* argv[],
                     boost::program_options::variables_map& opt_map);

} // namespace options