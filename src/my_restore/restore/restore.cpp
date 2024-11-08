#include "restore.hpp"
#include "../../util/backup/full_backup.hpp"
#include "../../util/filesystem/copy.hpp"
#include "../../util/format.hpp"

#include <boost/filesystem/directory.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/detail/errc.hpp>
#include <boost/system/detail/error_code.hpp>
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

void CopyNonExistingDirsAndFilesFromFullBackup(const fs::path& latest_full_backup, const fs::path& to, system::error_code& error) {
  for (const auto& entry : fs::directory_iterator{latest_full_backup, error}) {
    if (fs::is_regular_file(entry, error)) {
      fs::copy_file(entry, to / entry.path().filename(), fs::copy_options::skip_existing, error);
      if (error) {
        util::format::PrintError("Error while copying {} to {}\n", entry.path().generic_string(), to.generic_string());
        break;
      }
    } else if (!error && fs::is_directory(entry, error)) {
      auto dest = to / entry.path().filename();
      if (!fs::exists(dest, error) && error.value() == system::errc::no_such_file_or_directory) {
        error.clear();
        fs::copy(entry, dest, fs::copy_options::recursive, error);
        if (error) {
          util::format::PrintError("Error while copying dir {} to {}\n", entry.path().generic_string(), dest.generic_string());
          break;
        }
      } else if (error) {
        util::format::PrintError("Error while checking {} existence\n", dest.generic_string());
        break;
      }
    } else if (error) {
      util::format::PrintError("Error while checking {} file type\n", entry.path().generic_string());
      break;
    }
  }

  if (error) {
    util::format::PrintError("Error while iterating through dir {}\n", latest_full_backup.generic_string());
  }
}

void CopyFromTo(const fs::path& latest_full_backup, const fs::path& from, const fs::path& to, system::error_code& error) {
  util::filesystem::CopyFromTo(from, to, error, kDefaultOptions);
  if (error) {
    return;
  }

  CopyNonExistingDirsAndFilesFromFullBackup(latest_full_backup, to, error);
}

void RestoreImpl(fs::path from, const fs::path& to, system::error_code& error) {
  bool fast_path_performed = RestoreFastPath(from, to, error);
  if (error || fast_path_performed) {
    return;
  }
 
  auto parent = from;
  parent = parent.remove_filename().parent_path();
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