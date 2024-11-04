#include "backup.hpp"

#include "../util/config.hpp"
#include "../util/format.hpp"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include <fmt/chrono.h>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

namespace backup {

namespace fs = boost::filesystem;
namespace system = boost::system;

namespace {

const int kNoSuchFile =
    static_cast<int>(system::errc::no_such_file_or_directory);

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
    util::format::PrintError("Error while creating dir {}\n",
                             to.generic_string());
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

fs::path GetLatestWriteBackupDir(const fs::path& to,
                                 system::error_code& error) {
  return util::config::ReadFromConfig(to);
}

auto GetLatestWriteBackupDirTree(const fs::path& latest_backup,
                                 system::error_code& error) {
  std::unordered_map<std::string, std::unordered_set<std::string>>
      latest_backup_tree;
  const auto& str_backup_path = latest_backup.generic_string();
  const size_t kPathLen = latest_backup.size();
  for (const auto& entry :
       fs::recursive_directory_iterator{latest_backup, error}) {
    const auto& str_path = entry.path().generic_string();
    std::string key{std::ranges::mismatch(str_backup_path, str_path).in2,
                    str_path.begin() + str_path.rfind('/') + 1};

    latest_backup_tree[std::move(key)].emplace(str_path, kPathLen);
  }

  if (error) {
    util::format::PrintError("Error while iterating though dir {}",
                             str_backup_path);
  }

  return latest_backup_tree;
}

auto GetEntryAndBackupEntrySize(std::string_view entry,
                                std::string_view backup_entry,
                                system::error_code& error)
    -> std::pair<boost::uintmax_t, boost::uintmax_t> {
  auto entry_sz = fs::file_size(entry, error);
  if (error) {
    util::format::PrintError("Error while getting size of {}\n", entry);
    return {};
  }
  auto backup_entry_sz = fs::file_size(backup_entry, error);
  if (error) {
    util::format::PrintError("Error while getting size of {}\n", backup_entry);
    return {};
  }

  return {entry_sz, backup_entry_sz};
}

auto GetEntryAndBackupEntryWriteTime(
    std::string_view entry, std::string_view backup_entry,
    system::error_code& error) -> std::pair<std::time_t, std::time_t> {
  auto entry_time = fs::last_write_time(entry, error);
  if (error) {
    util::format::PrintError("Error while obtaining dir {} write time\n", entry);
    return {};
  }
  auto backup_entry_time = fs::last_write_time(backup_entry, error);
  if (error) {
    util::format::PrintError("Error while obtaining dir {} write time\n",
                             backup_entry);
    return {};
  }

  return {entry_time, backup_entry_time};
}

void PerformIncrementalCopy(std::string_view entry, std::string backup_entry,
                            const fs::file_status& entry_stat,
                            system::error_code& error) {
  fs::path dest{std::move(backup_entry)};
  if (entry_stat.type() != fs::file_type::directory_file) {
    dest.remove_filename();
  }

  CreateDirs(dest, error);
  if (error) {
    return;
  }
  CopyFromTo(entry, dest, error);
}

} // namespace

void PerformFullBackup(fs::path from, fs::path to, system::error_code& error) {
  CreateSubdirForBackup(to, error);
  if (error) {
    return;
  }

  CopyFromTo(from, to, error);
  util::config::WriteToConfig(to.parent_path(), to.generic_string());
}

void PerformIncrementalBackup(fs::path from, fs::path to,
                              system::error_code& error) {
  bool is_fast_path_performed =
      PerformIncrementalBackupFastPath(from, to, error);
  if (error || is_fast_path_performed) {
    return;
  }

  auto latest_backup = GetLatestWriteBackupDir(to, error);
  if (error) {
    return;
  }

  auto latest_backup_tree = GetLatestWriteBackupDirTree(latest_backup, error);
  if (error) {
    return;
  }

  bool is_now_dir_created = false;
  const size_t kFromLen = from.generic_string().size();
  for (auto it = fs::recursive_directory_iterator{from, error};
       it != fs::recursive_directory_iterator{}; ++it) {
    const auto& entry = it->path().generic_string();
    std::string key{std::ranges::mismatch(from.generic_string(), entry).in2,
                    entry.begin() + entry.rfind('/') + 1};

    auto entry_stat = fs::status(entry, error);
    if (error) {
      util::format::PrintError("Error while getting info on {}\n", entry);
      return;
    }

    std::string value(entry, kFromLen);
    if (latest_backup_tree[std::move(key)].contains(value)) {
      std::string backup_entry = latest_backup.generic_string() + value;
      auto backup_entry_stat = fs::status(backup_entry, error);
      if (error) {
        util::format::PrintError("Error while getting info on {}\n",
                                 backup_entry);
        return;
      }

      if (backup_entry_stat.type() == entry_stat.type()) {
        auto [entry_time, backup_entry_time] =
            GetEntryAndBackupEntryWriteTime(entry, backup_entry, error);
        if (error) {
          return;
        }
        if (entry_stat.type() == fs::file_type::directory_file &&
            entry_time <= backup_entry_time) {
          continue;
        } else if (entry_time <= backup_entry_time) {
          auto [entry_sz, backup_entry_sz] =
              GetEntryAndBackupEntrySize(entry, backup_entry, error);
          if (error) {
            return;
          }
          if (entry_sz == backup_entry_sz) {
            continue;
          }
        }
      }
    }

    if (!is_now_dir_created) {
      CreateSubdirForBackup(to, error);
      if (error) {
        return;
      }
      is_now_dir_created = true;
    }

    PerformIncrementalCopy(entry, to.generic_string() + value, entry_stat,
                           error);
    if (error) {
      return;
    }

    if (entry_stat.type() == fs::file_type::directory_file) {
      it.disable_recursion_pending();
    }
  }

  if (error) {
    util::format::PrintError("Error while iterating through dir {}\n",
                             from.generic_string());
    return;
  }
}

} // namespace backup