# Nintendo DS/DSi homebrew build. Requires BlocksDS + Wonderful Toolchain.
NAME := iPhone_Dou
GAME_TITLE := iPhone Dou
GAME_SUBTITLE := Fold Edition - English 2.1
GAME_AUTHOR := Unofficial Parody
GAME_ICON := artwork/launcher.png
ARM7ELF = $(BLOCKSDS)/sys/arm7/main_core/arm7_minimal.elf
CFLAGS := -std=gnu11 -Wextra -Werror
LDFLAGS := -Wl,-z,noexecstack
BLOCKSDS ?= /opt/blocksds/core
include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
