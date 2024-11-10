#include "restore.hpp"
#include "../../util/backup/full_backup.hpp"
#include "../../util/filesystem/copy.hpp"
#include "../../util/format.hpp"

#include <boost/filesystem/operations.hpp>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

namespace restore {

namespace {
namespace fs = boost::filesystem;
namespace system = boost::system;

const auto kDefaultOptions = fs::copy_options::recursive | fs::copy_options::overwrite_existing;

void DeleteMetadata(const fs::path& to, system::error_code& error) {
  util::backup::UnmarkAsFullBackup(to, error);
}

bool RestoreFastPath(const fs::path& from, const fs::path& to, system::error_code& error) {
  bool is_full_backup = util::backup::CheckIsFullBackup(from, error);
  if (is_full_backup) {
    util::filesystem::CopyFromTo(from, to, error, kDefaultOptions);
    if (!error) {
      DeleteMetadata(to, error);
    }
  }
  return !error && is_full_backup;
}

fs::path GetLatestFullBackup(const fs::path& from, const fs::path& parent_from, fs::path globally_latest_fb, system::error_code& error) {
  fs::path latest_full_backup;
  if (globally_latest_fb < from) {
    latest_full_backup = std::move(globally_latest_fb);
  } else {
    for (const auto& entry : fs::directory_iterator{parent_from, error}) {
      if (latest_full_backup < entry && entry < from && util::backup::CheckIsFullBackup(entry, error)) {
        latest_full_backup = entry;
      }
      if (error) {
        break;
      }
    }
  }

  if (error) {
    util::format::PrintError("Error while iterating through {}\n", parent_from.generic_string());
    latest_full_backup.clear();
  }

  return latest_full_backup;
}

void CopyFromToOverwriteDirs(const fs::path& from, const fs::path& to, fs::copy_options options, system::error_code& error) {
  for (const auto& entry : fs::directory_iterator{from, error}) {
    if (fs::is_regular_file(entry, error)) {
      const auto file_options = fs::copy_options::skip_existing | fs::copy_options::update_existing | fs::copy_options::overwrite_existing;
      fs::copy_file(entry, to / entry.path().filename(), options & file_options, error);
      if (error) {
        util::format::PrintError("Error while copying {} to {}\n", entry.path().generic_string(), to.generic_string());
        break;
      }
    } else if (!error && fs::is_directory(entry, error)) {
      auto dest = to / entry.path().filename();
      if (!fs::exists(dest, error) && error.value() == system::errc::no_such_file_or_directory) {
        error.clear();
        util::filesystem::CopyFromTo(entry, dest, error, options & fs::copy_options::recursive);
        if (error) {
          break;
        }
      } else if (error) {
        util::format::PrintError("Error while checking {} existence\n", dest.generic_string());
        break;
      }

      fs::remove_all(dest, error);
      if (error) {
        util::format::PrintError("Error while removing dir {}\n", dest.generic_string());
        break;
      }
      util::filesystem::CopyFromTo(entry, dest, error, options & fs::copy_options::recursive);
      if (error) {
        break;
      }
    } else if (error) {
      util::format::PrintError("Error while checking {} file type\n", entry.path().generic_string());
      break;
    }
  }

  if (error) {
    util::format::PrintError("Error while iterating through dir {}\n", from.generic_string());
  }
}

void CopyFromTo(const fs::path& latest_full_backup, const fs::path& from, const fs::path& to, system::error_code& error) {
  CopyFromToOverwriteDirs(latest_full_backup, to, kDefaultOptions, error);
  if (error) {
    return;
  }
  CopyFromToOverwriteDirs(from, to, kDefaultOptions, error);
}

void RestoreImpl(fs::path from, const fs::path& to, system::error_code& error) {
  bool fast_path_performed = RestoreFastPath(from, to, error);
  if (error || fast_path_performed) {
    return;
  }
 
  auto parent = from;
  while (parent.generic_string().back() == fs::path::separator) {
    parent.remove_trailing_separator();
  }
  parent = parent.parent_path();
  auto [has_full_backup, globally_latest_fb] = util::backup::GetLatestFullBackup(parent, error);
  if (!has_full_backup) {
    throw std::logic_error{"The provided backup directory is not a backup directory actually\n"};
  }
  
  auto latest_full_backup = GetLatestFullBackup(from, parent, std::move(globally_latest_fb), error);
  if (error) {
    return;
  }

  CopyFromTo(latest_full_backup, from, to, error);
  if (error) {
    return;
  }
  DeleteMetadata(to, error);
}

} // namespace

void Restore(fs::path from, const fs::path& to, system::error_code& error) {
  RestoreImpl(std::move(from), to, error);
}

} // namespace restore