CC=x86_64-linux-gnu-gcc
TARGET=xigrabber
CFLAGS=-Wall -Wextra -Werror -std=c11

MINGW_CC=x86_64-w64-mingw32-gcc
MINGW_TARGET=$(TARGET).exe
MINGW_CFLAGS=$(CFLAGS) -municode -Wl,--subsystem,console -lshlwapi

SRC=src/main.c
SRCS=$(SRC) src/*.h

$(TARGET): $(SRCS)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS)

$(MINGW_TARGET): $(SRCS)
	$(MINGW_CC) $(SRC) -o $(MINGW_TARGET) $(MINGW_CFLAGS)

all: $(TARGET) $(MINGW_TARGET)

clean:
	rm $(TARGET) $(MINGW_TARGET)
