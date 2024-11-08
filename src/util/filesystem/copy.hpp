#pragma once

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

namespace util::filesystem {

namespace {

namespace fs = boost::filesystem;
namespace system = boost::system;

} // namespace

void CopyFromTo(const fs::path& from, const fs::path& to,
                system::error_code& error, fs::copy_options options);

} // namespace util::filesystem