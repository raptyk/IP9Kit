// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Patryk Strzelecki <patryk@strzelecki.info>
#pragma once

#include <stdint.h>

// GPIOs that are the same on the Cardputer and the Cardputer ADV.
namespace ip9::pins {

constexpr uint8_t kIrTx = 44;

// microSD slot (FSPI).
constexpr int8_t kSdSck = 40;
constexpr int8_t kSdMiso = 39;
constexpr int8_t kSdMosi = 14;
constexpr int8_t kSdCs = 12;

}  // namespace ip9::pins
