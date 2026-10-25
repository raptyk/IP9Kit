// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Patryk Strzelecki <patryk@strzelecki.info>
#include "ip9/ui/Icons.h"

#include <math.h>

namespace ip9::icons {

int scaled(float value, float scale) {
    return static_cast<int>(lroundf(value * scale));
}

void drawSettings(M5Canvas& canvas, int cx, int cy, uint16_t colour, float k, uint16_t background) {
    const int outer = scaled(15, k);
    for (int tooth = 0; tooth < 8; ++tooth) {
        const float angle = tooth * PI / 4;
        const int x = cx + static_cast<int>(cosf(angle) * outer);
        const int y = cy + static_cast<int>(sinf(angle) * outer);
        canvas.fillCircle(x, y, max(2, scaled(4, k)), colour);
    }
    canvas.fillCircle(cx, cy, scaled(13, k), colour);
    canvas.fillCircle(cx, cy, scaled(6, k), background);
}

void drawHelp(M5Canvas& canvas, int cx, int cy, uint16_t colour, float k) {
    const int radius = scaled(17, k);
    canvas.fillArc(cx, cy, radius, radius - max(2, scaled(3, k)), 0, 360, colour);
    canvas.setFont(k >= 0.8f ? &fonts::FreeSansBold18pt7b : &fonts::FreeSansBold9pt7b);
    canvas.setTextSize(1);
    canvas.setTextColor(colour);
    canvas.setTextDatum(middle_center);
    canvas.drawString("?", cx + 1, cy + 1);
}

}  // namespace ip9::icons
