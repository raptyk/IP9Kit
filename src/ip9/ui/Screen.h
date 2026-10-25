// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Patryk Strzelecki <patryk@strzelecki.info>
#pragma once

#include <M5GFX.h>

#include <utility>

#include "ip9/hw/KeyboardInput.h"
#include "ip9/ui/Theme.h"

namespace ip9 {

class Shell;

// A full-screen view. The Shell owns a stack of screens; only the top one is drawn
// and receives key events. The status bar and footer are drawn by the Shell.
class Screen {
public:
    explicit Screen(Shell& shell) : shell_(shell) {}
    virtual ~Screen() = default;

    virtual void onEnter() {}
    virtual void onKey(const KeyEvent& event) = 0;
    // Called on every main loop iteration while the screen is on top.
    virtual void tick() {}
    // Draws the content area (between status bar and footer).
    virtual void draw(M5Canvas& canvas, int top, int height) = 0;
    virtual const char* footer() const { return nullptr; }
    // Chip on the left of the status bar: which section this is. nullptr leaves the chip
    // to the app (Shell::statusLine), e.g. the active link mode of a remote.
    virtual const char* chipLabel() const { return nullptr; }
    virtual uint16_t chipColour() const { return theme::accent(); }
    // Flags with an app-defined meaning (the shell never reads them), e.g. "sends infrared only".
    virtual uint32_t traits() const { return 0; }

protected:
    Shell& shell_;
};

// A screen with typed access to the app that owns it: `AppT` derives from Shell, `Base` is
// Screen or one of the shared screens (ListScreen, CarouselScreen, ...). An app declares
//   class Screen : public ip9::AppScreen<App> { public: using AppScreen::AppScreen; };
//   class ListScreen : public ip9::AppScreen<App, ip9::ListScreen> { public: using AppScreen::AppScreen; };
// and its screens use `app_` for the app's own services. (Real classes, not aliases: their
// names then hide ip9::Screen / ip9::ListScreen inside the derived screens.)
template <class AppT, class Base = Screen>
class AppScreen : public Base {
public:
    template <class... Args>
    explicit AppScreen(AppT& app, Args&&... args) : Base(app, std::forward<Args>(args)...), app_(app) {}

protected:
    AppT& app_;
};

}  // namespace ip9
