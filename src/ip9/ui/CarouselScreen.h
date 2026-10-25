// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Patryk Strzelecki <patryk@strzelecki.info>
#pragma once

#include <Arduino.h>

#include "ip9/ui/Screen.h"

namespace ip9 {

// What the carousel shows for the selected card.
struct CarouselCard {
    const char* title;
    const char* text;      // word-wrapped description under the title
    uint16_t colour;       // tile behind the big icon
    String status;         // optional status line ("Ready", "Insert a card"); empty = none
    uint16_t statusColour;
    float iconScale = 1.0f;
};

// Home screen of an IP9 app: a carousel of large icons, one card per function.
// Each card explains itself and shows whether it is ready. Keys: , / (and ; .) pick a card,
// enter / G0 open it, 1-9 open a card directly, tab or ? open the help.
class CarouselScreen : public Screen {
public:
    CarouselScreen(Shell& shell, int cardCount);

    void onKey(const KeyEvent& event) override;
    void draw(M5Canvas& canvas, int top, int height) override;

protected:
    virtual CarouselCard card(int index) const = 0;
    virtual void drawIcon(M5Canvas& canvas, int index, int cx, int cy, uint16_t colour, float scale,
                          uint16_t background) const = 0;
    virtual void open(int index) = 0;
    virtual void openHelp() = 0;
    // App-specific keys (e.g. "r" = record now); true when the key was handled.
    virtual bool onExtraKey(const KeyEvent& /*event*/) { return false; }
    // Called whenever another card comes into view by moving left / right.
    virtual void onCardShown(int /*index*/) {}

    void move(int delta);
    int selected() const { return selected_; }
    void setSelected(int index);

private:
    int cardCount_;
    int selected_ = 0;
    int slideDirection_ = 0;  // -1 / +1 during the short slide animation
    uint32_t slideStartedAt_ = 0;
};

}  // namespace ip9
