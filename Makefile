# Compiler and flags
CC = gcc
CFLAGS = -Wall -Werror -g -O2

# Directories
SRCDIR = src

# Find all .c files in the source directory
SRCS = $(wildcard $(SRCDIR)/*.c)
# Replace .c with .o to create a list of object files
OBJS = $(SRCS:.c=.o)

# Use pkg-config to find the correct flags for the libraries
# Add -I$(SRCDIR) to include our own source directory for header files
LIBS = $(shell pkg-config --libs concord curl jansson)
INCS = $(shell pkg-config --cflags concord curl jansson) -I$(SRCDIR)

# Target executable name
TARGET = discord-bot

# Default rule
all: $(TARGET)

# Rule to link the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCS) -o $(TARGET) $(OBJS) $(LIBS)

# Rule to compile .c files into .o (object) files
$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

# Rule to clean up build files
clean:
	rm -f $(TARGET) $(OBJS)

