CFLAGS = -I. -Wall -g -fPIC -Og -std=gnu99  
LIBS += -lthemis

all: rd_themis.so

rd_themis.o: src/rd_themis.c
	$(CC) $(CFLAGS) -c -o $@ src/rd_themis.c

rd_themis.so: rd_themis.o 
	$(LD) -o $@ rd_themis.o -shared $(LIBS) -lc 

clean:
	rm -rf *.so *.o

test: all
	./test.sh