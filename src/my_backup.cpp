#include "options/options.hpp"

#include <boost/program_options/variables_map.hpp>

int main(int argc, char* argv[]) {
  boost::program_options::variables_map opt_map;
  options::ParseCmdOptions(argc, argv, opt_map);
}