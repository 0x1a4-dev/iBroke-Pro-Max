# Third-party notices

Dou OS's native UI renderer, code-drawn app icons, dialog, gameplay, sound data and application code are provided under `LICENSE`. Photographic assets were generated with Imagegen for this project; their provenance and prompt are in `artwork/ASSETS.md`. The ROM also links unmodified open-source runtime components. Third-party licenses apply to those components and are not replaced by the application's MIT license.

| Component | Use | Source / license |
| --- | --- | --- |
| BlocksDS 1.24.0 SDK | Startup code and ROM integration | https://codeberg.org/blocksds/sdk · Zlib and Mozilla Public License 2.0 notices included |
| BlocksDS libnds | DS/DSi graphics, input, IRQ and sound support | https://codeberg.org/blocksds/libnds · Zlib; additional included component notices under `licenses/` |
| BlocksDS ARM7 main core | Unmodified `arm7_minimal.elf` linked into the ROM | https://codeberg.org/blocksds/sdk/src/tag/v1.24.0/sys/arm7 · Zlib notice included |
| Picolibc | Standard C runtime | https://github.com/picolibc/picolibc · Collection of permissive licenses; COPYING included |
| GCC runtime | Compiler support routines | https://gcc.gnu.org/ · GPLv3 with GCC Runtime Library Exception 3.1 |
| Inter | Rasterized, antialiased font glyphs in `source/assets.c`; unmodified font under `artwork/fonts/` | https://github.com/google/fonts/tree/main/ofl/inter · SIL Open Font License 1.1 in `licenses/Inter-OFL.txt` |

Runtime source is available through the linked upstream repositories. BlocksDS binaries used for this build: https://blocksds.skylyrac.net/packages/rolling/linux/x86_64/blocksds-toolchain-1.24.0-1-x86_64.pkg.tar.xz . The Picolibc package is `toolchain-gcc-arm-none-eabi-picolibc-generic-1.8.13.r26367.770d86889-1`; compiler: Wonderful GCC 16.2.0. The SDK/runtime/compiler themselves are not bundled as development tools.

The unmodified SDK Makefile included by the project's Makefile is CC0-1.0. Its notice is included in `licenses/`.

DeSmuME / libretro is used solely as a separate test tool; the emulator and its code are not included in this release. Test frontend in `tests/emulator_smoke.py` is project code. No BIOS, firmware dump, retail game, proprietary Apple operating system, or third-party music is included.
