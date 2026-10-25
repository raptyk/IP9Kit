// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Patryk Strzelecki <patryk@strzelecki.info>
#include "ip9/ui/MessageScreens.h"

#include "ip9/app/Shell.h"
#include "ip9/ui/Painter.h"
#include "ip9/ui/Strings.h"
#include "ip9/ui/Theme.h"

namespace ip9 {

// ---------------------------------------------------------------- ConfirmScreen

ConfirmScreen::ConfirmScreen(Shell& shell, String question, String detail, std::function<void()> onConfirm,
                             std::function<void()> onCancel)
    : Screen(shell),
      question_(std::move(question)),
      detail_(std::move(detail)),
      onConfirm_(std::move(onConfirm)),
      onCancel_(std::move(onCancel)) {}

void ConfirmScreen::onKey(const KeyEvent& event) {
    if (event.repeat) {
        return;
    }
    std::function<void()> action;
    if (event.code == keycode::kEnter || event.code == 'y') {
        action = onConfirm_;
    } else if (event.code == keycode::kEsc || event.code == keycode::kBackspace || event.code == 'n') {
        action = onCancel_;
    } else {
        return;
    }
    shell_.pop();
    if (action) {
        action();
    }
}

const char* ConfirmScreen::footer() const {
    return str::kConfirmFooter;
}

void ConfirmScreen::draw(M5Canvas& canvas, int top, int height) {
    painter::drawCenteredLines(canvas, top, height, question_.c_str(), detail_.isEmpty() ? nullptr : detail_.c_str());
}

// ---------------------------------------------------------------- InfoScreen

InfoScreen::InfoScreen(Shell& shell, String title, String body, String detail, uint32_t autoCloseMs)
    : Screen(shell),
      title_(std::move(title)),
      body_(std::move(body)),
      detail_(std::move(detail)),
      autoCloseMs_(autoCloseMs) {}

void InfoScreen::onEnter() {
    shownAt_ = millis();
}

void InfoScreen::tick() {
    if (autoCloseMs_ > 0 && millis() - shownAt_ > autoCloseMs_) {
        shell_.pop();
    }
}

void InfoScreen::onKey(const KeyEvent& event) {
    if (!event.repeat && (event.code == keycode::kEnter || event.code == keycode::kEsc ||
                          event.code == keycode::kBackspace || event.code == keycode::kTab)) {
        shell_.pop();
    }
}

const char* InfoScreen::footer() const {
    return str::kFooterInfo;
}

void InfoScreen::draw(M5Canvas& canvas, int top, int height) {
    using namespace theme;
    const int margin = 8;
    canvas.fillRoundRect(margin, top + 6, kScreenWidth - 2 * margin, height - 12, 8, kSurface);
    canvas.drawRoundRect(margin, top + 6, kScreenWidth - 2 * margin, height - 12, 8, accent());
    painter::drawCenteredLines(canvas, top + 6, height - 12, title_.c_str(), body_.c_str(),
                               detail_.isEmpty() ? nullptr : detail_.c_str());
}

}  // namespace ip9
