// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Patryk Strzelecki <patryk@strzelecki.info>
#pragma once

#include <stdint.h>

// Status LED colours (0xRRGGBB) that mean the same in every IP9 app.
namespace ip9::led {

constexpr uint32_t kSuccess = 0x00FF30;  // two green blinks: done, connected, saved
constexpr uint32_t kError = 0xFF0000;    // red blinks: something went wrong or needs doing first

}  // namespace ip9::led
