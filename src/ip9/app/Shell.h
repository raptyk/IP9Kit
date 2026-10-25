// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Patryk Strzelecki <patryk@strzelecki.info>
#pragma once

#include <M5Unified.h>

#include <memory>
#include <vector>

#include "ip9/hw/Device.h"
#include "ip9/hw/KeyboardInput.h"
#include "ip9/hw/StatusLed.h"
#include "ip9/ui/Screen.h"

namespace ip9 {

// Content of the status bar: section chip on the left, coloured state dot + text, battery on the right.
struct StatusLine {
    const char* chip;
    uint16_t chipColour;
    uint16_t dotColour;
    String text;
};

// Common frame of every IP9 Cardputer app: board start-up, the screen stack, the status bar,
// footer and toasts, rendering through one off-screen canvas, screen dimming / switching off
// and the status LED. An app derives its App class from Shell, owns its own services and
// fills in the hooks (statusLine, onLoop, updateStatusLed, ...).
class Shell {
public:
    Shell();
    virtual ~Shell() = default;

    // One main loop iteration (Arduino loop()).
    void loop();

    // Screen stack. The first screen pushed is the root (home) and never closes.
    void push(std::unique_ptr<Screen> screen);
    void pop();
    void popToRoot();
    // Pops screens until `target` is on top (only the top screen if `target` is not on the stack).
    void popTo(Screen* target);
    Screen* top() const { return screens_.empty() ? nullptr : screens_.back().get(); }
    void invalidate() { dirty_ = true; }

    // Short message shown above the footer.
    void toast(const String& message);
    // A toast for something the user has to fix or do first, plus one red double blink.
    void warn(const String& message);
    // The red double blink alone (setup needed, nothing found, a locked option).
    void signalWarning();
    // Two green blinks (saved, connected).
    void signalSuccess();

    // Backlight level and idle timeouts (0 = never), status LED on / off.
    void setDisplayPower(uint8_t brightness, uint32_t dimAfterMs, uint32_t offAfterMs, bool statusLed);

    Device& device() { return device_; }
    KeyboardInput& keyboard() { return keyboard_; }
    StatusLed& statusLed() { return statusLed_; }

protected:
    // Starts the board (M5Cardputer, display in landscape), detects the model and creates the
    // canvas. `config` comes from M5.config() with the app's audio / IMU choices; `accent` is the
    // app's accent colour (selected rows, frames). Call first in the app's begin().
    void beginShell(m5::M5Unified::config_t config, uint16_t accent);

    // Status bar content for the screen on top.
    virtual StatusLine statusLine(const Screen& top) = 0;
    // Services of the app, after the key events and before the top screen's tick().
    virtual void onLoop() {}
    // Sets the status LED background from the app state (called every loop).
    virtual void updateStatusLed() {}
    // True while the screen must not dim (e.g. the air mouse moves the pointer).
    virtual bool keepAwake() const { return false; }

    // A short red flash every 10 s while the battery is at 10 % or less and not charging;
    // for updateStatusLed() when nothing else is shown.
    void blinkIfBatteryLow();
    void setToastDuration(uint32_t ms) { toastDurationMs_ = ms; }

    Device device_;
    KeyboardInput keyboard_;
    StatusLed statusLed_;

private:
    void handleInput();
    void updatePower();
    void render();
    void drawStatusBar();
    void drawFooter(const char* text);
    void drawToast();

    M5Canvas canvas_;
    std::vector<std::unique_ptr<Screen>> screens_;
    std::vector<std::unique_ptr<Screen>> retired_;  // popped screens, freed after the current event
    bool dirty_ = true;
    uint32_t lastRenderAt_ = 0;
    uint32_t lastStatusRefreshAt_ = 0;

    String toastText_;
    uint32_t toastUntil_ = 0;
    uint32_t toastDurationMs_ = 2000;

    uint8_t brightness_ = 128;
    uint32_t dimAfterMs_ = 30000;
    uint32_t offAfterMs_ = 0;
    uint32_t lastInputAt_ = 0;
    bool screenDimmed_ = false;
    bool screenOff_ = false;
    uint32_t lastBatteryBlinkAt_ = 0;
};

}  // namespace ip9
