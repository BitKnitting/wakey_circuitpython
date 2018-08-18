ifneq ($(lastword a b),b)
$(error These Makefiles require make 3.81 or newer)
endif

# Set TOP to be the path to get from the current directory (where make was
# invoked) to the top of the tree. $(lastword $(MAKEFILE_LIST)) returns
# the name of this makefile relative to where make was invoked.
#
# We assume that this file is in the py directory so we use $(dir ) twice
# to get to the top of the tree.

THIS_MAKEFILE := $(lastword $(MAKEFILE_LIST))
TOP := $(patsubst %/py/mkenv.mk,%,$(THIS_MAKEFILE))

# Turn on increased build verbosity by defining BUILD_VERBOSE in your main
# Makefile or in your environment. You can also use V=1 on the make command
# line.

ifeq ("$(origin V)", "command line")
BUILD_VERBOSE=$(V)
endif
ifndef BUILD_VERBOSE
BUILD_VERBOSE = 0
endif
ifeq ($(BUILD_VERBOSE),0)
Q = @
STEPECHO = @:
else ifeq ($(BUILD_VERBOSE),1)
Q = @
STEPECHO = @echo
else
Q =
STEPECHO = @echo
endif
# Since this is a new feature, advertise it
ifeq ($(BUILD_VERBOSE),0)
$(info Use make V=1, make V=2 or set BUILD_VERBOSE similarly in your environment to increase build verbosity.)
endif

# default settings; can be overridden in main Makefile

PY_SRC ?= $(TOP)/py
BUILD ?= build

ECHO = @echo

CD = cd
CP = cp
FIND = find
MKDIR = mkdir
PYTHON = python
RM = rm
RSYNC = rsync
SED = sed

AS = $(CROSS_COMPILE)as
CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE = $(CROSS_COMPILE)size
STRIP = $(CROSS_COMPILE)strip
AR = $(CROSS_COMPILE)ar
ifeq ($(MICROPY_FORCE_32BIT),1)
CC += -m32
CXX += -m32
LD += -m32
endif

MAKE_FROZEN = $(TOP)/tools/make-frozen.py
MPY_CROSS = $(TOP)/mpy-cross/mpy-cross
MPY_TOOL = $(TOP)/tools/mpy-tool.py
PREPROCESS_FROZEN_MODULES = PYTHONPATH=$(TOP)/tools/python-semver $(TOP)/tools/preprocess_frozen_modules.py

all:
.PHONY: all

.DELETE_ON_ERROR:

MKENV_INCLUDED = 1
