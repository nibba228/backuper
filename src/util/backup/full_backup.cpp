#include "full_backup.hpp"
#include "../format.hpp"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/system/error_code.hpp>

namespace util::backup {

namespace {
namespace fs = boost::filesystem;
namespace system = boost::system;

const fs::path kLatestFullBackupFile{".latest_full_backup"};
const fs::path kFullBackup{".full_backup"};

bool CheckFileExists(const fs::path& path, system::error_code& error) {
  bool exists = fs::exists(path, error);
  if (error && error != boost::system::errc::no_such_file_or_directory) {
    util::format::PrintError("Error while checking {} existence\n", path.generic_string());
    return false;
  }
  error.clear();

  return exists;
}

} // namespace

std::pair<bool, fs::path> GetLatestFullBackup(const fs::path& where, system::error_code& error) {
  fs::path path = where / kLatestFullBackupFile;

  bool exists = CheckFileExists(path, error);
  if (error || !exists) {
    return {false, {}};
  }

  fs::ifstream file{path};
  fs::path backup_dir;
  file >> backup_dir;

  return {true,std::move(backup_dir)};
}

void UpdateLatestFullBackup(const fs::path& where, const std::string_view new_backup_folder) {
  fs::ofstream file{where / kLatestFullBackupFile};
  file << new_backup_folder;
}

bool CheckIsFullBackup(const fs::path& where, system::error_code& error) {
  return CheckFileExists(where / kFullBackup, error);
}

// Creates an empty file in a full backup folder. Since then it means that
// the folder is the folder of a full backup
void MarkAsFullBackup(const fs::path& where) {
  fs::ofstream new_file{where / kFullBackup};
}

void UnmarkAsFullBackup(const fs::path& where, system::error_code& error) {
  auto path = where / kFullBackup;
  fs::remove(path, error);
  if (error) {
    util::format::PrintError("Error while deleting {}", path.generic_string());
  }
}

} // namespace util::backup