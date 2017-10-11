CROSS_COMPILE = aarch64-linux-gnu-

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

BIN = armstub8.bin
ELF = armstub8.elf
DIS = armstub8.dis

OBJ = start.o \
	  app.o

LDS = linker-script.lds

DEV = /dev/sdi1
MOUNTPOINT = /media/pi-sd/boot

all: $(BIN) $(DIS)
.PHONY: all

%.o : %.S
	$(CC) -Wall -c -o $@ $<

%.o : %.c
	$(CC) -std=gnu99 -Wall -c -o $@ $<

$(ELF) : $(OBJ) $(LDS)
	@#$(CC) -static -nostartfiles -T $(LDS) -o $@ -Wl,--as-needed $(OBJ)
	$(LD) -T $(LDS) -o $@ $(OBJ)

$(BIN) : $(ELF)
	$(OBJCOPY) $< -O binary $@

$(DIS) : $(ELF)
	$(OBJDUMP) -D $< > $@

clean:
	rm -f *.o *.elf *.bin *.dis

install: $(BIN)
	@sudo sh -xc 'mount $(DEV) $(MOUNTPOINT) && \
		          rm -f $(MOUNTPOINT)/kernel*.img && \
		          cp -f $(BIN) $(MOUNTPOINT)/armstub8.bin && \
				  umount $(DEV)'

.PHONY: clean install
