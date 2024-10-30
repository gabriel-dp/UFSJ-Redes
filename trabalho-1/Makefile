# Compiler
CC = gcc

# Code directory structure
BINDIR := bin
BUILDDIR := build
INCDIR := include	
SRCDIR := src

# Compiler flags
CFLAGS = -Wall -g
LFLAGS = 


# Default target
.PHONY: all
all: client server

# Rule to create necessary directories
$(BUILDDIR) $(BINDIR):
	@ if [ ! -d ./$@ ]; then mkdir -p $@; fi

# Pattern rule to compile .c files to .o files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) -c $< -o $@ $(CFLAGS) -I $(INCDIR)

# Rule to compile the client executable
client: $(BUILDDIR)/client.o $(BUILDDIR)/utils.o | $(BINDIR)
	$(CC) -o $(BINDIR)/client $(BUILDDIR)/client.o $(BUILDDIR)/utils.o $(LFLAGS)

# Rule to compile the server executable
server: $(BUILDDIR)/server.o $(BUILDDIR)/utils.o | $(BINDIR)
	$(CC) -o $(BINDIR)/server $(BUILDDIR)/server.o $(BUILDDIR)/utils.o $(LFLAGS)


# Clean up generated files
.PHONY: clean
clean:
	rm -rf $(BINDIR) $(BUILDDIR)
