#pragma once

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

namespace restore {

namespace {
namespace fs = boost::filesystem;
} // namespace

void Restore(fs::path from, const fs::path& to, boost::system::error_code& error);

} // namespace restore