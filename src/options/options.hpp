#pragma once

#include <boost/program_options/variables_map.hpp>

namespace options {

void ParseCmdOptions(int argc, char* argv[], boost::program_options::variables_map& opt_map);

} // namespace options