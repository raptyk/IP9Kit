# IP9Kit

Shared core of the **IP9 apps** for the **M5Stack Cardputer** and **Cardputer ADV** – https://cardputer.ip9.pl

Every IP9 app is its own repository and firmware; IP9Kit is the part they have in common, so a new
app starts with a working home screen, menus, help, dialogs, status bar, status LED and power
handling, and all apps look and behave the same.

| App | Repository |
|---|---|
| IP9 LG Remote – Magic-Remote-style controller for LG webOS TVs | https://github.com/raptyk/IP9LGRemote |
| IP9 VoiceRec – MP3 voice recorder with voice activation | https://github.com/raptyk/IP9VoiceRec |

Author: Patryk Strzelecki <patryk@strzelecki.info> · License: MIT

## What is inside

```
src/IP9Kit.h          umbrella header
src/ip9/
  app/Shell           base class of an app's App: screen stack, status bar, footer, toasts,
                      rendering through one canvas, dimming / screen off, status LED, battery blink
  hw/Device           model detection (Cardputer / ADV), smoothed battery gauge, backlight
  hw/KeyboardInput    key events with auto-repeat, fn arrows, G0 button, held-key suppression
  hw/StatusLed        RGB LED on GPIO21: background pattern + queue of flashes / blinks
  hw/Pins, LedColours GPIOs shared by both boards, LED colours with a fixed meaning
  ui/Theme            palette, screen geometry, per-app accent colour
  ui/Painter          text clipping / wrapping, list rows, titles, battery, key hints,
                      scroll bar, spinner, level meter, progress bar, durations and sizes
  ui/Screen           Screen base class and AppScreen<App> (typed access to the app)
  ui/ListScreen       self-explaining menus: value rows, descriptions, locked rows
  ui/CarouselScreen   home screen: one large card per function, status line per card
  ui/HelpScreen       paged key cheat sheet
  ui/MessageScreens   ConfirmScreen (yes / no) and InfoScreen (pop-up)
  ui/Icons            shared icons (settings, help)
  ui/Strings          texts of the shared screens
tools/make_release.py PlatformIO post-build step producing <name>-<version>[-full].bin
```

## Using it in an app

`platformio.ini`:

```ini
[platformio]
extra_configs = ip9kit-local*.ini

[ip9kit]
source = https://github.com/raptyk/IP9Kit.git#v1.0.0

[env]
build_unflags = -std=gnu++11
build_src_flags = -std=gnu++17 -Wall -Wextra -Wno-missing-field-initializers
extra_scripts = post:scripts/make_release.py
custom_release_name = IP9MyApp
lib_deps =
    m5stack/M5Cardputer
    ${ip9kit.source}
```

To work on IP9Kit and an app at the same time, keep both checkouts side by side and create
`ip9kit-local.ini` (git-ignored) in the app:

```ini
[ip9kit]
source = symlink://../IP9Kit
```

The app's `src/ui/Screen.h` brings the kit into the app's namespace and declares the app's screen
base classes:

```cpp
#include <IP9Kit.h>

namespace myapp {
using namespace ip9;
class App;
class Screen : public ip9::AppScreen<App> { public: using AppScreen::AppScreen; };
class ListScreen : public ip9::AppScreen<App, ip9::ListScreen> { public: using AppScreen::AppScreen; };
}
```

`App` derives from `ip9::Shell`, calls `beginShell(config, accent)` first in `begin()`, pushes its
home screen (an `ip9::CarouselScreen`) and implements `statusLine()`; `onLoop()`,
`updateStatusLed()` and `keepAwake()` are optional. IP9 LG Remote and IP9 VoiceRec are complete
examples.

## IP9 conventions

- **Name**: the product is called `IP9 <Name>` (UI, M5Burner listing), the repository and the
  release images `IP9<Name>` (`IP9LGRemote-1.6.0.bin`). M5Launcher shows the first letters of
  the file name, so every IP9 image starts with `IP9`.
- **Home**: https://cardputer.ip9.pl – every README, M5Burner listing and About screen points
  there (`ip9::kBrandUrl`).
- **Keys**: `; . , /` arrows (with or without `fn`), `enter` / `G0` open, `esc` / `del` back,
  `tab` or `?` help, `1`–`9` open a card or menu row directly.
- **Look**: dark palette from `ip9::theme`, one accent per app, one colour per section or mode;
  every menu row explains itself at the bottom of the screen; footers fit Font0 (max ~38 characters).
- **Status LED**: two green blinks = done / connected, red double blink = something to fix,
  short red blink every 10 s = battery at 10 % or less.
- **Code**: English only; UI texts in `Strings.h`; includes relative to `src/`; C++17.
- **Versions**: the kit uses semantic versioning and git tags (`v1.0.0`); apps pin a tag.

## Releasing IP9Kit

1. Bump `version` in `library.json` and `kKitVersion` in `src/ip9/Version.h`, add a CHANGELOG entry.
2. Build every app against the local checkout (`ip9kit-local.ini`).
3. Commit, tag `vX.Y.Z`, push the tag; then move the apps' `[ip9kit] source` to the new tag.
