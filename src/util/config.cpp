#include "config.hpp"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace util::config {

namespace fs = boost::filesystem;

namespace {
const std::string kConfigFile = ".latest_full_backup_path";
} // namespace

fs::path ReadFromConfig(const fs::path& where) {
  std::string out;
  fs::ifstream file{where / fs::path{kConfigFile}};
  file >> out;

  return {std::move(out)};
}

void WriteToConfig(const fs::path& where, std::string msg) {
  fs::ofstream file{where / fs::path{kConfigFile}};
  file << std::move(msg);
}

} // namespace util::config