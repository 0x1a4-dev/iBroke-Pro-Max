/* Dou OS 2.1 English: an original, offline phone parody for a sideways DS. MIT.
 */
#include "ui.h"
#include "assets.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define W UI_WIDTH
#define H UI_HEIGHT
#define S UI_STRIDE
#define RGB(r, g, b)                                                           \
  (0x8000u | ((r) >> 3) | (((g) >> 3) << 5) | (((b) >> 3) << 10))
static const uint16_t BLACK = RGB(20, 20, 24), WHITE = RGB(255, 255, 255),
                      BG = RGB(242, 242, 247), GRAY = RGB(122, 122, 131),
                      LINE = RGB(225, 225, 231), BLUE = RGB(0, 122, 255),
                      GREEN = RGB(48, 209, 88), RED = RGB(255, 59, 48),
                      ORANGE = RGB(255, 159, 10);
const int ui_home_apps[12] = {CALENDAR, PHOTOS,  CAMERA, NOTES,
                              CLOCK,    MAPS,    STORE,  SPARI,
                              SETTINGS, WEATHER, CALC,   BATTERY};
static const int dockApps[4] = {PHONE, SAFARI, MESSAGES, MUSIC};
static const char *names[] = {
    "Dou OS",  "Spari",      "Camera",   "App Store", "Battery+", "Messages",
    "Weather", "Calculator", "Settings", "Photos",    "Calendar", "Notes",
    "Maps",    "Phone",      "Safari",   "Music",     "Clock"};
static const char *shortNames[] = {
    "Dou OS",  "Spari",      "Camera",   "App Store", "Battery+", "Messages",
    "Weather", "Calculator", "Settings", "Photos",    "Calendar", "Notes",
    "Maps",    "Phone",      "Safari",   "Music",     "Clock"};
static const char *questions[] = {"Am I rich?", "What's new?", "Help me save."};
static const char *answers[] = {
    "Two screens. Your bank balance just went blank.",
    "Emotionally? Maybe. After the accessories? No.",
    "Wealth is relative. The invoice is absolute.",
    "The price. Everything else got a new adjective.",
    "Two screens now. Your DS looks oddly unimpressed.",
    "We call it Dou. Innovation is when the name sounds new.",
    "Keep your DSi. Oops. That was my last day on the job.",
    "Buy nothing. Sadly, that tip is incompatible with us.",
    "I turned your savings goal into a monthly subscription. You're welcome."};
static const char *products[] = {"Air Pro", "USB-Courage", "Stand"};
static const char *productSub[] = {"0 g. Full price.",
                                   "Adapter for your adapter.",
                                   "It stands. Your balance falls."};
static const int prices[] = {99, 79, 999};
static const char *tracks[] = {"Charger not included", "Think twice",
                               "Air in your cart"};
static int clamp(int v, int a, int b) { return v < a ? a : v > b ? b : v; }
uint32_t ui_rotate_buttons(uint32_t k) {
  uint32_t out = k & ~(DO_UP | DO_RIGHT | DO_DOWN | DO_LEFT);
  if (k & DO_RIGHT)
    out |= DO_UP;
  if (k & DO_DOWN)
    out |= DO_RIGHT;
  if (k & DO_LEFT)
    out |= DO_DOWN;
  if (k & DO_UP)
    out |= DO_LEFT;
  return out;
}
void ui_rotate_touch(int px, int py, int *x, int *y) {
  *x = clamp(py, 0, 191);
  *y = clamp(255 - px, 0, 255);
}
static void pixel(uint16_t *p, int x, int y, uint16_t c) {
  if ((unsigned)x < W && (unsigned)y < H)
    p[y * S + x] = c;
}
static void rect(uint16_t *p, int x, int y, int w, int h, uint16_t c) {
  int x0 = clamp(x, 0, W), x1 = clamp(x + w, 0, W), y0 = clamp(y, 0, H),
      y1 = clamp(y + h, 0, H);
  for (int j = y0; j < y1; j++)
    for (int i = x0; i < x1; i++)
      p[j * S + i] = c;
}
static int inset(int j, int h, int r) {
  int dy = j < r ? r - j - 1 : j >= h - r ? j - (h - r) : 0, c = 0;
  if (j < r || j >= h - r)
    while (c < r && (r - c - 1) * (r - c - 1) + dy * dy > r * r)
      c++;
  return c;
}
static void rr(uint16_t *p, int x, int y, int w, int h, int r, uint16_t c) {
  r = clamp(r, 0, (w < h ? w : h) / 2);
  for (int j = 0; j < h; j++) {
    int cut = inset(j, h, r);
    rect(p, x + cut, y + j, w - 2 * cut, 1, c);
  }
}
static uint16_t mix(uint16_t d, uint16_t c, int a) {
  if (a >= 15)
    return c;
  int r = ((d & 31) * (16 - a) + (c & 31) * a) >> 4;
  int g = (((d >> 5) & 31) * (16 - a) + ((c >> 5) & 31) * a) >> 4;
  int b = (((d >> 10) & 31) * (16 - a) + ((c >> 10) & 31) * a) >> 4;
  return 0x8000 | r | (g << 5) | (b << 10);
}
static void glass(uint16_t *p, int x, int y, int w, int h, int r, uint16_t c,
                  int a) {
  for (int j = 0; j < h; j++) {
    int cut = inset(j, h, r);
    for (int i = cut; i < w - cut; i++) {
      int xx = x + i, yy = y + j;
      if ((unsigned)xx < W && (unsigned)yy < H)
        p[yy * S + xx] = mix(p[yy * S + xx], c, a);
    }
  }
}
static void circle(uint16_t *p, int x, int y, int r, uint16_t c) {
  for (int dy = -r; dy <= r; dy++) {
    int dx = r;
    while (dx * dx + dy * dy > r * r)
      dx--;
    rect(p, x - dx, y + dy, dx * 2 + 1, 1, c);
  }
}
static void line(uint16_t *p, int x0, int y0, int x1, int y1, uint16_t c) {
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1, dy = -abs(y1 - y0),
      sy = y0 < y1 ? 1 : -1, e = dx + dy;
  for (;;) {
    pixel(p, x0, y0, c);
    if (x0 == x1 && y0 == y1)
      break;
    int z = e * 2;
    if (z >= dy) {
      e += dy;
      x0 += sx;
    }
    if (z <= dx) {
      e += dx;
      y0 += sy;
    }
  }
}
static uint32_t nextchar(const char **t) {
  const unsigned char *p = (const unsigned char *)*t;
  uint32_t c = *p++;
  if (c >= 0xc0 && c < 0xe0 && *p)
    c = ((c & 31) << 6) | (*p++ & 63);
  else if (c >= 0xe0 && c < 0xf0 && p[0] && p[1]) {
    c = ((c & 15) << 12) | ((p[0] & 63) << 6) | (p[1] & 63);
    p += 2;
  }
  *t = (const char *)p;
  return c;
}
static const Glyph *glyph(int f, uint32_t c) {
  const Glyph *g = glyphs + f * GLYPHS_PER_FONT;
  if (c >= 32 && c <= 126)
    return &g[c - 32];
  for (int i = 95; i < GLYPHS_PER_FONT; i++)
    if (g[i].code == c)
      return &g[i];
  return &g['?' - 32];
}
static int width(int f, const char *t) {
  int n = 0;
  while (*t)
    n += glyph(f, nextchar(&t))->advance;
  return n;
}
static void text(uint16_t *p, int x, int y, int f, uint16_t c, const char *t) {
  while (*t) {
    const Glyph *g = glyph(f, nextchar(&t));
    for (int j = 0; j < g->h; j++)
      for (int i = 0; i < g->w; i++) {
        int xx = x + g->x + i, yy = y + g->y + j,
            a = font_pixels[g->offset + j * g->w + i];
        if (a && (unsigned)xx < W && (unsigned)yy < H)
          p[yy * S + xx] = mix(p[yy * S + xx], c, a);
      }
    x += g->advance;
  }
}
static void center(uint16_t *p, int x, int y, int f, uint16_t c,
                   const char *t) {
  text(p, x - width(f, t) / 2, y, f, c, t);
}
static int wrap(uint16_t *p, int x, int y, int f, int maxw, int step,
                uint16_t c, const char *t) {
  char row[180] = {0};
  int n = 0;
  const char *word = t;
  while (*word) {
    if (*word == '\n') {
      if (n) {
        row[n] = 0;
        text(p, x, y, f, c, row);
      }
      y += step;
      n = 0;
      row[0] = 0;
      word++;
      continue;
    }
    const char *end = word;
    while (*end && *end != ' ' && *end != '\n')
      end++;
    int len = (int)(end - word);
    if (n + len + 2 >= (int)sizeof(row))
      break;
    char isolated[180];
    memcpy(isolated, word, len);
    isolated[len] = 0;
    if (width(f, isolated) > maxw) {
      if (n) {
        row[n] = 0;
        text(p, x, y, f, c, row);
        y += step;
        n = 0;
        row[0] = 0;
      }
      const char *part = word;
      while (part < end) {
        const char *next = part;
        uint32_t code = nextchar(&next);
        int bytes = (int)(next - part);
        if (n && width(f, row) + glyph(f, code)->advance > maxw) {
          text(p, x, y, f, c, row);
          y += step;
          n = 0;
        }
        memcpy(row + n, part, bytes);
        n += bytes;
        row[n] = 0;
        part = next;
      }
      word = *end == ' ' ? end + 1 : end;
      continue;
    }
    char candidate[180];
    memcpy(candidate, row, n);
    int nn = n;
    if (nn)
      candidate[nn++] = ' ';
    memcpy(candidate + nn, word, len);
    candidate[nn + len] = 0;
    if (n && width(f, candidate) > maxw) {
      row[n] = 0;
      text(p, x, y, f, c, row);
      y += step;
      n = 0;
    }
    if (n)
      row[n++] = ' ';
    memcpy(row + n, word, len);
    n += len;
    row[n] = 0;
    word = *end == ' ' ? end + 1 : end;
  }
  if (n) {
    text(p, x, y, f, c, row);
    y += step;
  }
  return y;
}
static void gradient(uint16_t *p, int r0, int g0, int b0, int r1, int g1,
                     int b1) {
  for (int y = 0; y < H; y++)
    rect(p, 0, y, W, 1,
         RGB(r0 + (r1 - r0) * y / 255, g0 + (g1 - g0) * y / 255,
             b0 + (b1 - b0) * y / 255));
}
static void wall(const UI *s, uint16_t *p, int side) {
  const uint16_t *src = s->theme ? wallpaper1 : wallpaper0;
  for (int y = 0; y < H; y++)
    memcpy(p + y * S, src + y * 384 + side * 192, W * 2);
}
static void icon(uint16_t *p, int x, int y, int app, int size) {
  int o = (app - 1) * 1024;
  for (int j = 0; j < size; j++)
    for (int i = 0; i < size; i++) {
      int xx = x + i, yy = y + j, k = o + (j * 32 / size) * 32 + i * 32 / size,
          a = app_alpha[k];
      if (a && (unsigned)xx < W && (unsigned)yy < H)
        p[yy * S + xx] = mix(p[yy * S + xx], app_icons[k], a);
    }
}
static void portrait(uint16_t *p, int x, int y, int w, int h, int r) {
  for (int j = 0; j < h; j++) {
    int cut = inset(j, h, r);
    for (int i = cut; i < w - cut; i++)
      pixel(p, x + i, y + j,
            portrait_pixels[(j * 204 / h) * 164 + i * 164 / w]);
  }
}
static void status(uint16_t *p, bool light) {
  uint16_t c = light ? WHITE : BLACK;
  text(p, 10, 4, 1, c, "9:41");
  for (int i = 0; i < 3; i++)
    rect(p, 146 + i * 3, 11 - i, 2, 3 + i, c);
  rr(p, 160, 7, 20, 8, 2, c);
  rr(p, 161, 8, 17, 6, 1, light ? RGB(166, 181, 188) : WHITE);
  rect(p, 162, 9, 13, 4, c);
  rect(p, 181, 9, 1, 4, c);
}
static void homebar(uint16_t *p, bool light) {
  rr(p, 69, 251, 54, 3, 2, light ? WHITE : BLACK);
}
static void page(uint16_t *p, bool dark) {
  rect(p, 0, 0, W, H, dark ? BLACK : BG);
  status(p, dark);
  homebar(p, dark);
}
static void nav(const UI *s, uint16_t *p, const char *sub) {
  page(p, false);
  text(p, 10, 23, 2, BLUE, "‹ Home");
  int f = width(4, names[s->app]) > 172 ? 3 : 4;
  text(p, 10, 44, f, BLACK, names[s->app]);
  if (sub)
    text(p, 11, 75, 1, GRAY, sub);
}
static void detail(uint16_t *p, const char *title, const char *sub) {
  page(p, false);
  int f = width(4, title) > 172 ? 3 : 4;
  text(p, 10, 32, f, BLACK, title);
  if (sub)
    text(p, 11, 66, 0, GRAY, sub);
}
static void button(uint16_t *p, int x, int y, int w, int h, const char *t,
                   bool selected, uint16_t c) {
  rr(p, x, y, w, h, 10, selected ? RGB(224, 237, 255) : WHITE);
  center(p, x + w / 2, y + (h - 11) / 2 - 1, 2, c, t);
}
static void row(uint16_t *p, int y, const char *title, const char *sub,
                bool selected) {
  rr(p, 10, y, 172, 39, 10, selected ? RGB(224, 237, 255) : WHITE);
  text(p, 20, y + 5, 2, BLACK, title);
  if (sub)
    text(p, 20, y + 23, 0, GRAY, sub);
  text(p, 167, y + 12, 2, GRAY, "›");
}
static bool hit(Input in, int x, int y, int w, int h) {
  return in.pressed && in.x >= x && in.x < x + w && in.y >= y && in.y < y + h;
}
static void toast(UI *s, const char *t) {
  snprintf(s->toast, sizeof(s->toast), "%s", t);
  s->toastTicks = 90;
}
static void openApp(UI *s, int a) {
  s->app = a;
  s->selection = 0;
  s->event = 1;
  s->toastTicks = 0;
  s->lockDrag = 0;
}
static uint32_t random32(UI *s) {
  uint32_t x = s->rng;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  return s->rng = x;
}
static void startGame(UI *s) {
  s->gameState = 1;
  s->gameTime = 1800;
  s->gameBattery = 60;
  s->gameScore = s->gameReward = 0;
  s->playerX = 96;
  s->spawn = 25;
  memset(s->drops, 0, sizeof(s->drops));
  s->event = 2;
}
static void finishGame(UI *s) {
  s->gameState = 2;
  if (s->gameScore > s->best)
    s->best = s->gameScore;
  s->event = 3;
}
static void buy(UI *s, int n) {
  if (s->credits < prices[n]) {
    toast(s, "Can't afford air. Try Battery+.");
    s->event = 3;
    return;
  }
  s->credits -= prices[n];
  s->spent += prices[n];
  s->purchases++;
  s->event = 2;
  char b[72];
  snprintf(b, sizeof(b), "€%d in play money. Gone.", prices[n]);
  toast(s, b);
}
static void calcReset(UI *s) {
  s->calcValue = s->calcLeft = s->calcFee = 0;
  s->calcOp = 0;
  s->calcError = 0;
  s->calcFresh = 1;
}
static void calcApply(UI *s) {
  int64_t a = s->calcLeft, b = s->calcValue, v = b;
  switch (s->calcOp) {
  case '+':
    v = a + b;
    break;
  case '-':
    v = a - b;
    break;
  case '*':
    v = a * b;
    break;
  case '/':
    if (!b) {
      s->calcError = 1;
      return;
    }
    v = a / b;
    break;
  }
  if (v > 999999999LL || v < -999999999LL) {
    s->calcError = 2;
    return;
  }
  s->calcValue = v;
}
static void calcKey(UI *s, int k) {
  static const char keys[] = "789/456*123-C0=+";
  char c = keys[k];
  s->event = 1;
  if (c == 'C') {
    calcReset(s);
    return;
  }
  if (s->calcError)
    return;
  if (c >= '0' && c <= '9') {
    if (s->calcFresh) {
      s->calcValue = 0;
      s->calcFresh = 0;
      s->calcFee = 0;
    }
    if (s->calcValue <= 99999999)
      s->calcValue = s->calcValue * 10 + c - '0';
  } else if (c == '=') {
    calcApply(s);
    s->calcOp = 0;
    if (!s->calcError && s->calcPremium) {
      int64_t v = s->calcValue;
      s->calcFee = ((v < 0 ? -v : v) * 19 + 50) / 100;
      s->calcValue += s->calcFee;
      if (s->calcValue > 999999999LL)
        s->calcError = 2;
    }
    s->calcFresh = 1;
  } else {
    if (s->calcOp && !s->calcFresh)
      calcApply(s);
    s->calcLeft = s->calcValue;
    s->calcOp = c;
    s->calcFresh = 1;
  }
}
static void ask(UI *s, int n) {
  s->spariQuestion = n;
  s->spariWait = 45;
  s->spariSerial++;
  s->spariReply = n * 3 + (s->spariSerial - 1) % 3;
  s->event = 1;
}
static void setting(UI *s, int n) {
  s->settingInfo = n;
  s->event = 1;
  if (n == 0)
    s->theme ^= 1;
  if (n == 1)
    s->sound ^= 1;
  if (n == 2)
    toast(s, "Face ID detected: insufficient funds.");
  if (n == 3)
    toast(s, "Two screens. Zero subscriptions.");
}
static void noteKey(UI *s, int k) {
  static const char keys[] = "QWERTYUIOPASDFGHJKLZXCVBNM ";
  if (k == 27) {
    if (s->noteLen)
      s->note[--s->noteLen] = 0;
  } else if (s->noteLen < 80) {
    s->note[s->noteLen++] = keys[k];
    s->note[s->noteLen] = 0;
  }
  s->event = 1;
}
static void dialKey(UI *s, int k) {
  static const char keys[] = "123456789*0#";
  if (k == 12) {
    s->phoneCalling ^= 1;
    s->event = 2;
  } else if (s->dialLen < 16) {
    s->dial[s->dialLen++] = keys[k];
    s->dial[s->dialLen] = 0;
    s->event = 1;
  }
}
void ui_init(UI *s) {
  memset(s, 0, sizeof(*s));
  s->app = LOCK;
  s->sound = 1;
  s->credits = 2000;
  s->rng = 0x44f04d21;
  s->spariQuestion = s->spariReply = -1;
  s->calcFresh = 1;
  s->playerX = 96;
  s->calendarDay = 30;
  strcpy(s->note, "Keep the DS.");
  s->noteLen = (int)strlen(s->note);
}
int ui_take_sound(UI *s) {
  int e = s->event;
  s->event = 0;
  return s->sound ? e : 0;
}
void ui_tick(UI *s, Input in) {
  s->frame++;
  s->event = 0;
  if (s->toastTicks)
    s->toastTicks--;
  if (s->flash)
    s->flash--;
  if (s->spariWait)
    s->spariWait--;
  if (s->clockRunning && s->clockTicks < 359999)
    s->clockTicks++;
  if (s->musicPlaying) {
    s->musicTicks = (s->musicTicks + 1) % 216000;
    if (s->musicTicks % 15 == 0)
      s->event = 4 + (s->musicTicks / 15 + s->musicTrack * 2) % 8;
  }
  if (in.down & DO_START) {
    s->app = LOCK;
    s->lockDrag = 0;
    s->lockX = 0;
    s->toastTicks = 0;
    s->event = 1;
    return;
  }
  if (s->app == LOCK) {
    if (in.down & DO_A) {
      openApp(s, HOME);
      return;
    }
    if (hit(in, 0, 194, 192, 62)) {
      s->lockDrag = 1;
      s->touchStartY = in.y;
    }
    if (s->lockDrag && in.touch) {
      s->lockX = clamp(s->touchStartY - in.y, 0, 100);
      if (s->lockX >= 50)
        openApp(s, HOME);
    }
    if (in.released) {
      s->lockDrag = 0;
      s->lockX = 0;
    }
    return;
  }
  if ((in.down & DO_B) || (s->app > HOME && hit(in, 0, 19, 65, 22))) {
    openApp(s, HOME);
    return;
  }
  if (in.pressed && in.y >= 245) {
    s->lockDrag = 2;
    s->touchStartY = in.y;
  }
  if (s->lockDrag == 2 && in.touch && s->touchStartY - in.y >= 35) {
    openApp(s, HOME);
    return;
  }
  if (in.released)
    s->lockDrag = 0;
  if (s->app == HOME) {
    int n = s->selection;
    if (n < 12) {
      int r = n / 3, c = n % 3;
      if (in.down & DO_UP)
        r = (r + 3) % 4;
      if (in.down & DO_DOWN)
        r = (r + 1) % 4;
      if (in.down & DO_LEFT)
        c = (c + 2) % 3;
      if (in.down & DO_RIGHT) {
        if (c == 2)
          n = 12 + r;
        else
          c++;
      }
      if (n < 12)
        n = r * 3 + c;
    } else {
      if (in.down & DO_UP)
        n = 12 + (n - 12 + 3) % 4;
      if (in.down & DO_DOWN)
        n = 12 + (n - 12 + 1) % 4;
      if (in.down & DO_LEFT)
        n = (n - 12) * 3 + 2;
      if (in.down & DO_RIGHT)
        n = (n - 12) * 3;
    }
    s->selection = n;
    if (in.down & DO_Y) {
      s->theme ^= 1;
      s->event = 1;
    }
    if (in.down & DO_A) {
      openApp(s, n < 12 ? ui_home_apps[n] : dockApps[n - 12]);
      return;
    }
    if (hit(in, 8, 25, 65, 59)) {
      openApp(s, WEATHER);
      return;
    }
    if (hit(in, 82, 25, 65, 59)) {
      openApp(s, MAPS);
      return;
    }
    for (int i = 0; i < 12; i++)
      if (hit(in, 4 + (i % 3) * 47, 92 + (i / 3) * 38, 43, 37)) {
        openApp(s, ui_home_apps[i]);
        return;
      }
    for (int i = 0; i < 4; i++)
      if (hit(in, 151, 81 + i * 38, 39, 37)) {
        openApp(s, dockApps[i]);
        return;
      }
    return;
  }
  if (s->app == SPARI) {
    if (in.down & DO_UP)
      s->selection = (s->selection + 2) % 3;
    if (in.down & DO_DOWN)
      s->selection = (s->selection + 1) % 3;
    if (in.down & DO_A)
      ask(s, s->selection);
    for (int i = 0; i < 3; i++)
      if (hit(in, 10, 100 + i * 43, 172, 38)) {
        s->selection = i;
        ask(s, i);
      }
    return;
  }
  if (s->app == CAMERA) {
    if ((in.down & DO_X) || hit(in, 139, 207, 45, 37)) {
      s->filter = (s->filter + 1) % 3;
      s->event = 1;
    }
    if ((in.down & DO_A) || hit(in, 68, 201, 56, 44)) {
      s->photos++;
      s->flash = 12;
      s->event = 2;
      toast(s, "48 pixels. Please do not zoom in.");
    }
    if (hit(in, 10, 207, 39, 37))
      openApp(s, PHOTOS);
    return;
  }
  if (s->app == STORE) {
    if (in.down & DO_UP)
      s->selection = (s->selection + 2) % 3;
    if (in.down & DO_DOWN)
      s->selection = (s->selection + 1) % 3;
    if (in.down & DO_A)
      buy(s, s->selection);
    for (int i = 0; i < 3; i++)
      if (hit(in, 10, 94 + i * 47, 172, 43)) {
        s->selection = i;
        buy(s, i);
      }
    return;
  }
  if (s->app == BATTERY) {
    if (s->gameState != 1) {
      if ((in.down & DO_A) || hit(in, 18, 190, 156, 44))
        startGame(s);
      return;
    }
    if (in.held & DO_LEFT)
      s->playerX -= 4;
    if (in.held & DO_RIGHT)
      s->playerX += 4;
    if (in.touch && in.y > 49)
      s->playerX = in.x;
    s->playerX = clamp(s->playerX, 23, 169);
    s->gameTime--;
    if (s->gameTime % 60 == 0)
      s->gameBattery--;
    if (--s->spawn <= 0) {
      for (int i = 0; i < 9; i++)
        if (!s->drops[i].alive) {
          s->drops[i] = (Drop){18 + (int)(random32(s) % 156), 60,
                               (random32(s) % 4) == 0, 1};
          break;
        }
      s->spawn = 37;
    }
    for (int i = 0; i < 9; i++) {
      Drop *d = &s->drops[i];
      if (!d->alive)
        continue;
      d->y += s->gameTime < 900 ? 2 : 1;
      if (d->y >= 216) {
        d->alive = 0;
        if (abs(d->x - s->playerX) < 25) {
          if (d->kind) {
            s->gameBattery -= 14;
            s->event = 3;
          } else {
            s->gameBattery = clamp(s->gameBattery + 12, 0, 100);
            s->gameScore += 10;
            s->credits += 8;
            s->gameReward += 8;
            s->event = 2;
          }
        } else if (!d->kind)
          s->gameBattery -= 4;
      }
    }
    s->gameBattery = clamp(s->gameBattery, 0, 100);
    if (!s->gameBattery || s->gameTime <= 0)
      finishGame(s);
    return;
  }
  if (s->app == MESSAGES) {
    if (in.down & DO_UP)
      s->selection = (s->selection + 2) % 3;
    if (in.down & DO_DOWN)
      s->selection = (s->selection + 1) % 3;
    if (in.down & DO_A) {
      s->contact = s->selection;
      s->event = 1;
    }
    for (int i = 0; i < 3; i++)
      if (hit(in, 6, 97 + i * 47, 180, 43)) {
        s->contact = s->selection = i;
        s->event = 1;
      }
    return;
  }
  if (s->app == WEATHER) {
    if (in.down & DO_UP)
      s->forecast = (s->forecast + 2) % 3;
    if (in.down & (DO_DOWN | DO_A))
      s->forecast = (s->forecast + 1) % 3;
    for (int i = 0; i < 3; i++)
      if (hit(in, 10, 100 + i * 43, 172, 38)) {
        s->forecast = i;
        s->event = 1;
      }
    return;
  }
  if (s->app == CALC) {
    if (in.down & DO_LEFT)
      s->calcCursor = (s->calcCursor + 15) % 16;
    if (in.down & DO_RIGHT)
      s->calcCursor = (s->calcCursor + 1) % 16;
    if (in.down & DO_UP)
      s->calcCursor = (s->calcCursor + 12) % 16;
    if (in.down & DO_DOWN)
      s->calcCursor = (s->calcCursor + 4) % 16;
    if (in.down & DO_A)
      calcKey(s, s->calcCursor);
    if (in.down & DO_X)
      calcReset(s);
    if ((in.down & DO_Y) || hit(in, 104, 20, 81, 24)) {
      s->calcPremium ^= 1;
      s->event = 1;
    }
    for (int i = 0; i < 16; i++)
      if (hit(in, 10 + (i % 4) * 44, 80 + (i / 4) * 42, 38, 38)) {
        s->calcCursor = i;
        calcKey(s, i);
      }
    return;
  }
  if (s->app == SETTINGS) {
    if (in.down & DO_UP)
      s->selection = (s->selection + 3) % 4;
    if (in.down & DO_DOWN)
      s->selection = (s->selection + 1) % 4;
    if (in.down & DO_A)
      setting(s, s->selection);
    for (int i = 0; i < 4; i++)
      if (hit(in, 10, 94 + i * 36, 172, 32)) {
        s->selection = i;
        setting(s, i);
      }
    return;
  }
  if (s->app == PHOTOS) {
    if ((in.down & DO_A) || hit(in, 10, 171, 172, 30)) {
      openApp(s, CAMERA);
      return;
    }
    if ((in.down & DO_X) || hit(in, 10, 210, 172, 27)) {
      s->filter = (s->filter + 1) % 3;
      s->event = 1;
    }
    return;
  }
  if (s->app == CALENDAR) {
    if (in.down & DO_LEFT)
      s->calendarDay--;
    if (in.down & DO_RIGHT)
      s->calendarDay++;
    if (in.down & DO_UP)
      s->calendarDay -= 7;
    if (in.down & DO_DOWN)
      s->calendarDay += 7;
    s->calendarDay = (s->calendarDay + 30) % 31 + 1;
    for (int d = 1; d <= 31; d++) {
      int n = d + 2;
      if (hit(in, 12 + (n % 7) * 24, 112 + (n / 7) * 23, 23, 22)) {
        s->calendarDay = d;
        s->event = 1;
      }
    }
    return;
  }
  if (s->app == NOTES) {
    if (in.down & DO_LEFT)
      s->noteCursor = (s->noteCursor + 27) % 28;
    if (in.down & DO_RIGHT)
      s->noteCursor = (s->noteCursor + 1) % 28;
    if (in.down & DO_UP)
      s->noteCursor = (s->noteCursor + 18) % 28;
    if (in.down & DO_DOWN)
      s->noteCursor = (s->noteCursor + 10) % 28;
    if (in.down & DO_A)
      noteKey(s, s->noteCursor);
    if (in.down & DO_X) {
      s->noteLen = 0;
      s->note[0] = 0;
      s->event = 1;
    }
    for (int i = 0; i < 26; i++) {
      int x = i < 10   ? 5 + i * 18
              : i < 19 ? 14 + (i - 10) * 18
                       : 23 + (i - 19) * 18;
      int y = i < 10 ? 160 : i < 19 ? 183 : 206;
      if (hit(in, x, y, 17, 21)) {
        s->noteCursor = i;
        noteKey(s, i);
      }
    }
    if (hit(in, 42, 230, 105, 17))
      noteKey(s, 26);
    if (hit(in, 151, 206, 35, 21))
      noteKey(s, 27);
    return;
  }
  if (s->app == MAPS) {
    if ((in.down & DO_A) || hit(in, 10, 213, 172, 31)) {
      s->mapRoute ^= 1;
      s->event = 1;
    }
    return;
  }
  if (s->app == PHONE) {
    if (in.down & DO_LEFT)
      s->dialCursor = (s->dialCursor + 12) % 13;
    if (in.down & DO_RIGHT)
      s->dialCursor = (s->dialCursor + 1) % 13;
    if (in.down & DO_UP)
      s->dialCursor = (s->dialCursor + 10) % 13;
    if (in.down & DO_DOWN)
      s->dialCursor = (s->dialCursor + 3) % 13;
    if (in.down & DO_A)
      dialKey(s, s->dialCursor);
    if (in.down & DO_Y)
      dialKey(s, 12);
    if (in.down & DO_X) {
      if (s->dialLen)
        s->dial[--s->dialLen] = 0;
    }
    for (int i = 0; i < 12; i++)
      if (hit(in, 18 + (i % 3) * 59, 66 + (i / 3) * 39, 38, 37)) {
        s->dialCursor = i;
        dialKey(s, i);
      }
    if (hit(in, 73, 222, 46, 27))
      dialKey(s, 12);
    return;
  }
  if (s->app == SAFARI) {
    if (in.down & DO_UP)
      s->selection = (s->selection + 2) % 3;
    if (in.down & DO_DOWN)
      s->selection = (s->selection + 1) % 3;
    if (in.down & DO_A) {
      s->browserPage = s->selection;
      s->event = 1;
    }
    for (int i = 0; i < 3; i++)
      if (hit(in, 10, 163 + i * 27, 172, 24)) {
        s->browserPage = s->selection = i;
        s->event = 1;
      }
    return;
  }
  if (s->app == MUSIC) {
    if ((in.down & DO_A) || hit(in, 66, 130, 60, 60)) {
      s->musicPlaying ^= 1;
      s->event = s->musicPlaying ? 4 : 1;
    }
    if ((in.down & DO_LEFT) || hit(in, 13, 142, 36, 40)) {
      s->musicTrack = (s->musicTrack + 2) % 3;
      s->musicTicks = 0;
      s->event = 1;
    }
    if ((in.down & DO_RIGHT) || hit(in, 143, 142, 36, 40)) {
      s->musicTrack = (s->musicTrack + 1) % 3;
      s->musicTicks = 0;
      s->event = 1;
    }
    return;
  }
  if (s->app == CLOCK) {
    if ((in.down & DO_A) || hit(in, 104, 174, 72, 47)) {
      s->clockRunning ^= 1;
      s->event = 1;
    }
    if ((in.down & DO_X) || hit(in, 16, 174, 72, 47)) {
      s->clockTicks = 0;
      s->event = 1;
    }
    return;
  }
}
static void mapview(uint16_t *p, int x, int y, int w, int h, int route) {
  rr(p, x, y, w, h, 11, RGB(227, 235, 214));
  rect(p, x + 5, y + h / 4, w / 3, h / 3, RGB(183, 218, 158));
  rect(p, x + w * 2 / 3, y + h / 2, w / 4, h / 4, RGB(185, 216, 155));
  for (int k = 0; k < 3; k++) {
    int xx = x + w * (k + 1) / 4;
    rect(p, xx, y + 3, 4, h - 6, WHITE);
    int yy = y + h * (k + 1) / 4;
    rect(p, x + 3, yy, w - 6, 4, WHITE);
  }
  for (int d = -2; d <= 2; d++) {
    line(p, x + 6, y + h - 9 + d, x + w - 9, y + 11 + d, RGB(243, 201, 90));
  }
  if (route) {
    for (int d = -1; d <= 1; d++) {
      line(p, x + 11 + d, y + h - 15, x + w / 2 + d, y + h - 15, BLUE);
      line(p, x + w / 2 + d, y + h - 15, x + w / 2 + d, y + h / 3, BLUE);
      line(p, x + w / 2, y + h / 3 + d, x + w - 18, y + h / 3 + d, BLUE);
    }
  }
  int px = x + w - 17, py = y + h / 3;
  circle(p, px, py, 5, WHITE);
  circle(p, px, py, 3, BLUE);
}
static void photo48(const UI *s, uint16_t *p, int x, int y, int w, int h) {
  for (int j = 0; j < 6; j++)
    for (int i = 0; i < 8; i++) {
      uint16_t c =
          portrait_pixels[(j * 204 / 6 + 17) * 164 + (i * 164 / 8 + 10)];
      int r = (c & 31) * 8, g = ((c >> 5) & 31) * 8, b = ((c >> 10) & 31) * 8;
      if (s->filter == 1) {
        r = clamp(r + 32, 0, 255);
        g = clamp(g + 10, 0, 255);
      }
      if (s->filter == 2)
        r = g = b = (r + g + b) / 3;
      rect(p, x + i * w / 8, y + j * h / 6, (i + 1) * w / 8 - i * w / 8,
           (j + 1) * h / 6 - j * h / 6, RGB(r, g, b));
    }
}
static void battery(uint16_t *p, int x, int y, int w, int h, int value,
                    uint16_t c) {
  rr(p, x, y, w, h, 6, LINE);
  rr(p, x + 3, y + 3, (w - 6) * clamp(value, 0, 100) / 100, h - 6, 3, c);
  rect(p, x + w, y + h / 3, 3, h / 3, LINE);
}
static void orb(const UI *s, uint16_t *p, int x, int y) {
  circle(p, x, y, 28, RGB(238, 231, 253));
  circle(p, x - 4, y - 3, 21, RGB(157, 159, 255));
  circle(p, x + 6, y + 4, 17, RGB(244, 158, 216));
  circle(p, x - 5, y + 7, 12, RGB(137, 225, 245));
  for (int i = -16; i <= 16; i += 3) {
    int h = 3 + (abs(i) + (s->frame / 6) % 9) % 11;
    rect(p, x + i, y - h / 2, 1, h, WHITE);
  }
}
static void homeLeft(const UI *s, uint16_t *p) {
  wall(s, p, 0);
  status(p, false);
  portrait(p, 14, 29, 164, 204, 13);
  for (int j = 150; j < 204; j++) {
    int cut = inset(j, 204, 13);
    for (int i = cut; i < 164 - cut; i++) {
      int pos = (29 + j) * S + 14 + i;
      p[pos] = mix(p[pos], BLACK, (j - 150) * 11 / 54);
    }
  }
  text(p, 24, 193, 3, WHITE, "On This Day");
  text(p, 24, 215, 0, WHITE, "Two screens. Since 2004.");
  circle(p, 163, 217, 7, WHITE);
  for (int i = 0; i < 5; i++)
    line(p, 161 + i, 214 + i / 2, 161 + i, 220 - i / 2, GRAY);
  circle(p, 89, 245, 2, WHITE);
  circle(p, 101, 245, 2, RGB(209, 205, 193));
}
static void homeRight(const UI *s, uint16_t *p) {
  wall(s, p, 1);
  status(p, false);
  rr(p, 8, 25, 65, 59, 11, RGB(55, 123, 180));
  text(p, 14, 30, 0, WHITE, "Cupertino");
  text(p, 13, 41, 4, WHITE, "22°");
  circle(p, 58, 66, 4, RGB(255, 214, 71));
  text(p, 14, 69, 0, WHITE, "Sunny. Pricey.");
  mapview(p, 82, 25, 65, 59, 1);
  rr(p, 86, 64, 57, 16, 5, WHITE);
  text(p, 91, 68, 0, BLACK, "To the Store");
  glass(p, 151, 78, 36, 160, 15, WHITE, 6);
  for (int n = 0; n < 12; n++) {
    int x = 10 + (n % 3) * 47, y = 95 + (n / 3) * 38, a = ui_home_apps[n];
    if (s->selection == n)
      glass(p, x - 2, y - 2, 32, 32, 9, WHITE, 7);
    icon(p, x, y, a, 28);
    const char *t = shortNames[a];
    int xx = x + 14 - width(0, t) / 2;
    text(p, xx + 1, y + 30, 0, RGB(103, 94, 81), t);
    text(p, xx, y + 29, 0, WHITE, t);
  }
  for (int n = 0; n < 4; n++) {
    int y = 86 + n * 38;
    if (s->selection == 12 + n)
      glass(p, 153, y - 2, 32, 32, 9, WHITE, 10);
    icon(p, 155, y, dockApps[n], 28);
  }
  homebar(p, true);
}
static void lockLeft(const UI *s, uint16_t *p) {
  wall(s, p, 0);
  status(p, true);
  center(p, 96, 39, 2, WHITE, "Friday, October 30");
  center(p, 97, 61, 6, RGB(123, 130, 133), "9:41");
  center(p, 96, 60, 6, WHITE, "9:41");
  glass(p, 14, 170, 164, 61, 14, WHITE, 5);
  text(p, 25, 181, 2, WHITE, "iPhone Dou");
  wrap(p, 25, 201, 1, 142, 13, WHITE, "Two screens. One loan.");
}
static void lockRight(const UI *s, uint16_t *p) {
  wall(s, p, 1);
  status(p, true);
  rr(p, 89, 38, 14, 13, 3, WHITE);
  rr(p, 92, 32, 8, 12, 4, WHITE);
  rr(p, 94, 34, 4, 7, 2, RGB(138, 160, 174));
  glass(p, 10, 92, 172, 67, 13, WHITE, 10);
  icon(p, 20, 102, MESSAGES, 21);
  text(p, 49, 103, 1, BLACK, "Notifications");
  text(p, 49, 118, 0, GRAY, "now");
  wrap(p, 20, 134, 1, 151, 12, BLACK, "Your DS did this years ago.");
  center(p, 96, 190 - s->lockX / 3, 2, WHITE, "↑");
  center(p, 96, 213, 1, WHITE, "Swipe up to open");
  center(p, 96, 233, 0, WHITE, "or press A");
  homebar(p, true);
}
static void albumArt(uint16_t *p, int x, int y, int w) {
  rr(p, x, y, w, w, 13, RGB(248, 66, 104));
  for (int j = 0; j < w - 20; j += 5)
    line(p, x + 10, y + 10 + j, x + w - 11, y + 10 + j,
         RGB(247, 86 + j / 3, 127));
  int cx = x + w / 2, cy = y + w / 2;
  rect(p, cx - 4, cy - 30, 5, 57, WHITE);
  rect(p, cx + 28, cy - 36, 5, 54, WHITE);
  for (int j = 0; j < 6; j++)
    line(p, cx - 4, cy - 30 + j, cx + 32, cy - 36 + j, WHITE);
  circle(p, cx - 11, cy + 27, 10, WHITE);
  circle(p, cx + 21, cy + 18, 10, WHITE);
}
void ui_render_top(const UI *s, uint16_t *p) {
  char b[120];
  if (s->app == HOME) {
    homeLeft(s, p);
    return;
  }
  if (s->app == LOCK) {
    lockLeft(s, p);
    return;
  }
  if (s->app == SPARI) {
    detail(p, "Spari", "ARTIFICIAL ARROGANCE");
    orb(s, p, 96, 102);
    if (s->spariQuestion < 0) {
      center(p, 96, 147, 3, BLACK, "Go ahead. Ask.");
      wrap(p, 18, 175, 2, 156, 16, GRAY,
           "I don't know. But I can charge more for it.");
    } else {
      center(p, 96, 141, 2, BLUE, questions[s->spariQuestion]);
      wrap(p, 14, 166, 2, 164, 15, BLACK,
           s->spariWait ? "Thinking about your data …"
                        : answers[s->spariReply]);
    }
    center(p, 96, 237, 0, GRAY, "Offline. With confidence.");
    return;
  }
  if (s->app == CAMERA) {
    page(p, true);
    text(p, 12, 27, 2, WHITE, "48 PIXELS  ·  DOU RAW");
    photo48(s, p, 10, 54, 172, 160);
    if (s->flash > 9)
      rect(p, 10, 54, 172, 160, WHITE);
    center(p, 96, 225, 1, WHITE, "Every pixel makes a statement.");
    center(p, 96, 239, 0, GRAY, "Simulated. Not a real camera.");
    return;
  }
  if (s->app == STORE) {
    detail(p, "For You", "TODAY IN THE APP STORE");
    rr(p, 10, 86, 172, 106, 13, RGB(31, 115, 238));
    icon(p, 78, 98, STORE, 36);
    center(p, 96, 146, 3, WHITE, "Less. For more.");
    center(p, 96, 171, 1, WHITE, "Designed in Marketing.");
    snprintf(b, sizeof(b), "€%d", s->spent);
    text(p, 11, 204, 4, BLACK, b);
    snprintf(b, sizeof(b), "Pointless upgrades: %d", s->purchases);
    text(p, 12, 234, 1, GRAY, b);
    return;
  }
  if (s->app == BATTERY) {
    detail(p, "Battery", "CHARGER SOLD SEPARATELY");
    int v = s->gameState ? s->gameBattery : 60;
    snprintf(b, sizeof(b), "%d%%", v);
    center(p, 96, 90, 6, v < 25 ? RED : BLACK, b);
    battery(p, 22, 153, 145, 24, v, v < 25 ? RED : GREEN);
    if (s->gameState == 1) {
      snprintf(b, sizeof(b), "%d points · %d s", s->gameScore,
               (s->gameTime + 59) / 60);
      center(p, 96, 196, 3, BLACK, b);
      center(p, 96, 227, 1, GRAY, "Catch green. Dodge red.");
    } else if (s->gameState == 2) {
      center(p, 96, 187, 3, BLACK, v ? "Battery saved." : "Battery empty.");
      snprintf(b, sizeof(b), "%d points · +€%d", s->gameScore, s->gameReward);
      center(p, 96, 215, 2, BLUE, b);
      center(p, 96, 238, 0, GRAY, "The profit margin lives on.");
    } else {
      center(p, 96, 192, 3, BLACK, "Battery anxiety?");
      wrap(p, 20, 219, 1, 152, 13, GRAY,
           "30 seconds against the business model.");
    }
    return;
  }
  if (s->app == MESSAGES) {
    static const char *who[] = {"Support", "Your Bank", "Mom"};
    static const char *msg[] = {
        "Two screens? Please buy a license for the second one first.",
        "New phone detected. We ordered tissues just in case.",
        "Your DS already did that. Since when is opening a hinge innovation?"};
    static const char *reply[] = {"My DSi calls that Tuesday.",
                                  "It's all play money. Promise.",
                                  "Since they started charging €1,499."};
    page(p, false);
    circle(p, 96, 49, 17, RGB(185, 190, 201));
    center(p, 96, 39, 3, WHITE,
           s->contact == 0   ? "S"
           : s->contact == 1 ? "B"
                             : "M");
    center(p, 96, 71, 2, BLACK, who[s->contact]);
    rr(p, 9, 98, 163, 84, 13, RGB(225, 225, 230));
    wrap(p, 19, 110, 2, 143, 15, BLACK, msg[s->contact]);
    rr(p, 25, 195, 158, 42, 13, BLUE);
    wrap(p, 36, 202, 1, 136, 13, WHITE, reply[s->contact]);
    return;
  }
  if (s->app == WEATHER) {
    static const char *temp[] = {"22°", "19°", "41°"};
    static const char *conditions[] = {"Sunny", "Cloudy budget",
                                       "Battery heatwave"};
    gradient(p, 43, 133, 204, 123, 181, 220);
    status(p, true);
    center(p, 96, 38, 3, WHITE, "Cupertino");
    center(p, 96, 61, 6, WHITE, temp[s->forecast]);
    center(p, 96, 119, 2, WHITE, conditions[s->forecast]);
    center(p, 96, 139, 1, WHITE, "H: 49°  L: your balance");
    glass(p, 10, 174, 172, 63, 13, WHITE, 4);
    wrap(p, 22, 186, 2, 148, 16, WHITE,
         s->forecast == 0   ? "Feels like 49 degrees. The CPU calls it passion."
         : s->forecast == 1 ? "Cloudy with a chance of a new subscription."
                            : "Light headwinds. Mostly for the battery.");
    homebar(p, true);
    return;
  }
  if (s->app == CALC) {
    detail(p, "Added Value", "IT ALL ADDS UP. FOR US.");
    icon(p, 76, 88, CALC, 40);
    wrap(p, 13, 149, 2, 166, 16, BLACK,
         s->calcPremium ? "Press = to add a 19 percent luxury surcharge."
                        : "It actually calculates. Almost bad for business.");
    snprintf(b, sizeof(b), "Surcharge: %lld", (long long)s->calcFee);
    text(p, 13, 214, 2, ORANGE, b);
    text(p, 13, 238, 0, GRAY, "Integers. Division rounds toward zero.");
    return;
  }
  if (s->app == SETTINGS) {
    detail(p, "About Dou", "DOU OS 2.1 / UNOFFICIAL PARODY");
    rr(p, 10, 91, 172, 60, 12, WHITE);
    icon(p, 22, 102, SETTINGS, 36);
    text(p, 70, 103, 3, BLACK, "iPhone Dou");
    text(p, 70, 126, 0, GRAY, "Think twice.");
    text(p, 12, 167, 2, BLACK, "2 screens. 0 subscriptions.");
    wrap(p, 12, 194, 1, 168, 14, GRAY,
         s->settingInfo == 2 ? "Face ID recognizes your face. Especially your "
                               "willingness to pay."
                             : "Turn the DS 90° left. Touch on the right. B: "
                               "Home. START: lock.");
    return;
  }
  if (s->app == PHOTOS) {
    page(p, false);
    if (s->photos) {
      text(p, 10, 28, 3, BLACK, "Latest Capture");
      photo48(s, p, 10, 62, 172, 153);
      center(p, 96, 228, 1, GRAY, "48 pixels. Each one paid for.");
    } else {
      portrait(p, 14, 29, 164, 204, 13);
      glass(p, 21, 199, 150, 25, 8, BLACK, 7);
      center(p, 96, 204, 1, WHITE, "On This Day · 2004");
    }
    return;
  }
  if (s->app == CALENDAR) {
    detail(p, "October 2026", "YOUR EVENTS ARE ENTIRELY FICTIONAL");
    snprintf(b, sizeof(b), "%d", s->calendarDay);
    center(p, 96, 86, 6, RED, b);
    rr(p, 10, 153, 172, 40, 10, WHITE);
    rect(p, 19, 161, 3, 24, RED);
    text(p, 29, 160, 2, BLACK, "Nothing-new keynote");
    text(p, 29, 178, 0, GRAY, "09:41 · circular thinking");
    rr(p, 10, 201, 172, 38, 10, WHITE);
    rect(p, 19, 209, 3, 23, BLUE);
    text(p, 29, 208, 2, BLACK, "Installment plan");
    text(p, 29, 225, 0, GRAY, "All day. Naturally.");
    return;
  }
  if (s->app == NOTES) {
    detail(p, "My Note", "STORED IN MEMORY ONLY");
    line(p, 10, 86, 182, 86, LINE);
    wrap(p, 12, 103, 3, 168, 21, BLACK,
         s->note[0] ? s->note : "A surprisingly empty idea.");
    return;
  }
  if (s->app == MAPS) {
    detail(p, s->mapRoute ? "You're Home" : "To the Store",
           "DEMO ROUTE / NO GPS");
    if (!s->mapRoute) {
      for (int d = -3; d <= 3; d++) {
        line(p, 97 + d, 150, 97 + d, 99, BLUE);
        line(p, 97, 99 + d, 45, 99 + d, BLUE);
      }
      for (int j = 0; j < 16; j++)
        line(p, 45 + j, 99 - j / 2, 45 + j, 99 + j / 2, BLUE);
    } else {
      circle(p, 96, 119, 27, GREEN);
      text(p, 78, 103, 4, WHITE, "OK");
    }
    wrap(p, 15, 173, 3, 162, 20, BLACK,
         s->mapRoute ? "Stay here. It costs nothing."
                     : "Left in 800 m. Your money stays here.");
    text(p, 15, 233, 0, GRAY, "Navigation with a financial detour.");
    return;
  }
  if (s->app == PHONE) {
    page(p, false);
    center(p, 96, 37, 3, BLACK, "Premium Hotline");
    center(p, 96, 75, s->dialLen > 13 ? 2 : 3, BLACK,
           s->dialLen ? s->dial : "No number");
    circle(p, 96, 136, 34, s->phoneCalling ? GREEN : RGB(220, 223, 231));
    icon(p, 78, 118, PHONE, 36);
    wrap(p, 20, 186, 2, 152, 17, GRAY,
         s->phoneCalling
             ? "Connecting your wallet only. This is a simulated call."
             : "For real conversations, please use a real phone.");
    return;
  }
  if (s->app == SAFARI) {
    static const char *heads[] = {"What's new?", "From €1,499", "Privacy"};
    static const char *copy[] = {"The name. The price. The presentation. Your "
                                 "DSi quietly opens its hinge.",
                                 "Two screens included. Content, charger and "
                                 "financial peace of mind sold separately.",
                                 "We collect no data here. That would take "
                                 "more work than the entire revolution."};
    detail(p, "dou.local", "SAFARI / LOCAL DEMO PAGE");
    text(p, 12, 96, 4, BLACK, heads[s->browserPage]);
    wrap(p, 12, 139, 2, 168, 17, BLACK, copy[s->browserPage]);
    text(p, 12, 237, 0, GRAY, "Offline. No external websites.");
    return;
  }
  if (s->app == MUSIC) {
    page(p, false);
    albumArt(p, 14, 29, 164);
    center(p, 96, 207, 3, BLACK, tracks[s->musicTrack]);
    center(p, 96, 232, 1, GRAY, "The Accessoryless");
    return;
  }
  if (s->app == CLOCK) {
    detail(p, "World Clock", "CUPERTINO / 9:41 / DEMO");
    circle(p, 96, 154, 67, WHITE);
    static const int dx[] = {0, 32, 55, 64, 55, 32, 0, -32, -55, -64, -55, -32},
                     dy[] = {-64, -55, -32, 0, 32, 55, 64, 55, 32, 0, -32, -55};
    for (int i = 0; i < 12; i++)
      line(p, 96 + dx[i] * 9 / 10, 154 + dy[i] * 9 / 10, 96 + dx[i],
           154 + dy[i], BLACK);
    for (int d = -1; d <= 1; d++) {
      line(p, 96, 154 + d, 60, 145 + d, BLACK);
      line(p, 96 + d, 154, 51 + d, 181, BLACK);
    }
    line(p, 96, 154, 107, 99, RED);
    circle(p, 96, 154, 3, BLACK);
    return;
  }
}
static void gameRight(const UI *s, uint16_t *p) {
  page(p, true);
  text(p, 10, 23, 2, BLUE, "‹ Home");
  text(p, 10, 45, 3, WHITE, "Battery+");
  if (s->gameState != 1) {
    battery(p, 55, 95, 79, 34, s->gameState == 2 ? s->gameBattery : 60, GREEN);
    center(p, 96, 149, 2, WHITE,
           s->gameState == 2 ? "Charger still sold separately."
                             : "Catch the chargers.");
    center(p, 96, 169, 1, GRAY, "Stylus or left / right");
    button(p, 18, 194, 156, 35, s->gameState == 2 ? "Suffer again" : "Start",
           true, BLUE);
    return;
  }
  for (int y = 69; y < 214; y += 18)
    line(p, 10, y, 182, y, RGB(40, 41, 45));
  for (int i = 0; i < 9; i++) {
    const Drop *d = &s->drops[i];
    if (!d->alive)
      continue;
    if (d->kind) {
      rr(p, d->x - 6, d->y - 8, 12, 17, 3, RED);
      rr(p, d->x - 4, d->y - 6, 8, 13, 2, BLACK);
    } else {
      rr(p, d->x - 7, d->y - 6, 14, 13, 3, GREEN);
      rect(p, d->x - 4, d->y - 10, 2, 5, GREEN);
      rect(p, d->x + 2, d->y - 10, 2, 5, GREEN);
      rect(p, d->x - 1, d->y + 7, 3, 4, GREEN);
    }
  }
  rr(p, s->playerX - 22, 217, 44, 24, 7, WHITE);
  rr(p, s->playerX - 18, 220, 36, 15, 4, BLACK);
  rect(p, s->playerX - 8, 225, 16, 4, GREEN);
  rect(p, s->playerX - 3, 238, 6, 1, BLACK);
}
void ui_render_bottom(const UI *s, uint16_t *p) {
  char b[120];
  if (s->app == HOME) {
    homeRight(s, p);
    return;
  }
  if (s->app == LOCK) {
    lockRight(s, p);
    return;
  }
  if (s->app == SPARI) {
    nav(s, p, "Your assistant has sales targets");
    for (int i = 0; i < 3; i++)
      row(p, 100 + i * 43, questions[i],
          i == 0   ? "A financial question."
          : i == 1 ? "A very short answer."
                   : "A dangerous wish.",
          s->selection == i);
    center(p, 96, 237, 0, GRAY, "The answer appears on the left.");
  } else if (s->app == CAMERA) {
    page(p, true);
    text(p, 10, 23, 2, WHITE, "‹ Home");
    center(p, 96, 47, 0, WHITE, "48 PIXELS · PRO MAX");
    photo48(s, p, 10, 70, 172, 111);
    if (s->flash > 9)
      rect(p, 10, 70, 172, 111, WHITE);
    for (int x = 1; x < 3; x++)
      line(p, 10 + x * 172 / 3, 70, 10 + x * 172 / 3, 180, RGB(204, 204, 204));
    for (int y = 1; y < 3; y++)
      line(p, 10, 70 + y * 111 / 3, 181, 70 + y * 111 / 3, RGB(204, 204, 204));
    static const char *filters[] = {"ORIGINAL", "PRICEY", "NOSTALGIA"};
    center(p, 96, 190, 0, ORANGE, filters[s->filter]);
    circle(p, 96, 223, 20, WHITE);
    circle(p, 96, 223, 17, BLACK);
    circle(p, 96, 223, 15, WHITE);
    portrait(p, 14, 210, 26, 26, 5);
    center(p, 161, 217, 3, WHITE, "↻");
  } else if (s->app == STORE) {
    nav(s, p, "All play money. All overpriced.");
    for (int i = 0; i < 3; i++) {
      int y = 94 + i * 47;
      rr(p, 10, y, 172, 43, 10, s->selection == i ? RGB(224, 237, 255) : WHITE);
      text(p, 19, y + 5, 2, BLACK, products[i]);
      text(p, 19, y + 24, 0, GRAY, productSub[i]);
      rr(p, 133, y + 8, 42, 23, 11, RGB(234, 235, 240));
      snprintf(b, sizeof(b), "€%d", prices[i]);
      center(p, 154, y + 13, 1, BLUE, b);
    }
    snprintf(b, sizeof(b), "Budget: €%d", s->credits);
    center(p, 96, 239, 0, GRAY, b);
  } else if (s->app == BATTERY)
    gameRight(s, p);
  else if (s->app == MESSAGES) {
    static const char *who[] = {"Support", "Your Bank", "Mom"};
    static const char *sub[] = {"About that license …", "We're fine. Really.",
                                "The DS already did that."};
    nav(s, p, "3 chats · 0 real connections");
    for (int i = 0; i < 3; i++) {
      int y = 97 + i * 47;
      rr(p, 6, y, 180, 43, 9, s->selection == i ? RGB(224, 237, 255) : WHITE);
      circle(p, 28, y + 20, 14,
             i == 0   ? RGB(164, 184, 210)
             : i == 1 ? RGB(133, 181, 156)
                      : RGB(193, 159, 189));
      center(p, 28, y + 11, 2, WHITE, i == 0 ? "S" : i == 1 ? "B" : "M");
      text(p, 49, y + 4, 2, BLACK, who[i]);
      text(p, 49, y + 23, 0, GRAY, sub[i]);
    }
    text(p, 10, 239, 0, GRAY, "Tap a chat. Read it on the left.");
  } else if (s->app == WEATHER) {
    nav(s, p, "Cupertino · entirely fictional");
    static const char *d[] = {"Today", "Tomorrow", "After the Update"};
    static const char *sub[] = {"22° · sunny and pricey", "19° · cloudy budget",
                                "41° · battery heatwave"};
    for (int i = 0; i < 3; i++)
      row(p, 100 + i * 43, d[i], sub[i], s->forecast == i);
    center(p, 96, 237, 0, GRAY, "100% chance of accessories.");
  } else if (s->app == CALC) {
    static const char *keys[] = {"7", "8", "9", "÷", "4", "5", "6", "×",
                                 "1", "2", "3", "−", "C", "0", "=", "+"};
    page(p, true);
    text(p, 10, 24, 2, WHITE, "‹ Home");
    button(p, 104, 21, 78, 21, s->calcPremium ? "19% Pro: on" : "19% Pro: off",
           false, ORANGE);
    if (s->calcError)
      snprintf(b, sizeof(b), "%s",
               s->calcError == 1 ? "Priceless ÷ 0" : "Too big. Bold.");
    else
      snprintf(b, sizeof(b), "%lld", (long long)s->calcValue);
    int f = width(5, b) > 172 ? 3 : 5;
    text(p, 182 - width(f, b), 42, f, WHITE, b);
    for (int i = 0; i < 16; i++) {
      int x = 10 + (i % 4) * 44, y = 80 + (i / 4) * 42;
      uint16_t bg = i % 4 == 3 || i == 14 ? ORANGE
                    : i == 12             ? RGB(165, 165, 169)
                                          : RGB(55, 55, 58);
      if (i == s->calcCursor)
        bg = mix(bg, WHITE, 4);
      rr(p, x, y, 38, 38, 19, bg);
      center(p, x + 19, y + 10, 3, i == 12 ? BLACK : WHITE, keys[i]);
    }
  } else if (s->app == SETTINGS) {
    nav(s, p, "iPhone Dou · Version 2.1");
    static const char *title[] = {"Appearance", "Sound", "Face ID", "About"};
    const char *val[] = {s->theme ? "Pastel" : "Dunes", s->sound ? "On" : "Off",
                         "Budget", "2.1"};
    for (int i = 0; i < 4; i++) {
      int y = 94 + i * 36;
      rr(p, 10, y, 172, 32, 9, s->selection == i ? RGB(224, 237, 255) : WHITE);
      text(p, 19, y + 9, 2, BLACK, title[i]);
      text(p, 172 - width(1, val[i]), y + 10, 1,
           i == 1 && s->sound ? GREEN : GRAY, val[i]);
    }
  } else if (s->app == PHOTOS) {
    nav(s, p, "Library · on this device");
    portrait(p, 10, 96, 59, 65, 7);
    photo48(s, p, 78, 96, 68, 65);
    snprintf(b, sizeof(b), "1 photo + %d 48-pixel shots", s->photos);
    text(p, 10, 162, 0, GRAY, b);
    button(p, 10, 172, 172, 30, "Take a photo", false, BLUE);
    button(p, 10, 211, 172, 27, "Change filter", false, BLUE);
  } else if (s->app == CALENDAR) {
    nav(s, p, "October 2026");
    static const char *week[] = {"M", "T", "W", "T", "F", "S", "S"};
    for (int i = 0; i < 7; i++)
      center(p, 23 + i * 24, 95, 0, GRAY, week[i]);
    for (int d = 1; d <= 31; d++) {
      int n = d + 2, x = 23 + (n % 7) * 24, y = 113 + (n / 7) * 23;
      if (d == s->calendarDay)
        circle(p, x, y + 8, 11, RED);
      snprintf(b, sizeof(b), "%d", d);
      center(p, x, y + 2, 1, d == s->calendarDay ? WHITE : BLACK, b);
    }
    text(p, 11, 240, 0, GRAY, "Tap a day. Events on the left.");
  } else if (s->app == NOTES) {
    nav(s, p, "X clears · this session only");
    rr(p, 8, 92, 176, 58, 6, WHITE);
    wrap(p, 14, 97, 2, 164, 14, BLACK, s->note);
    rect(p, 0, 154, 192, 94, RGB(211, 214, 220));
    static const char keys[] = "QWERTYUIOPASDFGHJKLZXCVBNM";
    for (int i = 0; i < 26; i++) {
      int x = i < 10   ? 5 + i * 18
              : i < 19 ? 14 + (i - 10) * 18
                       : 23 + (i - 19) * 18;
      int y = i < 10 ? 160 : i < 19 ? 183 : 206;
      rr(p, x, y, 17, 20, 3, i == s->noteCursor ? RGB(210, 231, 255) : WHITE);
      char k[2] = {keys[i], 0};
      center(p, x + 8, y + 4, 1, BLACK, k);
    }
    rr(p, 151, 206, 35, 20, 3, WHITE);
    center(p, 168, 211, 0, BLACK, "Del");
    rr(p, 42, 230, 105, 16, 3, WHITE);
    center(p, 94, 233, 0, BLACK, "Space");
  } else if (s->app == MAPS) {
    nav(s, p, "Demo route · no GPS");
    mapview(p, 10, 95, 172, 108, !s->mapRoute);
    button(p, 10, 214, 172, 30,
           s->mapRoute ? "To the Store · 800 m" : "Go Home · €0", true, BLUE);
  } else if (s->app == PHONE) {
    static const char *keys[] = {"1", "2", "3", "4", "5", "6",
                                 "7", "8", "9", "*", "0", "#"};
    page(p, false);
    text(p, 10, 23, 2, BLUE, "‹ Home");
    center(p, 96, 47, 1, GRAY, "KEYPAD · DEMO");
    for (int i = 0; i < 12; i++) {
      int x = 37 + (i % 3) * 59, y = 84 + (i / 3) * 39;
      circle(p, x, y, 18,
             s->dialCursor == i ? RGB(218, 232, 250) : RGB(228, 228, 234));
      center(p, x, y - 12, 4, BLACK, keys[i]);
    }
    circle(p, 96, 236, 14, s->phoneCalling ? RED : GREEN);
    icon(p, 86, 226, PHONE, 20);
  } else if (s->app == SAFARI) {
    page(p, false);
    text(p, 10, 23, 2, BLUE, "‹ Home");
    rr(p, 10, 48, 172, 26, 9, RGB(226, 227, 232));
    center(p, 96, 55, 2, BLACK, "dou.local");
    center(p, 96, 87, 4, BLACK, "iPhone Dou");
    center(p, 96, 122, 2, GRAY, "Two screens. One loan.");
    static const char *links[] = {"What's new?", "Pricing", "Privacy"};
    for (int i = 0; i < 3; i++)
      button(p, 10, 163 + i * 27, 172, 24, links[i], s->selection == i, BLUE);
  } else if (s->app == MUSIC) {
    nav(s, p, "The Accessoryless · offline");
    center(p, 96, 100, 3, BLACK, tracks[s->musicTrack]);
    line(p, 17, 128, 175, 128, LINE);
    int progress = s->musicTicks % 480;
    line(p, 17, 128, 17 + 158 * progress / 480, 128, BLACK);
    if (s->musicPlaying) {
      rect(p, 84, 145, 8, 30, BLACK);
      rect(p, 100, 145, 8, 30, BLACK);
    } else
      for (int i = 0; i < 26; i++)
        line(p, 85 + i, 142 + i / 2, 85 + i, 179 - i / 2, BLACK);
    center(p, 30, 150, 3, BLACK, "‹‹");
    center(p, 162, 150, 3, BLACK, "››");
    center(p, 96, 204, 1, GRAY,
           s->musicPlaying ? "Playing. Composed in-house."
                           : "Press A or Play to listen");
    center(p, 96, 229, 0, GRAY, "No subscription. A few real notes.");
  } else if (s->app == CLOCK) {
    nav(s, p, "Stopwatch");
    int sec = s->clockTicks / 60;
    snprintf(b, sizeof(b), "%02d:%02d", sec / 60, sec % 60);
    center(p, 96, 102, 5, BLACK, b);
    snprintf(b, sizeof(b), ".%02d", s->clockTicks % 60 * 100 / 60);
    center(p, 96, 140, 2, GRAY, b);
    button(p, 16, 178, 72, 42, "Reset", false, GRAY);
    button(p, 104, 178, 72, 42, s->clockRunning ? "Stop" : "Start", true,
           s->clockRunning ? RED : GREEN);
  }
  if (s->toastTicks) {
    rr(p, 9, 209, 174, 34, 11, RGB(48, 49, 54));
    wrap(p, 19, 215, 1, 154, 12, WHITE, s->toast);
  }
}
