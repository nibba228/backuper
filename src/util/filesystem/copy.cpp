#include "copy.hpp"
#include "../format.hpp"

#include <boost/filesystem.hpp>

namespace util::filesystem {

namespace {

namespace fs = boost::filesystem;
namespace system = boost::system;

} // namespace

void CopyFromTo(const fs::path& from, const fs::path& to,
                system::error_code& error, fs::copy_options options) {
  fs::copy(from, to, options, error);
  if (error) {
    util::format::PrintError("Error while copying dir {} to {}\n",
                             from.generic_string(), to.generic_string());
  }
}

} // namespace util::filesystem