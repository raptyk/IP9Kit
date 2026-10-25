// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Patryk Strzelecki <patryk@strzelecki.info>
#include "ip9/ui/Theme.h"

namespace ip9::theme {

namespace {
uint16_t accentColour = kBrandAccent;
}  // namespace

uint16_t accent() {
    return accentColour;
}

void setAccent(uint16_t colour) {
    accentColour = colour;
}

}  // namespace ip9::theme
