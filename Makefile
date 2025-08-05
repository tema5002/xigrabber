CC := clang
CFLAGS := -Wall -Wextra -Werror -std=gnu99
TARGET := xigrabber
SRC := xigrabber.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $@ $(CFLAGS)

clean:
	$(RM) $(TARGET)
