CC = gcc
CFLAGS = -Wall -Iinclude
RAYLIB_FLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC := $(shell find src -name "*.c")
OBJ := $(patsubst src/%.c, build/%.o, $(SRC))
OUT = build/main

all: $(OUT)

# Enlazar objetos para crear el ejecutable
$(OUT): $(OBJ)
	@mkdir -p $(dir $@)
	@cp -r assets build/
	$(CC) $(OBJ) -o $@ $(RAYLIB_FLAGS)

# Compilar cada .c a .o manteniendo la estructura
build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build
