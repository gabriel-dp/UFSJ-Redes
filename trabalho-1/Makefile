# Compiler
CC = gcc

# Code directory structure
BINDIR := bin
BUILDDIR := build
INCDIR := include	
SRCDIR := src
UTILS = utils

# Compiler flags
CFLAGS = -Wall -g
LFLAGS = -lm


# Rule to create necessary directories
$(BUILDDIR) $(BINDIR):
	@ if [ ! -d ./$@ ]; then mkdir -p $@; fi

# Pattern rule to compile .c files to .o files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) -c $< -o $@ $(CFLAGS) -I $(INCDIR)

# Pattern rule to compile .o files to executables
$(BINDIR)/%: $(BUILDDIR)/%.o $(BUILDDIR)/$(UTILS).o | $(BINDIR)
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
