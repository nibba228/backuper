#include "backup.hpp"

#include "../../util/backup/full_backup.hpp"
#include "../../util/filesystem/copy.hpp"
#include "../../util/format.hpp"

#include <algorithm>
#include <fmt/color.h>
#include <unordered_map>
#include <unordered_set>

#include <fmt/chrono.h>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

namespace backup {

namespace {
namespace fs = boost::filesystem;
namespace system = boost::system;
using BackupTree =
    std::unordered_map<std::string, std::unordered_set<std::string>>;

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

bool PerformIncrementalBackupFastPath(const fs::path& from, fs::path to,
                                      system::error_code& error) {
  auto [has_full_backup, _] = util::backup::GetLatestFullBackup(to, error);

  if (!(error || has_full_backup)) {
    PerformFullBackup(from, std::move(to), error);
  } else if (error) {
    util::format::PrintError("Error while checking {} emptiness\n",
                             to.generic_string());
  }

  return !(error || has_full_backup);
}

BackupTree GetLatestFullBackupDirTree(const fs::path& latest_backup,
                                      system::error_code& error) {
  BackupTree latest_backup_tree;
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
    util::format::PrintError("Error while obtaining file {} write time\n",
                             entry);
    return {};
  }
  auto backup_entry_time = fs::last_write_time(backup_entry, error);
  if (error) {
    util::format::PrintError("Error while obtaining file {} write time\n",
                             backup_entry);
    return {};
  }

  return {entry_time, backup_entry_time};
}

void PerformIncrementalCopy(std::string_view entry, fs::path& to,
                            std::string value,
                            const fs::file_status& entry_stat,
                            bool& should_create_backup_dir,
                            system::error_code& error) {
  if (should_create_backup_dir) {
    CreateSubdirForBackup(to, error);
    if (error) {
      return;
    }
    should_create_backup_dir = false;
  }

  fs::path dest{to.generic_string() + std::move(value)};
  if (entry_stat.type() != fs::file_type::directory_file) {
    dest.remove_filename();
  }

  CreateDirs(dest, error);
  if (error) {
    return;
  }
  util::filesystem::CopyFromTo(entry, dest, error, fs::copy_options::recursive);
}

bool ShouldBackup(std::string_view entry, std::string_view backup_entry,
                  const fs::file_status& entry_stat,
                  const fs::file_status& backup_entry_stat,
                  system::error_code& error) {
  if (backup_entry_stat.type() == entry_stat.type()) {
    auto [entry_time, backup_entry_time] =
        GetEntryAndBackupEntryWriteTime(entry, backup_entry, error);
    if (error || (entry_stat.type() == fs::file_type::directory_file &&
                  entry_time <= backup_entry_time)) {
      return false;
    }

    if (entry_time <= backup_entry_time) {
      auto [entry_sz, backup_entry_sz] =
          GetEntryAndBackupEntrySize(entry, backup_entry, error);
      if (error || entry_sz == backup_entry_sz) {
        return false;
      }
    }
  }

  return true;
}

void ProcessEntries(const fs::path& from, fs::path& to,
                    const fs::path& latest_backup,
                    BackupTree& latest_backup_tree, system::error_code& error) {
  bool should_create_backup_dir = true;
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

      if (!ShouldBackup(entry, backup_entry, entry_stat, backup_entry_stat,
                        error)) {
        continue;
      }
      if (error) {
        return;
      }
    }

    PerformIncrementalCopy(entry, to, std::move(value), entry_stat,
                           should_create_backup_dir, error);
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
  } else if (should_create_backup_dir) {
    fmt::print(fmt::fg(fmt::color::sky_blue),
               "The diff to the latest full backup is empty. Backup is not "
               "created. To force its creation, provide -f flag instead of -i.\n");
  }
}

} // namespace

void PerformFullBackup(const fs::path& from, fs::path to, system::error_code& error) {
  CreateSubdirForBackup(to, error);
  if (error) {
    return;
  }

  util::filesystem::CopyFromTo(from, to, error, fs::copy_options::recursive);
  fs::path new_backup_folder = to.filename();
  util::backup::UpdateLatestFullBackup(to.parent_path(), to.generic_string());
  util::backup::MarkAsFullBackup(to);
}

void PerformIncrementalBackup(const fs::path& from, fs::path to,
                              system::error_code& error) {
  bool is_fast_path_performed =
      PerformIncrementalBackupFastPath(from, to, error);
  if (error || is_fast_path_performed) {
    return;
  }

  auto [_, latest_backup] = util::backup::GetLatestFullBackup(to, error);
  if (error) {
    return;
  }

  auto latest_backup_tree = GetLatestFullBackupDirTree(latest_backup, error);
  if (error) {
    return;
  }

  ProcessEntries(from, to, latest_backup, latest_backup_tree, error);
}

} // namespace backup