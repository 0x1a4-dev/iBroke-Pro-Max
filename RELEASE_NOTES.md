# iBroke Pro Max v2.1.0 — English Fold Edition

Two screens. One mortgage.

A sideways Nintendo DS/DSi homebrew parody with an iPhone-style home screen, widgets, a translucent dock, and 16 mini-apps. Includes Spari, a 48-pixel camera simulation, a fake App Store, a charger-catching minigame, editable QWERTY notes, an integer calculator, a stopwatch, and original chiptunes.

## Downloads

- `iPhone_Dou.nds` — ready-to-run homebrew ROM; all required graphics and sounds are embedded.
- `iBroke-Pro-Max-v2.1.0.zip` — ROM, complete source, English guide, required assets, licenses, and test reports.
- `SHA256SUMS.txt` — SHA-256 checksums for the downloadable files.

## Run

Copy `iPhone_Dou.nds` to your SD card and launch it with an existing homebrew loader such as TWiLight Menu++. Turn the open console **90° counterclockwise** so the touchscreen is on the **right**. Swipe up or press A to unlock; B returns home and START locks.

The launcher title is **iPhone Dou**. Purchases use play money. Camera, calls, maps, weather, chats, and Safari pages are offline simulations. Settings, notes, and scores last for the current session only.

## Verification

- Built with BlocksDS 1.24.0 and Wonderful GCC 16.2.0.
- All 16 apps, both rotated screens, touch targets, all four D-pad directions, swipe gestures, and sound checked in DeSmuME Libretro / Interpreter mode.
- Host checks pass with AddressSanitizer and UndefinedBehaviorSanitizer, including 10,000 input frames and a complete minigame round.
- **Physical DS/DSi testing is still pending.** Use Interpreter mode with the tested DeSmuME core.

Independent unofficial parody; not an Apple or Nintendo release. Project code is MIT licensed; third-party runtime/font notices are included. Created with assistance from Codex, with Imagegen-generated photo assets.
