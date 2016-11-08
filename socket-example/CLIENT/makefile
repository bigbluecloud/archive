OBJECTS = main.o init.o time.o file.o sys.o display.o
COMMH = /home/Jared/Assignment1/common.h

all: $(OBJECTS)
	cc -O $(OBJECTS) -o client

display.o: display.c
	cc -c -g display.c -L$(COMMH)

sys.o: sys.c
	cc -c -g sys.c -L$(COMMH)
	
file.o: file.c
	cc -c -g file.c -L$(COMMH)
	
time.o: time.c
	cc -c -g time.c -L$(COMMH)

init.o: init.c
	cc -c -g init.c -L$(COMMH)

main.o: main.c
	cc -c -g main.c -L$(COMMH)