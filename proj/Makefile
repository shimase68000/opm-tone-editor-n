#
# OPM Tone Editor 'N' Makefile
#

SHELL := /bin/bash

# for debug
define dbg
  @echo "[Making: $@] [Deps: $^] [Changed: $?]"
endef

PROJ := oe

.DEFAULT_GOAL := all

# --- define directory ---
XBINDIR := xbin
XOBJDIR := build_x68k
XLIBDIR := lib_h68k
XINCDIR := inc_h68k
XINCDIR_S := inc_h68k_s

OBJDIR  := build
LIBDIR  := lib
USRINC  := inc
USRINC_S := inc_s

SRCDIR  := src
SRCDIR_S := src_s

# --- Tools for Human68k ---
RUN68 := $(XBINDIR)/run68.exe
HAS   := $(XBINDIR)/has060x.x
HLK   := $(XBINDIR)/hlkx.r

# --- RAMDISK ---
RAMDISK ?= /mnt/ramdisk/$(PROJ)

# --- Build tools ---
CC    := m68k-xelf-gcc
LK    := $(CC)
NM    := m68k-xelf-nm
AS    := $(RUN68) $(HAS)

# SJIS環境ならこっち
SPECS := -specs=xc.sjis.specs

# include dirs
INCDIRS_S = . $(USRINC_S) $(XINCDIR_S) $(SRCDIR_S)

# GCC COMPILE OPTION
CPPFLAGS  = $(addprefix -I,$(INCDIRS_S))
CPPFLAGS += -finput-charset=cp932 -fno-builtin

CPUFLAGS  = -mcpu=68000
CWARN     = -Wall -Wextra

CFLAGS   := $(CPUFLAGS) $(CWARN) -O2 $(CPPFLAGS) -MMD -MP
#CFLAGS   := $(CPUFLAGS) $(CWARN) $(CPPFLAGS) -MMD -MP
LDFLAGS  := $(SPECS) -Wl,-s

ASFLAGS  := -m 68000 -i ./$(XINCDIR_S)

# XCライブラリ（install-xclib.sh 済み前提）
LDLIBS := \
        $(LIBDIR)/clib_elf.a \
        $(LIBDIR)/iocslib_elf.a \
        $(LIBDIR)/doslib_elf.a \
        $(LIBDIR)/baslib_elf.a \
        $(LIBDIR)/gnulib_elf.a \
        $(LIBDIR)/libdos_elf.a \
        $(LIBDIR)/libiocs_elf.a

# --- C sources (WSL build) ---
SRCS_C := \
        oe.c file.c json.c config.c mylib.c \
        key_routine.c event.c \
        ms_routine.c mevent.c ms_map.c \
        emenu.c fileop.c xevent.c\
        res_routine.c key_input.c \
        disp.c disp_sub.c disp_ex.c \
        flag.c cursor.c const.c \
        opmset.c scalekey.c trap7.c

SRCS_DEF := \
        disp.def \
        map_message.def \
        json_hier.def \
        json_error.def \
        json_para_check.def

# --- ASM sources (HAS060 on run68) ---
SRCS_S := \
        trap7_asm.s GETSSS.s

OBJS := $(SRCS_C:%.c=$(OBJDIR)/%.o)
DEPS := $(OBJS:.o=.d)

# --- FileEncoding conversion targets ---
# inc/
USRINC_FILES  := $(wildcard $(USRINC)/*.h)
USRINC_SJIS   := $(patsubst $(USRINC)/%,$(USRINC_S)/%,$(USRINC_FILES))
# src/
SRCS_C_SJIS   := $(SRCS_C:%.c=$(SRCDIR_S)/%.c)
SRCS_DEF_SJIS := $(SRCS_DEF:%.def=$(SRCDIR_S)/%.def)
SRCS_S_SJIS   := $(SRCS_S:%.s=$(SRCDIR_S)/%.s)

# UTF-8 -> SJIS conversion rules
$(SRCDIR_S)/%.c: $(SRCDIR)/%.c | $(SRCDIR_S)
	iconv -f UTF-8 -t CP932 $< > $@

$(SRCDIR_S)/%.def: $(SRCDIR)/%.def | $(SRCDIR_S)
	iconv -f UTF-8 -t CP932 $< > $@

$(SRCDIR_S)/%.s: $(SRCDIR)/%.s | $(SRCDIR_S)
	iconv -f UTF-8 -t CP932 $< > $@

$(USRINC_S)/%.h: $(USRINC)/%.h | $(USRINC_S)
	iconv -f UTF-8 -t CP932 $< > $@

# HASでHuman68k形式の .o を作る
X68K_ASM_OBJS := $(SRCS_S:%.s=$(XOBJDIR)/%.o)

# Convert to ELF .o for linking
ELF_ASM_OBJS  := $(SRCS_S:%.s=$(OBJDIR)/%_elf.o)

.SECONDARY:

# convert target (explicit)
.PHONY: convert
convert: $(SRCS_C_SJIS) $(SRCS_S_SJIS) $(USRINC_SJIS) $(SRCS_DEF_SJIS)

.PHONY: all
all:
	$(MAKE) convert
	$(MAKE) $(PROJ).x

.PHONY: clean show undef
$(OBJDIR):
	mkdir -p $(RAMDISK)/$@
	@if [ -L ./$@ ]; then \
		ln -sfn $(RAMDISK)/$@ ./$@; \
	elif [ ! -e ./$@ ]; then \
		ln -s $(RAMDISK)/$@ ./$@; \
	fi

$(XOBJDIR):
	mkdir -p $@

$(SRCDIR_S):
	mkdir -p $@

$(USRINC_S):
	mkdir -p $@

$(XINCDIR_S):
	mkdir -p $@

# Compile C (ELF relocatable)
$(OBJDIR)/%.o: $(SRCDIR_S)/%.c | $(OBJDIR)
	$(CC) $(SPECS) $(CFLAGS) -c $< -o $@

# Assemble (HAS via run68) -> Human68k .o
$(XOBJDIR)/%.o: $(SRCDIR_S)/%.s | $(XOBJDIR)
	$(AS) $(ASFLAGS) $< -o $@

# Convert Human68k .o -> ELF .o
# NOTE: do NOT use -k since your C symbols are without leading '_'
$(OBJDIR)/%_elf.o: $(XOBJDIR)/%.o | $(OBJDIR)
	x68k2elf.py -v $< $@

# Link (produces oe.x and oe.x.elf)
$(PROJ).x: $(OBJS) $(ELF_ASM_OBJS)
	$(LK) -o $@ $^ $(LDLIBS) $(LDFLAGS)
	@echo -e "\033[32m"
	ls -la $@
	@echo -e "\033[0m"

# Quick check: show undefined symbols after link objects are ready
undef: $(OBJS) $(ELF_ASM_OBJS)
	@echo "---- Undefined symbols (per object) ----"
	@$(NM) -u $(OBJS) $(ELF_ASM_OBJS) || true

show:
	@echo "OBJS=$(OBJS)"
	@echo "X68K_ASM_OBJS=$(X68K_ASM_OBJS)"
	@echo "ELF_ASM_OBJS=$(ELF_ASM_OBJS)"

-include $(DEPS)

check:
	make clean
	make all -j8

clean:
	rm -rf $(RAMDISK)/*
	rm -rf $(SRCDIR_S) $(USRINC_S)
	rm -rf $(OBJDIR) $(XOBJDIR)
	rm -rf $(PROJ).x $(PROJ).x.elf
