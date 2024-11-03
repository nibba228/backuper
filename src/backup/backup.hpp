#pragma once

#include <system_error>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

namespace backup {

namespace fs = boost::filesystem;
namespace system = boost::system;

void PerformFullBackup(fs::path from, fs::path to, system::error_code& error);

void PerformIncrementalBackup(fs::path from, fs::path to, system::error_code& error);

} // namespace backup