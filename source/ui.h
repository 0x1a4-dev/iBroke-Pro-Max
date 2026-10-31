#pragma once
#include <stdbool.h>
#include <stdint.h>

enum {
  DO_A = 1,
  DO_B = 2,
  DO_SELECT = 4,
  DO_START = 8,
  DO_RIGHT = 16,
  DO_LEFT = 32,
  DO_UP = 64,
  DO_DOWN = 128,
  DO_R = 256,
  DO_L = 512,
  DO_X = 1024,
  DO_Y = 2048
};
enum {
  LOCK = -1,
  HOME = 0,
  SPARI,
  CAMERA,
  STORE,
  BATTERY,
  MESSAGES,
  WEATHER,
  CALC,
  SETTINGS,
  PHOTOS,
  CALENDAR,
  NOTES,
  MAPS,
  PHONE,
  SAFARI,
  MUSIC,
  CLOCK
};
#define UI_WIDTH 192
#define UI_HEIGHT 256
#define UI_STRIDE 256
#define UI_PIXELS (UI_STRIDE * UI_HEIGHT)
extern const int ui_home_apps[12];
typedef struct {
  uint32_t down, held;
  int x, y;
  bool touch, pressed, released;
} Input;
typedef struct {
  int x, y, kind, alive;
} Drop;
typedef struct {
  int app, selection, frame, theme, sound, event;
  int lockDrag, lockX, touchStartX, touchStartY;
  int toastTicks;
  char toast[72];
  int credits, spent, purchases, photos, filter, flash;
  int spariQuestion, spariReply, spariWait, spariSerial;
  int contact, forecast, settingInfo;
  int gameState, gameTime, gameBattery, gameScore, best, playerX, spawn,
      gameReward;
  uint32_t rng;
  Drop drops[9];
  int calcCursor, calcPremium, calcFresh, calcError;
  int64_t calcValue, calcLeft, calcFee;
  char calcOp;
  int calendarDay, mapRoute, noteCursor, noteLen;
  char note[81];
  int dialCursor, dialLen, phoneCalling;
  char dial[17];
  int browserPage, musicPlaying, musicTrack, musicTicks, clockRunning,
      clockTicks;
} UI;

void ui_init(UI *s);
void ui_tick(UI *s, Input in);
void ui_render_top(const UI *s, uint16_t *pixels);
void ui_render_bottom(const UI *s, uint16_t *pixels);
int ui_take_sound(UI *s);
uint32_t ui_rotate_buttons(uint32_t physical);
void ui_rotate_touch(int physical_x, int physical_y, int *x, int *y);
