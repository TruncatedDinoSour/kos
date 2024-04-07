LIBS := -lcrypt
ifeq ($(NOQA),)
	CFLAGS += -std=c89 -ansi -Wall -Wextra -Wpedantic -pedantic -Wshadow -Werror -Wconversion -Wformat -Wuninitialized -Wmissing-prototypes -Wmissing-declarations -Wstrict-prototypes -Wredundant-decls -Wfloat-equal -Wcast-qual -Wnested-externs -Wvla -Winline -Wmissing-format-attribute -Wmissing-noreturn -pedantic-errors
endif

SRC_DIR := src
OBJ_DIR := obj

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

kos: obj/asm.o $(OBJ_FILES)
	$(CC) -o $@ $(CFLAGS) $(F_CFLAGS) $^ $(LIBS) $(LDFLAGS) $(F_LDFLAGS)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/%.h | $(OBJ_DIR)
	$(CC) -c -o $@ $(CFLAGS) $< $(LDFLAGS)

obj/asm.o: $(wildcard asm/*.asm) | $(OBJ_DIR)
	[ `uname -m` = x86_64 ] && nasm -f elf64 -o $(OBJ_DIR)/asm.o asm/64.asm || nasm -f elf32 -o $(OBJ_DIR)/asm.o asm/32.asm

clean:
	rm -rf kos $(OBJ_DIR)

strip:
	strip --remove-section=.note.gnu.gold-version --remove-section=.note --remove-section=.gnu.version --remove-section=.eh_frame --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --strip-symbol=__gmon_start__ --remove-section=.comment --remove-section=.eh_frame_ptr --strip-unneeded --strip-all --strip-debug --merge-notes --strip-dwo --discard-all --discard-locals --verbose kos

suid: kos
	chown root:root kos
	chmod 4111 kos
