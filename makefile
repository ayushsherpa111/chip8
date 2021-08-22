C_SRC := $(shell find ./ -type f -name '*.c')
CC := clang-9
O_SRC = ${C_SRC:.c=.o}
CFLAGS := `sdl2-config --libs --cflags ` -ggdb3 -O0 --std=c99 -Wall -lm

O_DIR := build
REL_DIR = $(O_DIR)/release

all: release link
	echo "OUTPUT: " ${REL_DIR}
	echo "OUT files: " ${O_SRC}
	rm -rf ${O_SRC}

release:
	mkdir -p ${REL_DIR}

link: ${O_SRC}
	echo ${O_SRC}
	$(CC) -g $^ -o ${REL_DIR}/main $(CFLAGS)

%.o: %.c
	echo "$< -> $@"
	$(CC) -g -c $< -o $@
