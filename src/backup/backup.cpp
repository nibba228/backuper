#include "backup.hpp"

#include "../util/color.hpp"

#include <fmt/chrono.h>
#include <fmt/color.h>

#include <boost/filesystem.hpp>
#include <boost/system/errc.hpp>


namespace backup {

namespace fs = boost::filesystem;
namespace system = boost::system;

namespace {

void CreateSubdirForBackup(fs::path& to, system::error_code& error) {
  std::time_t time = std::time(nullptr);
  std::string now = fmt::format("{:%Y-%m-%d_%H-%M-%S}", fmt::localtime(time));
  to /= fmt::format("/{}", now);

  bool exists = fs::exists(to, error);
  const auto& str_path = to.generic_string();

  if (error && error.value() != sys::errc::no_such_file_or_directory) {
    fmt::print(util::color::kBoldRed, "Error while checking existance of dir {}\n", str_path);
    return;
  }

  if (!exists) {
    fs::create_directories(to, error);
    if (error) {
      fmt::print(util::color::kBoldRed, "Error while creating dir {}\n", str_path);
    }
  }
}

void CopyFromTo(const fs::path& from, const fs::path& to, system::error_code& error) {
  // TODO: deal with symlinks
  // TODO: configure attempts for files creation
  fs::copy(from, to, fs::copy_options::recursive, error);
  if (error) {
    fmt::print(util::color::kBoldRed, "Error while copying dir {} to {}\n", from.generic_string(), to.generic_string());
    return;
  }
}

} // namespace

void PerformFullBackup(fs::path from, fs::path to, system::error_code& error) {
  CreateSubdirForBackup(to, error);
  if (error) {
    return;
  }

  CopyFromTo(from, to, error);
}

void PerformIncrementalBackup(fs::path from, fs::path to, system::error_code& error) {
  // TODO
}

} // namespace backup