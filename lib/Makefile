CC = gcc
CC_FLAGS = -std=gnu99 -Wall -DXZ_USE_CRC64 $(CFLAGS)

LIB_OBJ = decompress_unlzma.o

PROG_OBJ = decompress_test.o

ALL_OBJ = $(LIB_OBJ) $(PROG_OBJ)

TARGET = decompress_test

all: $(TARGET)

%.o : %.c
	$(CC) $(CC_FLAGS) -c -o $@ $<

$(TARGET): $(ALL_OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
