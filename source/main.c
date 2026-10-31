#ifdef __NDS__
#include "ui.h"
#include <nds.h>
#include <stdint.h>
#include <string.h>

static uint16_t top[UI_PIXELS] __attribute__((aligned(32)));
static uint16_t bottom[UI_PIXELS] __attribute__((aligned(32)));
static int8_t click[256] __attribute__((aligned(32)));
static int8_t tone[2048] __attribute__((aligned(32)));
static UI state;
static volatile uint32_t vblanks;
static void onVBlank(void) { vblanks++; }

static UI visibleState(const UI *s) {
  UI v = *s;
  v.frame = s->app == SPARI ? s->frame / 6 : 0;
  v.event = 0;
  v.rng = 0;
  v.spawn = 0;
  v.toastTicks = s->toastTicks > 0;
  v.spariWait = s->spariWait > 0;
  v.flash = s->flash > 9;
  v.gameTime = (s->gameTime + 59) / 60;
  v.musicTicks = s->app == MUSIC ? s->musicTicks / 15 : 0;
  v.clockTicks = s->app == CLOCK ? s->clockTicks / 2 : 0;
  return v;
}

int main(void) {
  defaultExceptionHandler();
  powerOn(POWER_ALL_2D);
  lcdMainOnTop();
  videoSetMode(MODE_5_2D);
  videoSetModeSub(MODE_5_2D);
  vramSetBankA(VRAM_A_MAIN_BG);
  vramSetBankC(VRAM_C_SUB_BG);
  int bgTop = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
  int bgBottom = bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
  bgShow(bgTop);
  bgShow(bgBottom);
  bgUpdate();
  // Hardware affine rotation: logical 192x256 -> physical 256x192.
  // Turn the console 90 degrees counterclockwise: touch screen is on the right.
  REG_BG3PA = 0;
  REG_BG3PB = 256;
  REG_BG3PC = -256;
  REG_BG3PD = 0;
  REG_BG3X = 0;
  REG_BG3Y = 255 << 8;
  REG_BG3PA_SUB = 0;
  REG_BG3PB_SUB = 256;
  REG_BG3PC_SUB = -256;
  REG_BG3PD_SUB = 0;
  REG_BG3X_SUB = 0;
  REG_BG3Y_SUB = 255 << 8;
  irqSet(IRQ_VBLANK, onVBlank);
  irqEnable(IRQ_VBLANK);
  uint16_t *dstTop = bgGetGfxPtr(bgTop);
  uint16_t *dstBottom = bgGetGfxPtr(bgBottom);
  soundEnable();
  for (int i = 0; i < 256; i++)
    click[i] = (int8_t)(((i / 7) % 2 ? 1 : -1) * (70 * (256 - i) / 256));
  static const int8_t wave[32] = {0,   12,  24,  35,  45,  53,  59,  63,
                                  64,  63,  59,  53,  45,  35,  24,  12,
                                  0,   -12, -24, -35, -45, -53, -59, -63,
                                  -64, -63, -59, -53, -45, -35, -24, -12};
  for (int i = 0; i < 2048; i++)
    tone[i] = (int8_t)(wave[i % 32] * (i < 1792 ? 256 : 2048 - i) / 256);
  DC_FlushRange(click, sizeof(click));
  DC_FlushRange(tone, sizeof(tone));
  keysSetRepeat(20, 7);
  ui_init(&state);
  UI lastTop = {0}, lastBottom = {0};
  bool first = true;
  uint32_t lastTick = vblanks;
  int topFrame = -10, bottomFrame = -10;
  bool wasTouch = false;
  while (1) {
    swiWaitForVBlank();
    scanKeys();
    uint32_t held = keysHeld(), down = keysDown();
    Input in = {0};
    in.down = ui_rotate_buttons(
        down | (keysDownRepeat() & (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT)));
    in.held = ui_rotate_buttons(held);
    in.touch = (held & KEY_TOUCH) != 0;
    in.pressed = in.touch && !wasTouch;
    in.released = !in.touch && wasTouch;
    if (in.touch) {
      touchPosition t;
      touchRead(&t);
      ui_rotate_touch(t.px, t.py, &in.x, &in.y);
    }
    wasTouch = in.touch;
    uint32_t now = vblanks, elapsed = now - lastTick;
    lastTick = now;
    int sound = 0;
    for (uint32_t t = 0; t < elapsed; t++) {
      ui_tick(&state, in);
      int nextSound = ui_take_sound(&state);
      if (nextSound)
        sound = nextSound;
      in.down = 0;
      in.pressed = false;
      in.released = false;
    }
    if (sound >= 4) {
      static const int hz[8] = {523, 659, 784, 659, 587, 698, 880, 784};
      soundPlaySample(tone, SoundFormat_8Bit, sizeof(tone),
                      hz[(sound - 4) % 8] * 32, 48, 64, false, 0);
    } else if (sound) {
      soundPlaySample(click, SoundFormat_8Bit, sizeof(click),
                      sound == 2   ? 12000
                      : sound == 3 ? 4200
                                   : 8000,
                      40, 64, false, 0);
    }
    UI visual = visibleState(&state);
    bool animatedTop =
        state.app == BATTERY || state.app == SPARI || state.app == CLOCK;
    if (first || (memcmp(&visual, &lastTop, sizeof(UI)) &&
                  (!animatedTop || state.frame - topFrame >= 6))) {
      ui_render_top(&state, top);
      DC_FlushRange(top, sizeof(top));
      dmaCopy(top, dstTop, sizeof(top));
      lastTop = visual;
      topFrame = state.frame;
    }
    if (first || (state.frame - bottomFrame >= 2 &&
                  memcmp(&visual, &lastBottom, sizeof(UI)))) {
      ui_render_bottom(&state, bottom);
      DC_FlushRange(bottom, sizeof(bottom));
      dmaCopy(bottom, dstBottom, sizeof(bottom));
      lastBottom = visual;
      bottomFrame = state.frame;
    }
    first = false;
  }
}
#endif
