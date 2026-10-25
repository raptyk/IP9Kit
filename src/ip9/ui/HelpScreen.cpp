// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Patryk Strzelecki <patryk@strzelecki.info>
#include "ip9/ui/HelpScreen.h"

#include "ip9/app/Shell.h"
#include "ip9/ui/Painter.h"
#include "ip9/ui/Strings.h"
#include "ip9/ui/Theme.h"

namespace ip9 {

HelpScreen::HelpScreen(Shell& shell, const char* title, const HelpPage* pages, int pageCount, int firstPage)
    : Screen(shell),
      title_(title),
      pages_(pages),
      pageCount_(pageCount),
      page_(constrain(firstPage, 0, pageCount - 1)) {}

void HelpScreen::onKey(const KeyEvent& event) {
    switch (event.code) {
        case keycode::kLeft:
            page_ = (page_ + pageCount_ - 1) % pageCount_;
            break;
        case keycode::kRight:
            if (event.shift) {
                shell_.pop();  // "?" again closes the help
                return;
            }
            page_ = (page_ + 1) % pageCount_;
            break;
        case keycode::kEsc:
        case keycode::kBackspace:
        case keycode::kEnter:
        case keycode::kTab:
            if (!event.repeat) {
                shell_.pop();
            }
            return;
        default:
            break;
    }
}

const char* HelpScreen::footer() const {
    return str::kHelpFooter;
}

void HelpScreen::draw(M5Canvas& canvas, int top, int height) {
    using namespace theme;
    const HelpPage& page = pages_[page_];
    char title[40];
    snprintf(title, sizeof(title), "%s  %d/%d  %s", title_, page_ + 1, pageCount_, page.title);
    const int contentTop = painter::drawTitle(canvas, top, title);
    const int rowHeight = (top + height - contentTop - 2) / 5;
    const int columnWidth = kScreenWidth / 2;

    canvas.setFont(&fonts::Font0);
    canvas.setTextSize(1);
    for (int i = 0; i < 10; ++i) {
        const HelpEntry& entry = page.entries[i];
        if (entry.keys == nullptr) {
            continue;
        }
        const int column = i / 5;
        const int row = i % 5;
        const int x = column * columnWidth + kPadding;
        const int y = contentTop + row * rowHeight + rowHeight / 2;

        // Key cap.
        const int capWidth = canvas.textWidth(entry.keys) + 8;
        canvas.fillRoundRect(x, y - 7, capWidth, 14, 3, kSurfaceHigh);
        canvas.setTextColor(kText);
        canvas.setTextDatum(middle_center);
        canvas.drawString(entry.keys, x + capWidth / 2, y);

        canvas.setTextColor(kTextMuted);
        canvas.setTextDatum(middle_left);
        canvas.drawString(entry.action, x + capWidth + 5, y);
    }
}

}  // namespace ip9
