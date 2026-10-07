#
# Template tonc makefile
#
# Yoinked mostly from DKP's template
#

# === SETUP ===========================================================

# --- No implicit rules ---
.SUFFIXES:

# --- Paths ---
export TONCLIB := ${DEVKITPRO}/libtonc

# === TONC RULES ======================================================
#
# Yes, this is almost, but not quite, completely like to
# DKP's base_rules and gba_rules
#

export PATH	:=	$(DEVKITARM)/bin:$(PATH)


# --- Executable names ---

PREFIX		?=	arm-none-eabi-

export CC	:=	$(PREFIX)gcc
export CXX	:=	$(PREFIX)g++
export AS	:=	$(PREFIX)as
export AR	:=	$(PREFIX)ar
export NM	:=	$(PREFIX)nm
export OBJCOPY	:=	$(PREFIX)objcopy

# LD defined in Makefile


# === LINK / TRANSLATE ================================================

%.gba : %.elf
	@$(OBJCOPY) -O binary $< $@
	@echo built ... $(notdir $@)
	@gbafix $@ -t$(TITLE)

#----------------------------------------------------------------------

%.mb.elf :
	@echo Linking multiboot
	$(LD) -specs=gba_mb.specs $(LDFLAGS) $(OFILES) $(LIBPATHS) $(LIBS) -o $@
	$(NM) -Sn $@ > $(basename $(notdir $@)).map

#----------------------------------------------------------------------

%.elf :
	@echo Linking cartridge
	$(LD) -specs=gba.specs $(LDFLAGS) $(OFILES) $(LIBPATHS) $(LIBS) -o $@
	$(NM) -Sn $@ > $(basename $(notdir $@)).map

#----------------------------------------------------------------------

%.a :
	@echo $(notdir $@)
	@rm -f $@
	$(AR) -crs $@ $^


# === OBJECTIFY =======================================================

%.iwram.o : %.iwram.cpp
	@echo $(notdir $<)
	$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CXXFLAGS) $(IARCH) -c $< -o $@

#----------------------------------------------------------------------
%.iwram.o : %.iwram.c
	@echo $(notdir $<)
	$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) $(IARCH) -c $< -o $@

#----------------------------------------------------------------------

%.o : %.cpp
	@echo $(notdir $<)
	$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CXXFLAGS) $(RARCH) -c $< -o $@

#----------------------------------------------------------------------

%.o : %.c
	@echo $(notdir $<)
	$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) $(RARCH) -c $< -o $@

#----------------------------------------------------------------------

%.o : %.s
	@echo $(notdir $<)
	$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d -x assembler-with-cpp $(ASFLAGS) -c $< -o $@

#----------------------------------------------------------------------

%.o : %.S
	@echo $(notdir $<)
	$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d -x assembler-with-cpp $(ASFLAGS) -c $< -o $@


#----------------------------------------------------------------------
# canned command sequence for binary data
#----------------------------------------------------------------------

define bin2o
	bin2s $< | $(AS) -o $(@)
	echo "extern const u8" `(echo $(<F) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"_end[];" > `(echo $(<F) | tr . _)`.h
	echo "extern const u8" `(echo $(<F) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"[];" >> `(echo $(<F) | tr . _)`.h
	echo "extern const u32" `(echo $(<F) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`_size";" >> `(echo $(<F) | tr . _)`.h
endef
# =====================================================================

# --- Main path ---

export PATH	:=	$(DEVKITARM)/bin:$(PATH)


# === PROJECT DETAILS =================================================
# PROJ		: Base project name
# TITLE		: Title for ROM header (12 characters)
# LIBS		: Libraries to use, formatted as list for linker flags
# BUILD		: Directory for build process temporaries. Should NOT be empty!
# SRCDIRS	: List of source file directories
# DATADIRS	: List of data file directories
# INCDIRS	: List of header file directories
# LIBDIRS	: List of library directories
# General note: use `.' for the current dir, don't leave the lists empty.

export PROJ	:= LinkCard_demo
TITLE		:= LinkCard_demo

LIBS		:= -ltonc -lugba

BUILD		:= build
SRCDIRS		:= src ../_lib ../../lib ../../lib/iwram_code ../_lib/libgbfs
DATADIRS	:= data
INCDIRS		:= src
LIBDIRS		:= $(TONCLIB) $(CURDIR)/../_lib/libugba

# --- switches ---

bMB		:= 0	# Multiboot build
bTEMPS	:= 0	# Save gcc temporaries (.i and .s files)
bDEBUG2	:= 0	# Generate debug info (bDEBUG2? Not a full DEBUG flag. Yet)


# === BUILD FLAGS =====================================================
# This is probably where you can stop editing
# NOTE: I've noticed that -fgcse and -ftree-loop-optimize sometimes muck
#	up things (gcse seems fond of building masks inside a loop instead of
#	outside them for example). Removing them sometimes helps

# --- Architecture ---

ARCH    := -mthumb-interwork -mthumb
RARCH   := -mthumb-interwork -mthumb
IARCH   := -mthumb-interwork -marm -mlong-calls

# --- Main flags ---

CFLAGS		:= -mcpu=arm7tdmi -mtune=arm7tdmi -Ofast
CFLAGS		+= -Wall
CFLAGS		+= $(INCLUDE)
CFLAGS		+= -ffast-math -fno-strict-aliasing

USERFLAGS ?=
CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions -std=c++17 -DLINK_DEVELOPMENT $(USERFLAGS)

ASFLAGS		:= $(ARCH) $(INCLUDE)
LDFLAGS 	:= $(ARCH) -Wl,--print-memory-usage,-Map,$(TARGET).map

# --- switched additions ----------------------------------------------

# --- Multiboot ? ---
ifeq ($(strip $(bMB)), 1)
	TARGET	:= $(PROJ).mb
	CFLAGS   += -DMULTIBOOT_BUILD=1
	CXXFLAGS += -DMULTIBOOT_BUILD=1
else
	TARGET	:= $(PROJ)
	CFLAGS   += -DMULTIBOOT_BUILD=0
	CXXFLAGS += -DMULTIBOOT_BUILD=0
endif

# --- Save temporary files ? ---
ifeq ($(strip $(bTEMPS)), 1)
	CFLAGS		+= -save-temps
	CXXFLAGS	+= -save-temps
endif

# --- Debug info ? ---

ifeq ($(strip $(bDEBUG)), 1)
	CFLAGS		+= -DDEBUG -g
	CXXFLAGS	+= -DDEBUG -g
	ASFLAGS		+= -DDEBUG -g
	LDFLAGS		+= -g
else
	CFLAGS		+= -DNDEBUG
	CXXFLAGS	+= -DNDEBUG
	ASFLAGS		+= -DNDEBUG
endif


# === BUILD PROC ======================================================
#
# Do not use CURDIR to decide whether this is the recursive build.
# CURDIR may contain spaces/parentheses and GNU make functions split it
# into words. Instead, pass an explicit IN_BUILD=1 marker.

ifeq ($(IN_BUILD),1)

# We are inside LinkCard_demo/build.
DEPENDS := $(OFILES:.o=.d)

$(OUTPUT).gba : $(OUTPUT).elf

$(OUTPUT).elf : $(OFILES)

-include $(DEPENDS)

else

# We are in LinkCard_demo.
# Keep paths handed to the recursive make relative to build/ so the
# absolute repository path never reaches /bin/sh or the compiler.

export OUTPUT := ../$(TARGET)

# Source/data lookup paths as seen FROM build/
export VPATH := \
	../src \
	../../_lib \
	../../../lib \
	../../../lib/iwram_code \
	../../_lib/libgbfs \
	../data

export DEPSDIR := .

# Discover files while still in LinkCard_demo
CFILES   := $(foreach dir,$(SRCDIRS),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SRCDIRS),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES   := $(foreach dir,$(SRCDIRS),$(notdir $(wildcard $(dir)/*.s)))
BINFILES := $(foreach dir,$(DATADIRS),$(notdir $(wildcard $(dir)/*.*)))

ifeq ($(strip $(CPPFILES)),)
	export LD := $(CC)
else
	export LD := $(CXX)
endif

export OFILES := \
	$(addsuffix .o,$(BINFILES)) \
	$(CFILES:.c=.o) \
	$(CPPFILES:.cpp=.o) \
	$(SFILES:.s=.o)

# Include/library paths as seen FROM build/
export INCLUDE := \
	-I../src \
	-I../../_lib/libugba/include \
	-I$(TONCLIB)/include \
	-I.

export LIBPATHS := \
	-L.. \
	-L../../_lib/libugba/lib \
	-L$(TONCLIB)/lib

$(BUILD):
	@[ -d "$@" ] || mkdir -p "$@"
	@$(MAKE) --no-print-directory -C "$@" -f "../Makefile" IN_BUILD=1
	$(NM) -Sn "$(TARGET).elf" > "$(BUILD)/$(TARGET).map"

all: $(BUILD)

clean:
	@echo clean ...
	@rm -rf "$(BUILD)" "$(TARGET).elf" "$(TARGET).gba" "$(TARGET).sav" "$(TARGET).out.gba" "loader.gbfs"

endif

# --- More targets ----------------------------------------------------

.PHONY: all clean rebuild package start restart

rebuild: clean $(BUILD) package

package: $(BUILD)
	cd "gbfs_files" && gbfs loader.gbfs * && mv loader.gbfs ..
	../gbfs.sh "$(TARGET).gba" "loader.gbfs" "$(TARGET).out.gba"

start: package
	start "$(TARGET).out.gba"

restart: rebuild start

# EOF
