# IP9Kit

Wspólny rdzeń aplikacji IP9 dla M5Stack Cardputer / Cardputer ADV (marka https://cardputer.ip9.pl).
Biblioteka PlatformIO (`library.json`), przestrzeń nazw `ip9`, include'y z prefiksem `ip9/` (`#include "ip9/ui/Painter.h"`).

## Repozytoria

Każda aplikacja to osobne repozytorium w katalogu obok IP9Kit:
`IP9LGRemote`, `IP9VoiceRec`, … Aplikacje pobierają IP9Kit przez `lib_deps` z przypiętym tagiem
(`[ip9kit] source = https://github.com/raptyk/IP9Kit.git#vX.Y.Z`); lokalnie `ip9kit-local.ini`
w aplikacji przełącza na `symlink://../IP9Kit`.

## Zasady

- Kod w 100% po angielsku; teksty UI wspólnych ekranów w `src/ip9/ui/Strings.h` (stopki Font0: max ~38 znaków).
- Do IP9Kit trafia tylko to, czego używają (lub na pewno użyją) co najmniej dwie aplikacje i co nie zna
  żadnej konkretnej aplikacji. Wszystko specyficzne zostaje w aplikacji (Settings, usługi, ekrany funkcji).
- Zmiana zachowania rdzenia = zmiana we wszystkich aplikacjach: po każdej zmianie zbudować wszystkie
  aplikacje z lokalnym IP9Kit (`pio run` w każdej).
- Wersjonowanie semantyczne: `library.json` + `src/ip9/Version.h` + `CHANGELOG.md` + tag `vX.Y.Z`.
- Brak RTTI: aplikacja rozróżnia ekrany przez wirtualne `chipLabel()` / `traits()`, nie przez `dynamic_cast`.
- Ekrany aplikacji dziedziczą z prawdziwych klas aplikacji (`class Screen : public ip9::AppScreen<App>`),
  nie z aliasów – inaczej wstrzyknięta nazwa `ip9::Screen` przesłania nazwę w klasach pochodnych.

## Marka IP9

- Nazwa produktu `IP9 <Nazwa>`, repozytorium i obrazy `IP9<Nazwa>-<wersja>[-full].bin` (`custom_release_name`).
- Każdy README, opis M5Burner i ekran „About” wskazuje https://cardputer.ip9.pl (`ip9::kBrandUrl`).
- Wspólne klawisze, paleta, znaczenie kolorów LED – patrz README, sekcja „IP9 conventions”.
- `tools/make_release.py` to kanoniczna kopia skryptu; aplikacje trzymają identyczną kopię w `scripts/`.
