CC = gcc
CFLAGS = -Wall -Iinclude
RAYLIB_FLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
SRC = $(wildcard *.c)
OUT = build/main

all: $(OUT)

$(OUT): $(SRC)
	@mkdir -p build
	$(CC) $(CFLAGS) $^ -o $@ $(RAYLIB_FLAGS)

clean:
	rm -rf build
