OBJDIR = obj
OBJECTS = $(addprefix $(OBJDIR)/, main.o generic.o server.o client.o sem.o threadpool.o cygbarrier.o)
HEADH = -I/home/Jared/3420ICT/Assignment2/head
TARGET = bin/threading
CC = gcc
CFLAGS = -c -g
OFLAGS = -O3

vpath %.c src
vpath %.h head


all: $(OBJECTS)
	@$(CC) $(OFLAGS) $(OBJECTS) -o $(TARGET)
	@echo Compiled to $(TARGET)

$(OBJDIR)/%.o : %.c
	@$(CC) $(CFLAGS) $< $(HEADH) -o $@
	