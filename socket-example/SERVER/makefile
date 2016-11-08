OBJECTS = main.o listing.o clocks.o system.o init.o file.o
COMMH = /home/Jared/Assignment1/common.h

all: $(OBJECTS)
	cc -O $(OBJECTS) -o server

file.o: file.c
	cc -c -g file.c -L$(COMMH)
	
init.o: init.c
	cc -c -g init.c -L$(COMMH)

system.o: system.c
	cc -c -g system.c -L$(COMMH)
	
clocks.o: clocks.c
	cc -c -g clocks.c -L$(COMMH)
	
listing.o : listing.c
	cc -c -g listing.c -L$(COMMH)
	
main.o: main.c
	cc -c -g main.c -L$(COMMH)