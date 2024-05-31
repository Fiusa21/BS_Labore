CC=gcc
CFLAGS= -D_REENTRANT
DEPS= 
PROGRAMS= signals-self signals-term signals-overkill pipe mmap_anon process_vm_readv

all: $(PROGRAMS)

clean:
	-rm $(PROGRAMS)
output:
	ls -la  $(PROGRAMS) 

%: %c.c $(DEPS)
	$(CC) -o $@ $< $(CFLAGS)


