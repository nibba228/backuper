#include "options.hpp"

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

namespace options {

namespace {
namespace po = boost::program_options;

auto ConfigureOptions()
    -> std::pair<po::options_description, po::positional_options_description> {
  po::options_description common("Allowed options");
  common.add_options()("help", "get help message"),
      ("full,f", po::value<bool>()->default_value(false),
       "produce full backup"),
      ("increment,i", po::value<bool>()->default_value(false),
       "produce incremental backup");

  po::options_description hidden("Hidden options");
  hidden.add_options()
      ("from", po::value<std::string>(), "directory to make backup of"),
      ("to", po::value<std::string>(), "directory to store backup to");

  po::options_description cmd_options;
  cmd_options.add(common).add(hidden);

  po::positional_options_description pos_opts;
  pos_opts.add("from", 1);
  pos_opts.add("to", 2);

  return {std::move(cmd_options), std::move(pos_opts)};
}
} // namespace

void ParseCmdOptions(int argc, char* argv[], po::variables_map& opt_map) {
  auto [cmd_options, pos_opts] = ConfigureOptions();
  po::store(po::command_line_parser(argc, argv)
                .options(cmd_options)
                .positional(pos_opts)
                .run(),
            opt_map);
}

} // namespace options