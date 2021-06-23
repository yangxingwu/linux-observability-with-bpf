CLANG := clang

OUTPUT := .output
BPFTOOL := $(abspath ../../tools/bpftool)

APP := minimal

# Use our own libbpf API headers and Linux UAPI headers distributed with
# libbpf to avoid dependency on system-wide headers, which could be missing or
# outdated
INCLUDES := -I$(abspath $(OUTPUT)) -I$(abspath ../../libbpf/include/uapi)

# Get Clang's default includes on this system. We'll explicitly add these dirs
# to the includes list when compiling with `-target bpf` because otherwise some
# architecture-specific dirs will be "missing" on some architectures/distros -
# headers such as asm/types.h, asm/byteorder.h, asm/socket.h, asm/sockios.h,
# sys/cdefs.h etc. might be missing.
#
# 以下是一个具体的错误信息
# make: Entering directory '/home/vagrant/linux-observability-with-bpf/chapter2/hello_world'
# clang -target bpf -I.output -I../../libbpf/include/uapi -c bpf_program.c -o bpf_program.o
# In file included from bpf_program.c:1:
# In file included from ../../libbpf/include/uapi/linux/bpf.h:11:
# /usr/include/linux/types.h:5:10: fatal error: 'asm/types.h' file not found
# #include <asm/types.h>
#          ^~~~~~~~~~~~~
# 1 error generated.
# make: *** [Makefile:31: bpf_program.o] Error 1
# make: Leaving directory '/home/vagrant/linux-observability-with-bpf/chapter2/hello_world'
#
# Use '-idirafter': Don't interfere with include mechanics except where the
# build would have failed anyways.
CLANG_BPF_SYS_INCLUDES = $(shell $(CLANG) -v -E - </dev/null 2>&1 \
			 | sed -n '/<...> search starts here:/,/End of search list./{ s| \(/.*\)|-idirafter \1|p }')

.PHONY: all
all: $(APP)

$(OUTPUT) $(OUTPUT)/libbpf:
	mkdir -p $@

# build libbpf
LIBBPF_OBJ := $(abspath $(OUTPUT)/libbpf.a)
LIBBPF_SRC_DIR := $(abspath ../../libbpf/src)
LIBBPF_SRC := $(wildcard $(LIBBPF_SRC_DIR)/*.c)
LIBBPF_HEADER := $(wildcard $(LIBBPF_SRC_DIR)/*.h)
$(LIBBPF_OBJ): $(LIBBPF_SRC) $(LIBBPF_HEADER) $(LIBBPF_SRC_DIR)/Makefile | $(OUTPUT)/libbpf
	$(MAKE) -C $(LIBBPF_SRC_DIR) BUILD_STATIC_ONLY=1	\
		OBJDIR=$(dir $@)libbpf DESTDIR=$(dir $@)	\
		INCLUDEDIR= LIBDIR= UAPIDIR=			\
		install

# build BPF code
$(OUTPUT)/%.bpf.o: %.bpf.c | $(LIBBPF_OBJ) $(OUTPUT)
	clang -target bpf $(INCLUDES) $(CLANG_BPF_SYS_INCLUDES) -c $< -o $@

# build user-space code
$(OUTPUT)/%.skel.h: $(OUTPUT)/%.bpf.o | $(OUTPUT)
	$(BPFTOOL) gen skeleton $< > $@

$(OUTPUT)/%.o: %.c $(OUTPUT)/%.skel.h | $(OUTPUT)
	$(CC) $(INCLUDES) -c $< -o $@

$(APP): %: $(OUTPUT)/%.o $(LIBBPF_OBJ) | $(OUTPUT)
	$(CC) $^ -lelf -lz -o $@

.PHONY: clean
clean:
	@rm -rf $(OUTPUT)

.DELETE_ON_ERROR:

.SECONDARY:
