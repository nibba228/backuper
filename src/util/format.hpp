#pragma once

#include <fmt/color.h>

namespace util::format {

template <typename... T>
void PrintError(std::string msg, T&&... args) {
  fmt::print(fmt::fg(fmt::color::red) | fmt::emphasis::bold, std::move(msg),
             std::forward<T>(args)...);
}

} // namespace util::format