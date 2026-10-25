// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Patryk Strzelecki <patryk@strzelecki.info>
#include "ip9/ui/CarouselScreen.h"

#include "ip9/app/Shell.h"
#include "ip9/ui/Painter.h"
#include "ip9/ui/Theme.h"

namespace ip9 {

namespace {
constexpr uint32_t kSlideMs = 140;
}  // namespace

CarouselScreen::CarouselScreen(Shell& shell, int cardCount) : Screen(shell), cardCount_(cardCount) {}

void CarouselScreen::setSelected(int index) {
    selected_ = constrain(index, 0, cardCount_ - 1);
}

void CarouselScreen::move(int delta) {
    selected_ = (selected_ + delta + cardCount_) % cardCount_;
    slideDirection_ = delta;
    slideStartedAt_ = millis();
    onCardShown(selected_);
    shell_.invalidate();
}

void CarouselScreen::onKey(const KeyEvent& event) {
    if (onExtraKey(event)) {
        return;
    }
    switch (event.code) {
        case keycode::kLeft:
        case keycode::kUp:
            move(-1);
            return;
        case keycode::kRight:
            if (event.shift) {
                openHelp();  // "?"
                return;
            }
            move(1);
            return;
        case keycode::kDown:
            move(1);
            return;
        case keycode::kTab:
            if (!event.repeat) {
                openHelp();
            }
            return;
        case keycode::kEnter:
        case keycode::kButtonG0:
            if (!event.repeat) {
                open(selected_);
            }
            return;
        default:
            if (!event.repeat && event.code >= '1' && event.code <= '9' && event.code < '1' + cardCount_) {
                selected_ = event.code - '1';
                open(selected_);
            }
            return;
    }
}

void CarouselScreen::draw(M5Canvas& canvas, int top, int height) {
    using namespace theme;
    const CarouselCard current = card(selected_);
    const int centerX = kScreenWidth / 2;

    // Short slide-in of the selected icon.
    int offset = 0;
    const uint32_t age = millis() - slideStartedAt_;
    if (slideDirection_ != 0 && age < kSlideMs) {
        offset = slideDirection_ * static_cast<int>(30 * (kSlideMs - age) / kSlideMs);
        shell_.invalidate();
    }

    // Neighbour previews and chevrons hint that the list scrolls sideways.
    const int iconY = top + 24;
    drawIcon(canvas, (selected_ + cardCount_ - 1) % cardCount_, 34, iconY, kSurfaceHigh, 0.55f, kBackground);
    drawIcon(canvas, (selected_ + 1) % cardCount_, kScreenWidth - 34, iconY, kSurfaceHigh, 0.55f, kBackground);
    canvas.fillTriangle(6, iconY, 12, iconY - 6, 12, iconY + 6, kTextMuted);
    canvas.fillTriangle(kScreenWidth - 6, iconY, kScreenWidth - 12, iconY - 6, kScreenWidth - 12, iconY + 6,
                        kTextMuted);

    canvas.fillRoundRect(centerX - 26 + offset, iconY - 22, 52, 44, 10, current.colour);
    drawIcon(canvas, selected_, centerX + offset, iconY, kText, current.iconScale, current.colour);

    canvas.setTextSize(1);
    canvas.setFont(&fonts::FreeSansBold9pt7b);
    canvas.setTextColor(kText);
    canvas.setTextDatum(top_center);
    painter::drawClippedText(canvas, current.title, centerX, top + 49, kScreenWidth - 2 * kPadding);

    // Status line: is this ready, or what is missing?
    canvas.setFont(&fonts::Font0);
    int textTop = top + 66;
    if (!current.status.isEmpty()) {
        const int width = min<int>(canvas.textWidth(current.status), kScreenWidth - 40);
        const int x = centerX - width / 2;
        canvas.fillCircle(x - 7, textTop + 3, 3, current.statusColour);
        canvas.setTextColor(current.statusColour);
        canvas.setTextDatum(top_left);
        painter::drawClippedText(canvas, current.status, x, textTop, kScreenWidth - 40);
        textTop += 12;
    }
    canvas.setTextColor(kTextMuted);
    canvas.setTextDatum(top_center);
    const int lines = (top + height - textTop) / 10;
    painter::drawWrappedText(canvas, current.text, centerX, textTop, kScreenWidth - 2 * kPadding - 8, 10, lines);
}

}  // namespace ip9
