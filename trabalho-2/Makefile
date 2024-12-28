# Compiler
CC = gcc

# Code directory structure
BINDIR := bin
BUILDDIR := build
INCDIR := include	
SRCDIR := src
UTILS := hangman utils

# Compiler flags
CFLAGS = - Wall # -g -ggdb3
LFLAGS = 

# Rule to create necessary directories
$(BUILDDIR) $(BINDIR):
	@ if [ ! -d ./$@ ]; then mkdir -p $@; fi

# Generate object files for utilities
UTIL_OBJS := $(patsubst %,$(BUILDDIR)/%.o,$(UTILS))

# Pattern rule to compile .c files to .o files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) -c $< -o $@ $(CFLAGS) -I $(INCDIR)

# Pattern rule to compile .o files to executables
$(BINDIR)/%: $(BUILDDIR)/%.o $(UTIL_OBJS) | $(BINDIR)
	$(CC) -o $@ $^ $(LFLAGS)

# Pattern rule to compile the specified file using 'make <file_name>'
%: $(BINDIR)/%
	@echo "Building $@..."

# Keep object files after compilation
OBJECTS := $(patsubst %,$(BUILDDIR)/%.o,$(NAMES))
.SECONDARY: $(OBJECTS)

# Clean up generated files
.PHONY: all clean
clean:
	rm -rf $(BINDIR) $(BUILDDIR)
