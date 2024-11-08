#pragma once

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

namespace util::backup {

namespace {
namespace fs = boost::filesystem;
namespace system = boost::system;
} // namespace

std::pair<bool, fs::path> GetLatestFullBackup(const fs::path& where, system::error_code& error);

void UpdateLatestFullBackup(const fs::path& where, const std::string_view new_backup_folder);

bool CheckIsFullBackup(const fs::path& where, system::error_code& error);

void MarkAsFullBackup(const fs::path& where);

void UnmarkAsFullBackup(const fs::path& where, system::error_code& error);

} // namespace util::backup