#include "full_backup_mark.hpp"
#include "../format.hpp"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/system/error_code.hpp>

namespace util::backup {

namespace {
namespace fs = boost::filesystem;

const fs::path kFileToPut{".full_backup"};
} // namespace

bool CheckIsFullBackup(const fs::path& where, boost::system::error_code& error) {
  auto path = where / kFileToPut;
  bool exists = fs::exists(path, error);
  if (error && error != boost::system::errc::no_such_file_or_directory) {
    util::format::PrintError("Error while checking {} existance\n", path.generic_string());
    return false;
  }
  error.clear();
  return exists;
}

// Creates an empty file in a backup folder. Since then it means that
// the folder is the folder of a full backup
void MarkAsFullBackup(const fs::path& where_to_put) {
  fs::ofstream file{where_to_put / kFileToPut};
}

} // namespace util::backup