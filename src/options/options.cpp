#include "options.hpp"

#include <fmt/format.h>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>
#include <string>

namespace options {

namespace {
namespace po = boost::program_options;

po::options_description BuildHiddenOptions(const char* desc_from, const char* desc_to) {
  po::options_description hidden("Hidden options");
  hidden.add_options()
      (kFrom.c_str(), po::value<std::string>()->required(), desc_from /**/)
      (kTo.c_str(), po::value<std::string>()->required(), desc_to/**/);

  return hidden;
}

auto ConfigureOptions(bool is_backup)
    -> std::tuple<po::options_description, po::options_description, po::positional_options_description> {
  po::options_description common("Allowed options");
  po::options_description hidden;
  if (is_backup) {
    common.add_options()
        (kHelp.c_str(), "get help message")
        (fmt::format("{},f", kFull).c_str(), "produce full backup")
        (fmt::format("{},i", kIncrement).c_str(), "produce incremental backup");

    hidden.add(BuildHiddenOptions("directory to make backup of", "directory to store backup to"));
  } else {
    common.add_options()
      (kHelp.c_str(), "Usage: ./my_restore <backup-dir> <work-dir>\nExample: ./my_restore backup/2024-01-01_00-00-00 /work");
    hidden.add(BuildHiddenOptions("directory to obtain backup from", "directory to restore backup to"));
  }
  po::options_description cmd_options;
  cmd_options.add(common).add(hidden);

  po::positional_options_description pos_opts;
  pos_opts.add(kFrom.c_str(), 1);
  pos_opts.add(kTo.c_str(), 2);

  return {std::move(cmd_options), std::move(common), std::move(pos_opts)};
}
} // namespace

po::options_description ParseCmdOptions(int argc, char* argv[], po::variables_map& opt_map, bool is_backup) {
  auto [cmd_options, common, pos_opts] = ConfigureOptions(is_backup);
  po::store(po::command_line_parser(argc, argv)
                .options(cmd_options)
                .positional(pos_opts)
                .run(),
            opt_map);
  return common;
}

} // namespace options