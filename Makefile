# Compiler settings
CC := i686-w64-mingw32-gcc
CXX := i686-w64-mingw32-g++

# Output file
OUTPUT := d3d9.dll
OUTPUT_COPY := "C:/Program Files (x86)/The Sims 2 Starter Pack/Fun with Pets/SP9/TSBin"

# Object files
OBJS := main.o common.o patches.o

# Compilation flags
CFLAGS := -Wall -I. -std=gnu99 -mms-bitfields -s -O2 -masm=intel -shared -Wl,--subsystem,windows,--kill-at,--enable-stdcall-fixup -DNO_ZLIB
CPPFLAGS := -Wall -O2 -std=c++17 -DTIXML_USE_STL -DNO_ZLIB
CXXFLAGS := -Wall -Wno-strict-aliasing -I. -O2 -std=c++17 -mms-bitfields -DTIXML_USE_STL -fpermissive
LDFLAGS := -static -L. d3d9.def
LIBS := -ld3d9 -lstdc++ -lversion -lpthread -lMinHook

# Makefile targets
.PHONY: all clean

# Build target
all: $(OUTPUT)

# Clean target
clean:
	rm -f $(OUTPUT) *.o

# Linking the final output
$(OUTPUT): $(OBJS)
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
	cp $(OUTPUT) $(OUTPUT_COPY)
