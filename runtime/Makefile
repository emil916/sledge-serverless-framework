ARCH := $(shell uname -m)
TOTAL_CORES := $(shell getconf _NPROCESSORS_CONF)
PAGE_SIZE=$(shell getconf PAGESIZE)

# Compiler Settings
CC=clang
CC_OPTIONS = -O3 -flto -g -pthread -D_GNU_SOURCE

# CC_OPTIONS for Debugging
# CC_OPTIONS = -O0 -g -pthread -D_GNU_SOURCE

BINARY_NAME=sledgert

# Options: {USE_MEM_GENERIC, USE_MEM_VM}
USE_MEM = USE_MEM_VM

# Feature Toggles
# CFLAGS += -DADMISSIONS_CONTROL

# Debugging Flags

# Strips out calls to assert() and disables debuglog
# CFLAGS += -DNDEBUG

# Redirects debuglogs to /runtime/bin/sledge.log 
# CFLAGS += -DLOG_TO_FILE

# Various Informational Logs for Debugging
# CFLAGS += -DLOG_HTTP_PARSER
# CFLAGS += -DLOG_STATE_CHANGES
# CFLAGS += -DLOG_LOCK_OVERHEAD
# CFLAGS += -DLOG_LISTENER_LOCK_OVERHEAD
# CFLAGS += -DLOG_CONTEXT_SWITCHES
# CFLAGS += -DLOG_ADMISSIONS_CONTROL
# CFLAGS += -DLOG_REQUEST_ALLOCATION
# CFLAGS += -DLOG_PREEMPTION
# CFLAGS += -DLOG_MODULE_LOADING

# This flag dumps totals of incoming requests and outgoing responses, broken out by status code
# family, such as 2XX, 4XX, 5XX. It is useful to debug clients hanging waiting for a response.
# To log, run `call http_total_log()` while in GDB
# CFLAGS += -DLOG_TOTAL_REQS_RESPS

# This flag logs the total number of sandboxes in the various states
# It is useful to debug if sandboxes are "getting caught" in a particular state
# To log, run `call runtime_log_sandbox_states()` while in GDB
# CFLAGS += -DLOG_SANDBOX_COUNT

# This flag enables an per-worker atomic count of sandbox's local runqueue count in thread local storage
# Useful to debug if sandboxes are "getting caught" or "leaking" while in a local runqueue
# CFLAGS += -DLOG_LOCAL_RUNQUEUE

# System Configuration Flags

# Sets a flag equal to the processor architecture
CFLAGS += -D${ARCH}
CFLAGS += -DNCORES=${TOTAL_CORES}
CFLAGS += -DPAGE_SIZE=$(PAGE_SIZE)

# Optionally Disable preemption
# CFLAGS += -DPREEMPT_DISABLE

CFLAGS += -D${USE_MEM}

# Preprocessor
LDFLAGS += -Wl,--export-dynamic -ldl -lm
LDFLAGS += -Lthirdparty/dist/lib/
INCLUDES += -Iinclude/ -Ithirdparty/dist/include/

# CFILES
CFILES += src/*.c
CFILES += src/arch/${ARCH}/*.c
CFILES += src/libc/*.c
CFILES += src/memory/common.c
CFILES += thirdparty/dist/lib/http_parser.o
# TODO: Is USE_MEM_GENERIC out of date? I do not see that file.
# Does that mean we can make USE_MEM_VM an invariant?
ifeq ($(USE_MEM),USE_MEM_GENERIC)
CFILES += src/memory/generic.c
else ifeq ($(USE_MEM),USE_MEM_VM)
CFILES += src/memory/64bit_nix.c
endif

# Configuring Jasmine
JSMNCFLAGS += -DJSMN_STATIC
JSMNCFLAGS += -DJSMN_STRICT

all: clean runtime

runtime:
	@echo "Compiling runtime"
	@mkdir -p bin/
	@${CC} ${CC_OPTIONS} ${INCLUDES} ${CFLAGS} ${LDFLAGS} ${CFILES} ${JSMNCFLAGS} -L/usr/lib/ -luv $^ -o bin/${BINARY_NAME}

thirdparty:
	@echo "Compiling thirdparty"
	@make --no-print-directory -C thirdparty build

tools:
#	@echo "Compiling tools"
	@make --no-print-directory -C tools

clean:
	@rm -f core
	@echo "Cleaning up runtime"
	@rm -f bin/${BINARY_NAME}
#	@echo "Cleaning up tools"
#	@make --no-print-directory -C tools clean

distclean: clean
	@make --no-print-directory -C thirdparty clean

fetch:
	@git submodule update --init --recursive

init: fetch clean thirdparty runtime

.PHONY: all init clean fetch thirdparty runtime tools
