OBJECTS = obj/control.o obj/message.o obj/serial.o
HEADERS = inc/control.h inc/message.h
CFLAGS  = -g -Wall
GLLIBS = -lglfw -lGLEW -lGL -lm

all: $(OBJECTS) helios

obj/control.o: src/control.c $(HEADERS)
	gcc $(CFLAGS) -c -o obj/control.o src/control.c $(GLLIBS)

obj/message.o: src/message.c $(HEADERS)
	gcc $(CFLAGS) -c -o obj/message.o src/message.c

obj/serial.o: src/serial.c $(HEADERS)
	gcc $(CFLAGS) -c -o obj/serial.o src/serial.c

helios: src/main.c $(OBJECTS) $(HEADERS)
	gcc $(CFLAGS) -o helios src/main.c $(OBJECTS) $(GLLIBS)

clean:
	rm -f obj/*
	rm -f helios
