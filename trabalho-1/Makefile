# Makefile by gabriel-dp!

# Defines C Compiler
CC := gcc

# Code directory structure
BINDIR := bin
BUILDDIR := build
SRCDIR := src

# Compiler flags
CFLAGS := -Wall

# Linker flags
LDFLAGS := -lm

# %.o file names
NAMES := $(notdir $(basename $(wildcard $(SRCDIR)/*.c)))
OBJECTS := $(patsubst %,$(BUILDDIR)/%.o,$(NAMES))
BINARIES := $(patsubst %,$(BINDIR)/%,$(NAMES))

# Keep object files after compilation
.SECONDARY: $(OBJECTS)

# Rule to build all executables
all: $(BINARIES)


# Rule to create individual executables for each .c file
$(BINDIR)/%: $(BUILDDIR)/%.o
	@ if [ ! -d ./$(BINDIR) ]; then mkdir -p $(BINDIR); fi
	$(CC) -o $@ $< $(LDFLAGS)

# Rule for object binaries compilation
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@ if [ ! -d ./$(BUILDDIR) ]; then mkdir -p $(BUILDDIR); fi
	$(CC) -c $^ -o $@ $(CFLAGS) 


# Clean BIN and BUILD dirs
.PHONY: clean
clean: 
	rm -rf $(BUILDDIR) $(BINDIR)
