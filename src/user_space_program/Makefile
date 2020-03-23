# Enables debug messages while compiling
COMPILE_DEBUG=@

# Compile and Link flags, libraries
CC=$(CROSS_PREFIX)gcc
CFLAGS= -g -Wall 
LDFLAGS=
LIBS=

PROGS= user_prog

all: $(PROGS) 

# Add all object files to be linked in sequence
SYS_SH_OBJS:=

user_prog: $(SYS_SH_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) user_space_prog.c

%.o: %.c
	$(COMPILE_DEBUG)$(CC) $(CFLAGS) -c -o $@ $<
	$(COMPILE_DEBUG)echo "CC $<"

clean:
	rm -f *.o *.d *~ $(PROGS) 

