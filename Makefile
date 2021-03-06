CROSS_COMPILE = aarch64-linux-gnu-

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

CC_FLAGS = -std=gnu99 -Wall -Ilib $(CFLAGS)
AS_FLAGS = -Wall $(ASFLAGS)
LD_FLAGS = --as-needed $(LDFLAGS)


BIN = armstub8.bin
ELF = armstub8.elf
DIS = armstub8.dis

OBJ = start.o \
	  app.o \
	  lib/decompress_unlzma.o

LDS = linker-script.lds

DEV = /dev/sdi1
MOUNTPOINT = /media/pi-sd/boot

all: $(BIN) $(DIS)
.PHONY: all

%.o : %.S
	$(CC) $(AS_FLAGS) -c -o $@ $<

%.o : %.c
	$(CC) $(CC_FLAGS) -c -o $@ $<

$(ELF) : $(OBJ) $(LDS)
	@#$(CC) -static -nostartfiles -T $(LDS) -o $@ -Wl,--as-needed $(OBJ)
	$(LD) $(LD_FLAGS) -T $(LDS) -o $@ $(OBJ)

$(BIN) : $(ELF)
	$(OBJCOPY) $< -O binary $@

$(DIS) : $(ELF)
	$(OBJDUMP) -D $< > $@

clean:
	rm -f *.o lib/*.o *.elf *.bin *.dis

install: $(BIN)
	@sudo sh -xc 'mount $(DEV) $(MOUNTPOINT) && \
		          cp -f $(BIN) $(MOUNTPOINT)/armstub8.bin && \
				  umount $(DEV)'

.PHONY: clean install
