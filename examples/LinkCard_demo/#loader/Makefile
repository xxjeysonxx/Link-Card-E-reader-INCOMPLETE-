# //--------------------------------------------------------//
# // Based on Nintendo e-Reader "Hello World" Example (GBA) //
# // (c) 2004 Tim Schuerewegen                              //
# //--------------------------------------------------------//

# CHANGE THESE PATHS!
DEVKITPRO=/opt/devkitpro
EREADER=/opt/devkitpro/_e-reader

GCC_VER := $(shell $(DEVKITPRO)/devkitARM/bin/arm-none-eabi-gcc -dumpversion)
PATH := /bin:$(EREADER):$(DEVKITPRO)/tools/bin:$(DEVKITPRO)/devkitARM/bin:$(PATH)

PATH_LIB_GCC := $(shell $(DEVKITPRO)/devkitARM/bin/arm-none-eabi-gcc -print-libgcc-file-name)
PATH_LIB_GCC := $(dir $(PATH_LIB_GCC))

CFLAGS_EXTRA = -std=gnu17

ifeq ($(JAP),1)
  NAME = ローダー
  REGION = 2
  CFLAGS_EXTRA += -DREGION_JAP
  ifeq ($(ENG),1)
    CFLAGS_EXTRA += -DLANGUAGE_ENG
  endif
else
  NAME = DLC Loader
  REGION = 1
endif

# assemble and link
GCC      = $(DEVKITPRO)/devkitARM/bin/arm-none-eabi-gcc
AS       = $(DEVKITPRO)/devkitARM/bin/arm-none-eabi-as
LD       = $(DEVKITPRO)/devkitARM/bin/arm-none-eabi-ld
OBJCOPY  = $(DEVKITPRO)/devkitARM/bin/arm-none-eabi-objcopy

# e-reader tools
NEVPK    = $(EREADER)/nevpk.exe
NESAV    = $(EREADER)/neflmake.exe
NEDCMAKE = $(EREADER)/nedcmake.exe
NEDCENC  = $(EREADER)/nedcenc.exe
RAW2BMP  = $(EREADER)/raw2bmp.exe

.PHONY: clean

all : sav bmp loader

sav : main.sav

bmp : main.vpk
	$(NEDCMAKE) -i $< -type 2 -bin -type 2 -region $(REGION) -name "$(NAME)" -fill 1 -save 1 -o "main.bin"
	$(NEDCENC) -i "main.bin" -o "main.raw"
	$(RAW2BMP) -i "main.raw" -o "main.bmp"

main.sav : main.vpk
	$(NESAV) -i $< -o "$@" -type 2 -name "$(NAME)"

main.vpk : main.bin
	$(NEVPK) -i "$<" -o "$@" -c -level 0

main.bin : main.elf
	$(OBJCOPY) -O binary "$<" "$@"

main.elf : crt0.o main.o ereader.ld
	$(LD) crt0.o main.o -lgcc -L $(PATH_LIB_GCC) -T ereader.ld -O3 -o "$@"

main.o : src/main.c
	$(GCC) -mthumb $(CFLAGS_EXTRA) -c -O3 -o "$@" "$<"

crt0.o : crt0.s
	$(AS) -o "$@" "$<"

loader: main.loader

# `main.loader` contains the bytes that end up being sent via Link Cable
# It reads 4 bytes at offset 0x1002C as a little-endian UInt32 (the size),
# then copies from offset 0x10000 until (0x10000 + (0x002C+4+32+size)),
# rounded up to the next multiple of 32.
main.loader: main.sav
	@echo "Generating main.loader from main.sav..."
	@size_hex=$$(dd if=main.sav bs=1 skip=$$((0x1002C)) count=4 2>/dev/null | od -An -tx1 | tr -s ' ') && \
	set -- $$size_hex && \
	size=$$(( 0x$$1 + (0x$$2 << 8) + (0x$$3 << 16) + (0x$$4 << 24) )); \
	header_extra=$$((0x002C + 4 + 32)) ; \
	length=$$(( header_extra + size )); \
	total=$$(( (length + 31) & ~31 )); \
	dd if=main.sav bs=1 skip=$$((0x10000)) count=$$total of=main.loader 2>/dev/null

clean :
	rm -f *.bin *.bmp *.elf *.gba *.o *.raw *.sa1 *.sav *.vpk *.loader
