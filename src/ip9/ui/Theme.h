// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Patryk Strzelecki <patryk@strzelecki.info>
#pragma once

#include <stdint.h>

namespace ip9::theme {

// RGB565 palette shared by all IP9 Cardputer apps: dark background for battery and a dim room,
// one strong accent per app, and one colour per section so the active screen is obvious at a glance.
constexpr uint16_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return static_cast<uint16_t>(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

constexpr uint16_t kBackground = rgb(10, 10, 14);
constexpr uint16_t kSurface = rgb(28, 28, 36);
constexpr uint16_t kSurfaceHigh = rgb(48, 48, 60);
constexpr uint16_t kText = rgb(236, 236, 240);
constexpr uint16_t kTextMuted = rgb(140, 140, 155);
constexpr uint16_t kTextDisabled = rgb(80, 80, 92);
constexpr uint16_t kOk = rgb(60, 200, 110);
constexpr uint16_t kWarn = rgb(245, 170, 40);
constexpr uint16_t kError = rgb(235, 70, 60);

// Default accent (IP9 brand red); every app sets its own with setAccent() at start-up.
constexpr uint16_t kBrandAccent = rgb(226, 36, 74);

// Accent of the running app: selected list rows, toast and pop-up frames, spinner.
uint16_t accent();
void setAccent(uint16_t colour);

constexpr int kScreenWidth = 240;
constexpr int kScreenHeight = 135;
constexpr int kStatusBarHeight = 18;
constexpr int kFooterHeight = 16;
constexpr int kPadding = 6;

}  // namespace ip9::theme
