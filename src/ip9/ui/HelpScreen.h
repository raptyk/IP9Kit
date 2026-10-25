// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Patryk Strzelecki <patryk@strzelecki.info>
#pragma once

#include "ip9/ui/Screen.h"

namespace ip9 {

struct HelpEntry {
    const char* keys;    // drawn as a key cap, e.g. "; . , /"; nullptr = empty slot
    const char* action;
};

struct HelpPage {
    const char* title;
    HelpEntry entries[10];  // two columns x five rows
};

// Cheat sheet of the app's keys, split into pages; , / turn the pages, esc / enter / tab close it.
class HelpScreen : public Screen {
public:
    // `pages` must outlive the screen (a constexpr table); `firstPage` opens the page
    // that belongs to the calling screen.
    HelpScreen(Shell& shell, const char* title, const HelpPage* pages, int pageCount, int firstPage = 0);

    void onKey(const KeyEvent& event) override;
    void draw(M5Canvas& canvas, int top, int height) override;
    const char* footer() const override;

private:
    const char* title_;
    const HelpPage* pages_;
    int pageCount_;
    int page_;
};

}  // namespace ip9
