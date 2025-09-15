.POSIX:
.SUFFIXES:
.PHONY: clean run debug release

CC         = c99
NOLIBC     = -nostartfiles -fno-builtin
WARNINGS   = -Wall -Wextra -pedantic -Wno-unused-parameter
WINDRES    = windres -I res/

ifdef RELEASE
CFLAGS    += $(NOLIBC) -Oz -DNDEBUG=1
LDFLAGS   += -s
else
CFLAGS    += $(NOLIBC) $(WARNINGS) -O0 -DDEBUG=1 -ggdb3
endif

chres.exe: src/chres.c res/chres.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -o $@ $^ $(LDLIBS)

%.o: %.rc
	$(WINDRES) -o $@ $^

clean:
	rm chres.exe res/chres.o || true

run: chres.exe
	gdb --batch --ex=run --args chres.exe 1920x1080@144

debug: chres.exe
	gdb --ex=start --args chres.exe 1280x720@144

release: clean
	make RELEASE=1
