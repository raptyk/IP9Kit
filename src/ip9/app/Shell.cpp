// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Patryk Strzelecki <patryk@strzelecki.info>
#include "ip9/app/Shell.h"

#include <M5Cardputer.h>

#include <algorithm>

#include "ip9/hw/LedColours.h"
#include "ip9/ui/Painter.h"
#include "ip9/ui/Theme.h"

namespace ip9 {

namespace {

constexpr uint32_t kStatusRefreshMs = 500;
constexpr uint8_t kDimBrightness = 24;
constexpr uint32_t kMinFrameIntervalMs = 33;  // ~30 fps is plenty and leaves CPU time for input
constexpr int kBatteryWidth = 52;             // battery icon + "100%" on the right of the status bar
constexpr uint32_t kBatteryBlinkMs = 10000;

}  // namespace

Shell::Shell() : canvas_(&M5.Display) {}

void Shell::beginShell(m5::M5Unified::config_t config, uint16_t accent) {
    M5Cardputer.begin(config, true);
    M5.Display.setRotation(1);
    theme::setAccent(accent);
    device_.begin();

    canvas_.setColorDepth(16);
    if (!canvas_.createSprite(theme::kScreenWidth, theme::kScreenHeight)) {
        // Not enough contiguous RAM for 16-bit: fall back to 8-bit colour.
        canvas_.setColorDepth(8);
        canvas_.createSprite(theme::kScreenWidth, theme::kScreenHeight);
    }
    lastInputAt_ = millis();
}

void Shell::loop() {
    M5Cardputer.update();
    device_.update();
    keyboard_.update();
    handleInput();
    onLoop();
    if (!screens_.empty()) {
        screens_.back()->tick();
    }
    retired_.clear();
    updatePower();
    updateStatusLed();
    statusLed_.loop();

    const uint32_t now = millis();
    if (now - lastStatusRefreshAt_ >= kStatusRefreshMs) {
        lastStatusRefreshAt_ = now;
        dirty_ = true;  // status bar (battery, app state) may have changed
    }
    if (toastUntil_ != 0 && now > toastUntil_) {
        toastUntil_ = 0;
        dirty_ = true;
    }
    if (dirty_ && !screenOff_ && !screens_.empty() && now - lastRenderAt_ >= kMinFrameIntervalMs) {
        render();
    }
}

void Shell::push(std::unique_ptr<Screen> screen) {
    screens_.push_back(std::move(screen));
    screens_.back()->onEnter();
    dirty_ = true;
}

void Shell::pop() {
    if (screens_.size() <= 1) {
        return;  // the home screen is the root and never closes
    }
    retired_.push_back(std::move(screens_.back()));
    screens_.pop_back();
    screens_.back()->onEnter();
    dirty_ = true;
}

void Shell::popToRoot() {
    while (screens_.size() > 1) {
        retired_.push_back(std::move(screens_.back()));
        screens_.pop_back();
    }
    screens_.back()->onEnter();
    dirty_ = true;
}

void Shell::popTo(Screen* target) {
    const bool onStack = std::any_of(screens_.begin(), screens_.end(),
                                     [target](const std::unique_ptr<Screen>& screen) { return screen.get() == target; });
    if (!onStack) {
        pop();
        return;
    }
    while (screens_.size() > 1 && screens_.back().get() != target) {
        retired_.push_back(std::move(screens_.back()));
        screens_.pop_back();
    }
    screens_.back()->onEnter();
    dirty_ = true;
}

void Shell::toast(const String& message) {
    toastText_ = message;
    toastUntil_ = millis() + toastDurationMs_;
    dirty_ = true;
}

void Shell::warn(const String& message) {
    toast(message);
    signalWarning();
}

void Shell::signalWarning() {
    // Once per event and short on purpose: a hint, not an alarm.
    statusLed_.blink(led::kError, 2, 70, 90, true);
}

void Shell::signalSuccess() {
    statusLed_.blink(led::kSuccess, 2);
}

void Shell::setDisplayPower(uint8_t brightness, uint32_t dimAfterMs, uint32_t offAfterMs, bool statusLed) {
    brightness_ = brightness;
    dimAfterMs_ = dimAfterMs;
    offAfterMs_ = offAfterMs;
    // ADV: the LED is powered from the backlight rail, so an enabled status light keeps the
    // backlight at full duty (see Device::setLedPowerHold).
    device_.setLedPowerHold(statusLed);
    if (!screenDimmed_ && !screenOff_) {
        device_.setBrightness(brightness_);
    }
    statusLed_.setEnabled(statusLed);
}

void Shell::blinkIfBatteryLow() {
    if (millis() - lastBatteryBlinkAt_ > kBatteryBlinkMs) {
        lastBatteryBlinkAt_ = millis();  // the battery ADC is read at most every 10 s
        const int battery = device_.batteryPercent();
        if (battery >= 0 && battery <= 10 && !device_.isCharging()) {
            statusLed_.flash(led::kError, 40);
        }
    }
}

void Shell::handleInput() {
    while (keyboard_.hasEvent()) {
        KeyEvent event = keyboard_.nextEvent();
        lastInputAt_ = millis();
        if (screenOff_) {
            // The key that wakes the screen is swallowed, so nothing happens by accident
            // (no command sent, no recording stopped).
            screenOff_ = false;
            screenDimmed_ = false;
            device_.setBrightness(brightness_);
            keyboard_.suppressHeldKeys();
            dirty_ = true;
            continue;
        }
        if (screenDimmed_) {
            screenDimmed_ = false;
            device_.setBrightness(brightness_);
        }
        if (!screens_.empty()) {
            screens_.back()->onKey(event);
        }
        dirty_ = true;
    }
}

void Shell::updatePower() {
    if (keyboard_.anyHeld() || keepAwake()) {
        lastInputAt_ = millis();
        return;
    }
    const uint32_t idle = millis() - lastInputAt_;
    if (!screenOff_ && offAfterMs_ > 0 && idle > offAfterMs_) {
        screenOff_ = true;
        device_.setBrightness(0);
    } else if (!screenDimmed_ && !screenOff_ && dimAfterMs_ > 0 && idle > dimAfterMs_) {
        screenDimmed_ = true;
        device_.setBrightness(min<uint8_t>(kDimBrightness, brightness_));
    }
}

void Shell::render() {
    dirty_ = false;
    lastRenderAt_ = millis();
    canvas_.fillScreen(theme::kBackground);

    const int contentTop = theme::kStatusBarHeight;
    const int contentHeight = theme::kScreenHeight - theme::kStatusBarHeight - theme::kFooterHeight;
    Screen& screen = *screens_.back();
    screen.draw(canvas_, contentTop, contentHeight);

    drawStatusBar();
    drawFooter(screen.footer());
    drawToast();
    canvas_.pushSprite(0, 0);
}

void Shell::drawStatusBar() {
    using namespace theme;
    canvas_.fillRect(0, 0, kScreenWidth, kStatusBarHeight, kSurface);
    const StatusLine line = statusLine(*screens_.back());

    // Chip: which part of the app this is (or the active mode).
    canvas_.setFont(&fonts::Font0);
    canvas_.setTextSize(1);
    const int chipWidth = canvas_.textWidth(line.chip) + 12;
    canvas_.fillRoundRect(2, 2, chipWidth, kStatusBarHeight - 4, 4, line.chipColour);
    canvas_.setTextColor(kText);
    canvas_.setTextDatum(middle_center);
    canvas_.drawString(line.chip, 2 + chipWidth / 2, kStatusBarHeight / 2);

    // State: coloured dot + what the app is doing / connected to.
    const int dotX = 2 + chipWidth + 8;
    canvas_.fillCircle(dotX, kStatusBarHeight / 2, 3, line.dotColour);
    canvas_.setTextDatum(middle_left);
    canvas_.setTextColor(kText);
    painter::drawClippedText(canvas_, line.text, dotX + 7, kStatusBarHeight / 2,
                             kScreenWidth - kBatteryWidth - (dotX + 7));

    painter::drawBattery(canvas_, kScreenWidth - 4, kStatusBarHeight / 2, device_.batteryPercent(),
                         device_.isCharging());
}

void Shell::drawFooter(const char* text) {
    using namespace theme;
    const int top = kScreenHeight - kFooterHeight;
    canvas_.fillRect(0, top, kScreenWidth, kFooterHeight, kSurface);
    if (text == nullptr) {
        return;
    }
    canvas_.setFont(&fonts::Font0);
    canvas_.setTextSize(1);
    canvas_.setTextColor(kTextMuted);
    canvas_.setTextDatum(middle_center);
    canvas_.drawString(text, kScreenWidth / 2, top + kFooterHeight / 2);
}

void Shell::drawToast() {
    using namespace theme;
    if (toastUntil_ == 0) {
        return;
    }
    canvas_.setFont(&fonts::Font2);
    canvas_.setTextSize(1);
    const int maxTextWidth = kScreenWidth - 24;
    std::vector<String> lines = painter::wrapText(canvas_, toastText_, maxTextWidth);
    if (lines.size() > 2) {
        lines.resize(2);
    }
    int textWidth = 0;
    for (const String& line : lines) {
        textWidth = max<int>(textWidth, canvas_.textWidth(line));
    }
    const int lineHeight = 15;
    const int width = min<int>(textWidth + 16, kScreenWidth - 8);
    const int height = static_cast<int>(lines.size()) * lineHeight + 8;
    const int x = (kScreenWidth - width) / 2;
    const int y = kScreenHeight - kFooterHeight - height - 4;
    canvas_.fillRoundRect(x, y, width, height, 6, kSurfaceHigh);
    canvas_.drawRoundRect(x, y, width, height, 6, accent());
    canvas_.setTextColor(kText);
    canvas_.setTextDatum(top_center);
    for (size_t i = 0; i < lines.size(); ++i) {
        painter::drawClippedText(canvas_, lines[i], kScreenWidth / 2, y + 4 + i * lineHeight, maxTextWidth);
    }
}

}  // namespace ip9
