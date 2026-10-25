# Changelog

## 1.0.0 – 2026-10-25

First release, extracted from IP9 LG Remote 1.5.0 and IP9 VoiceRec 1.1.1:

- `Shell`: screen stack, status bar (`StatusLine`), footer, toasts, canvas rendering, dimming and
  screen off with the waking key swallowed, status LED hooks, low-battery blink.
- `Device`, `KeyboardInput`, `StatusLed` (configurable steady level), shared pins and LED colours.
- `Painter` with the level meter, progress bar and duration / size formatting.
- `Screen` / `AppScreen<App>`, `ListScreen`, `CarouselScreen`, `HelpScreen`, `ConfirmScreen`,
  `InfoScreen`, settings and help icons.
- Runtime accent colour per app (`theme::setAccent`).
- `tools/make_release.py`: release images named after `custom_release_name`.
