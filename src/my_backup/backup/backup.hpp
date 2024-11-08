#pragma once

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

namespace backup {

namespace {
namespace fs = boost::filesystem;
namespace system = boost::system;
} // namespace

void PerformFullBackup(const fs::path& from, fs::path to, system::error_code& error);

void PerformIncrementalBackup(const fs::path& from, fs::path to, system::error_code& error);

} // namespace backup