#include "backup.hpp"

#include "../util/format.hpp"

#include <system_error>

#include <fmt/chrono.h>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

namespace backup {

namespace fs = boost::filesystem;
namespace system = boost::system;

namespace {

const int kNoSuchFile = static_cast<int>(system::errc::no_such_file_or_directory);

bool CheckIsDirectory(const fs::path& to, system::error_code& error) {
  bool is_dir = fs::is_directory(to, error);
  if (error && error.value() != kNoSuchFile) {
    util::format::PrintError("Error while checking that path {} is directory\n",
                             to.generic_string());
    return false;
  }

  if (error.value() == kNoSuchFile) {
    error.clear();
    return true;
  }

  return is_dir;
}

bool CheckExists(const fs::path& to, system::error_code& error) {
  bool exists = fs::exists(to, error);

  if (error && error.value() != kNoSuchFile) {
    util::format::PrintError("Error while checking existance of dir {}\n",
                             to.generic_string());
    return false;
  } else if (error.value() == kNoSuchFile) {
    error.clear();
    return false;
  }

  return exists;
}

void CreateDirs(const fs::path& to, system::error_code& error) {
  fs::create_directories(to, error);
  if (error) {
    util::format::PrintError("Error while creating dir {}\n", to.generic_string());
  }
}

void CreateSubdirForBackup(fs::path& to, system::error_code& error) {
  bool is_dir = CheckIsDirectory(to, error);
  if (error) {
    return;
  }

  if (!is_dir) {
    util::format::PrintError("Path {} is not a directory\n",
                             to.generic_string());
    return;
  }

  std::time_t time = std::time(nullptr);
  std::string now = fmt::format("{:%Y-%m-%d_%H-%M-%S}", fmt::localtime(time));
  to /= fmt::format("/{}", now);

  bool exists = CheckExists(to, error);
  if (error) {
    return;
  }

  if (exists) {
    fs::remove_all(to, error);
    if (error) {
      util::format::PrintError("Error while deleting duplicate dir {}\n",
                               to.generic_string());
      return;
    }
  }

  CreateDirs(to, error);
}

void CopyFromTo(const fs::path& from, const fs::path& to,
                system::error_code& error) {
  fs::copy(from, to, fs::copy_options::recursive, error);
  if (error) {
    util::format::PrintError("Error while copying dir {} to {}\n",
                             from.generic_string(), to.generic_string());
    return;
  }
}

bool PerformIncrementalBackupFastPath(fs::path from, fs::path to,
                                      system::error_code& error) {
  bool is_empty = fs::is_empty(to, error);
  if (error.value() == kNoSuchFile) {
    is_empty = true;
    error.clear();
  }

  if (!error && is_empty) {
    PerformFullBackup(std::move(from), std::move(to), error);
  } else if (error) {
    util::format::PrintError("Error while checking {} emptiness\n",
                             to.generic_string());
  }

  return !error && is_empty;
}

} // namespace

void PerformFullBackup(fs::path from, fs::path to, system::error_code& error) {
  CreateSubdirForBackup(to, error);
  if (error) {
    return;
  }

  CopyFromTo(from, to, error);
}

void PerformIncrementalBackup(fs::path from, fs::path to,
                              system::error_code& error) {
  bool is_fast_path_performed =
      PerformIncrementalBackupFastPath(from, to, error);
  if (error || is_fast_path_performed) {
    return;
  }

  fs::directory_entry latest_dir;
  std::time_t last_time{0};
  
  for (auto&& entry : fs::directory_iterator{to}) {
    const auto time = fs::last_write_time(entry.path(), error);
    if (error) {
      util::format::PrintError("Error while iterating through dir {}\n",
                               to.generic_string());
      return;
    }

    if (time > last_time) {
      last_time = time;
      latest_dir = std::move(entry);
    }
  }
}

} // namespace backup