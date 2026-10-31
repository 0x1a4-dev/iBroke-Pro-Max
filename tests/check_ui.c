#include "ui.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const char *__asan_default_options(void) { return "detect_leaks=0"; }
int __lsan_is_turned_off(void) { return 1; }
static void key(UI *s, unsigned k) {
  Input i = {0};
  i.down = i.held = k;
  ui_tick(s, i);
}
static void tap(UI *s, int x, int y) {
  Input i = {0};
  i.pressed = i.touch = true;
  i.x = x;
  i.y = y;
  ui_tick(s, i);
}
static void frames(UI *s, int n) {
  Input i = {0};
  while (n--)
    ui_tick(s, i);
}
static void calc(UI *s, int n) { tap(s, 29 + (n % 4) * 44, 99 + (n / 4) * 42); }
static void open(UI *s, int a) {
  if (s->app != HOME)
    key(s, DO_B);
  int found = 0;
  for (int n = 0; n < 12; n++)
    if (ui_home_apps[n] == a) {
      tap(s, 24 + (n % 3) * 47, 109 + (n / 3) * 38);
      found = 1;
      break;
    }
  if (!found) {
    int apps[] = {PHONE, SAFARI, MESSAGES, MUSIC};
    for (int n = 0; n < 4; n++)
      if (apps[n] == a) {
        tap(s, 168, 100 + n * 38);
        found = 1;
        break;
      }
  }
  assert(found && s->app == a);
}
static void save(UI *s, const char *dir, const char *name) {
  uint16_t *a = calloc(UI_PIXELS + 2, 2), *b = calloc(UI_PIXELS + 2, 2);
  assert(a && b);
  a[0] = b[0] = 0x1234;
  a[UI_PIXELS + 1] = b[UI_PIXELS + 1] = 0x5678;
  ui_render_top(s, a + 1);
  ui_render_bottom(s, b + 1);
  assert(a[0] == 0x1234 && b[0] == 0x1234 && a[UI_PIXELS + 1] == 0x5678 &&
         b[UI_PIXELS + 1] == 0x5678);
  if (dir) {
    char path[512];
    snprintf(path, sizeof(path), "%s/%s.ppm", dir, name);
    FILE *f = fopen(path, "wb");
    assert(f);
    fprintf(f, "P6\n384 256\n255\n");
    for (int y = 0; y < 256; y++)
      for (int x = 0; x < 384; x++) {
        uint16_t v = (x < 192 ? a : b)[1 + y * UI_STRIDE + x % 192];
        unsigned char rgb[] = {((v & 31) * 255 + 15) / 31,
                               (((v >> 5) & 31) * 255 + 15) / 31,
                               (((v >> 10) & 31) * 255 + 15) / 31};
        fwrite(rgb, 1, 3, f);
      }
    fclose(f);
  }
  free(a);
  free(b);
}
int main(int argc, char **argv) {
  const char *dir = argc > 1 ? argv[1] : NULL;
  UI s;
  ui_init(&s);
  assert(ui_rotate_buttons(DO_UP) == DO_LEFT);
  assert(ui_rotate_buttons(DO_RIGHT) == DO_UP);
  assert(ui_rotate_buttons(DO_DOWN) == DO_RIGHT);
  assert(ui_rotate_buttons(DO_LEFT | DO_A) == (DO_DOWN | DO_A));
  int x, y;
  ui_rotate_touch(125, 90, &x, &y);
  assert(x == 90 && y == 130);
  save(&s, dir, "00_lock");
  tap(&s, 96, 236);
  Input drag = {0};
  drag.touch = true;
  drag.x = 96;
  drag.y = 160;
  ui_tick(&s, drag);
  assert(s.app == HOME);
  save(&s, dir, "01_home");
  key(&s, DO_START);
  assert(s.app == LOCK);
  key(&s, DO_A);
  for (int a = 1; a <= CLOCK; a++) {
    open(&s, a);
    char name[32];
    snprintf(name, sizeof(name), "%02d_app_%d", a + 1, a);
    save(&s, dir, name);
    tap(&s, 22, 29);
    assert(s.app == HOME);
  }
  open(&s, SPARI);
  tap(&s, 96, 117);
  frames(&s, 50);
  assert(s.spariReply == 0 && !s.spariWait);
  save(&s, dir, "20_spari");
  open(&s, CAMERA);
  key(&s, DO_A);
  frames(&s, 14);
  assert(s.photos == 1);
  key(&s, DO_X);
  assert(s.filter == 1);
  save(&s, dir, "21_photo");
  open(&s, STORE);
  int old = s.credits;
  key(&s, DO_A);
  assert(s.credits == old - 99 && s.purchases == 1);
  save(&s, dir, "22_store");
  open(&s, BATTERY);
  key(&s, DO_A);
  assert(s.gameState == 1);
  s.drops[0] = (Drop){96, 215, 0, 1};
  frames(&s, 1);
  assert(s.gameScore == 10 && s.gameReward == 8);
  for (int n = 0; n < 1800 && s.gameState == 1; n++) {
    Input i = {0};
    int target = -1, yy = -1;
    for (int j = 0; j < 9; j++)
      if (s.drops[j].alive && !s.drops[j].kind && s.drops[j].y > yy) {
        target = s.drops[j].x;
        yy = s.drops[j].y;
      }
    if (target >= 0) {
      i.touch = true;
      i.x = target;
      i.y = 230;
    }
    ui_tick(&s, i);
    if (n == 200)
      save(&s, dir, "23_game");
  }
  assert(s.gameState == 2 && s.best == s.gameScore && s.gameReward > 0);
  save(&s, dir, "24_game_result");
  open(&s, CALC);
  calc(&s, 8);
  calc(&s, 15);
  calc(&s, 9);
  calc(&s, 14);
  assert(s.calcValue == 3);
  calc(&s, 12);
  calc(&s, 4);
  calc(&s, 3);
  calc(&s, 13);
  calc(&s, 14);
  assert(s.calcError == 1);
  calc(&s, 12);
  key(&s, DO_Y);
  calc(&s, 8);
  calc(&s, 13);
  calc(&s, 13);
  calc(&s, 14);
  assert(s.calcValue == 119 && s.calcFee == 19);
  save(&s, dir, "25_calc");
  open(&s, NOTES);
  key(&s, DO_X);
  tap(&s, 13, 170);
  assert(s.noteLen == 1 && s.note[0] == 'Q');
  tap(&s, 168, 216);
  assert(s.noteLen == 0);
  tap(&s, 103, 170);
  tap(&s, 31, 216);
  assert(s.noteLen == 2 && s.note[0] == 'Y' && s.note[1] == 'Z');
  key(&s, DO_X);
  s.noteCursor = 0;
  for (int i = 0; i < 90; i++)
    key(&s, DO_A);
  assert(s.noteLen == 80 && s.note[80] == 0);
  strcpy(s.note, "Skip the upgrade. Keep the DS.");
  s.noteLen = (int)strlen(s.note);
  save(&s, dir, "26_notes");
  open(&s, CALENDAR);
  key(&s, DO_RIGHT);
  assert(s.calendarDay == 31);
  key(&s, DO_RIGHT);
  assert(s.calendarDay == 1);
  open(&s, PHONE);
  tap(&s, 37, 84);
  assert(s.dialLen == 1 && s.dial[0] == '1');
  tap(&s, 96, 236);
  assert(s.phoneCalling);
  save(&s, dir, "27_phone");
  open(&s, MUSIC);
  tap(&s, 96, 160);
  assert(s.musicPlaying);
  frames(&s, 35);
  assert(s.musicTicks >= 35);
  tap(&s, 96, 160);
  assert(!s.musicPlaying);
  save(&s, dir, "28_music");
  open(&s, CLOCK);
  tap(&s, 140, 198);
  frames(&s, 120);
  assert(s.clockRunning && s.clockTicks >= 120);
  tap(&s, 140, 198);
  int ticks = s.clockTicks;
  frames(&s, 15);
  assert(s.clockTicks == ticks);
  save(&s, dir, "29_clock");
  tap(&s, 96, 250);
  drag.touch = true;
  drag.x = 96;
  drag.y = 190;
  ui_tick(&s, drag);
  assert(s.app == HOME);
  key(&s, DO_Y);
  save(&s, dir, "30_pastel");
  unsigned rng = 7;
  for (int n = 0; n < 10000; n++) {
    rng = rng * 1664525u + 1013904223u;
    Input i = {0};
    i.x = (rng >> 8) % 192;
    i.y = (rng >> 16) % 256;
    i.pressed = i.touch = (rng & 7) == 0;
    i.down = (rng & 31) == 0 ? 1u << ((rng >> 24) % 12) : 0;
    i.held = i.down;
    ui_tick(&s, i);
    if (n % 37 == 0)
      save(&s, NULL, "fuzz");
  }
  puts("PASS: rotated touch/d-pad; swipe unlock/home; 16 apps; "
       "purchases/photo; calculator; full minigame; keyboard bounds; "
       "phone/music/stopwatch; framebuffer bounds; 10000 input frames.");
}
