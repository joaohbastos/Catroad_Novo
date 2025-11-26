# CatRoad - Linux
APP      := catroad
BUILD    := build
SRC_DIR  := src
INC_DIR  := include
SRCS     := $(wildcard $(SRC_DIR)/*.c)
OBJS     := $(SRCS:$(SRC_DIR)/%.c=$(BUILD)/%.o)

CC       := gcc
CFLAGS   := -O2 -Wall -Wextra -I$(INC_DIR)

RAYLIB_CFLAGS := $(shell pkg-config --cflags raylib 2>/dev/null)
RAYLIB_LIBS   := $(shell pkg-config --libs   raylib 2>/dev/null)

CFLAGS  += $(RAYLIB_CFLAGS) -I/usr/local/include
LDFLAGS := -Wl,--no-as-needed -L/usr/local/lib

LDLIBS  := $(RAYLIB_LIBS) -lraylib -lm -lpthread -ldl -lrt -lX11

$(BUILD)/$(APP): $(OBJS)
	@mkdir -p $(BUILD)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

$(BUILD)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run clean
run: $(BUILD)/$(APP)
	./$(BUILD)/$(APP)

clean:
	rm -rf $(BUILD)
