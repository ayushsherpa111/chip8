C_SRC := $(shell find .$(SRCDIR)/ -type f -name '*.c')
CC := clang-9
O_SRC = ${C_SRC:.c=.o}
CFLAGS := `sdl2-config --libs --cflags` -ggdb3 -O0 --std=c99 -Wall -lSDL2_image -lm

O_DIR := build
REL_DIR = $(O_DIR)/release

all: dir_release
	echo "OUTPUT: " ${REL_DIR}
	rm -rf ${O_SRC}

dir_release: release link
	rm -rf $(shell find ./${REL_DIR} -type f -name '*.c')

release:
	mkdir -p ${REL_DIR}

link: ${O_SRC}
	echo ${O_SRC}
	$(CC) $^ -o ${REL_DIR}/main $(CFLAGS)

%.o: %.c
	echo "$< -> $@"
	clang-9 -c $< -o $@
