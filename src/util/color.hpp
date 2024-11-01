#pragma once

#include <fmt/color.h>

namespace util::color {

const auto kBoldRed = fmt::fg(fmt::color::red) | fmt::emphasis::bold;

} // namespace util::color