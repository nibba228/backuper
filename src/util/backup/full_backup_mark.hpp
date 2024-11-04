#pragma once

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

namespace util::backup {

namespace {
namespace fs = boost::filesystem;
} // namespace

bool CheckIsFullBackup(const fs::path& where, boost::system::error_code& error);

void MarkAsFullBackup(const fs::path& where_to_put);

} // namespace util::backup