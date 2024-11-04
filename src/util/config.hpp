#pragma once

#include <boost/filesystem.hpp>

namespace util::config {

namespace fs = boost::filesystem;

fs::path ReadFromConfig(const fs::path& where);

void WriteToConfig(const fs::path& where, std::string msg);

} // namespace util::config