#### HOW TO USE ####
# To add a file to cassini or saturnd :
# - add the dir/name.o to the line CASSINI_OBJFILES or SATURND_OBJFILES
# - don't forget the .o instead of .c => the object files are created
#   automatically from the source files
#
# Don't touch anything else, the variables are here to do the work for you

# Right now there are no files for saturnd : if you try to make it
# or if you try to make all, there will be a fatal error


CC ?= gcc
CLFAGS ?= -Wall

# tell the linker where to find the .h files
LIBINCLUDE = include
CFLAGS += -I$(LIBINCLUDE)

# all the object files cassini needs
CASSINI_OBJFILES = src/cassini.o src/timing-text-io.o src/print-reply.o src/pipes.o src/read-reply.o src/write-request.o

# all the object files saturnd needs
SATURND_OBJFILES =


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# compile both the client and the daemon
all: cassini saturnd

# the client
cassini: $(CASSINI_OBJFILES)
	$(CC) $(CFLAGS) -o cassini $(CASSINI_OBJFILES)

# the daemon
saturnd: $(SATURND_OBJFILES)
	$(CC) $(CFLAGS) -o saturnd $(SATURND_OBJFILES)

# to remove the compiled files
.PHONY: clean
clean:
	rm -f cassini $(CASSINI_OBJFILES)
	rm -f saturnd $(SATURND_OBJFILES)
