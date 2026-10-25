// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Patryk Strzelecki <patryk@strzelecki.info>
#pragma once

#include <M5GFX.h>

// Vector icons shared by all IP9 apps (home screen cards). Icons are drawn around (cx, cy)
// at 1.0 = about 40 px; `background` is the colour behind the icon, used for cut-outs.
namespace ip9::icons {

// Rounds `value * scale` to whole pixels.
int scaled(float value, float scale);

void drawSettings(M5Canvas& canvas, int cx, int cy, uint16_t colour, float scale, uint16_t background);
void drawHelp(M5Canvas& canvas, int cx, int cy, uint16_t colour, float scale);

}  // namespace ip9::icons
