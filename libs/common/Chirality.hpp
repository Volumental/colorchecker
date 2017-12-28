#pragma once

#include "Logging.hpp"

namespace komb {

using SideType = int;
const SideType kLeft     = 0;
const SideType kRight    = 1;
const SideType kNumSides = 2;

inline const char* sideName(SideType side_idx)
{
    CHECK(side_idx == kLeft || side_idx == kRight);
    return side_idx == kLeft ? "left" : "right";
};

} // namespace komb
