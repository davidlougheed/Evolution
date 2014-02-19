CC=g++
CFLAGS=-L/opt/local/lib -lSDLmain -lSDL -Wl,-framework,Cocoa -framework OpenGL -Wall -O3 -I/opt/local/include/SDL -D_GNU_SOURCE=1 -D_THREAD_SAFE
SOURCES=main.cpp SDLMain.m

all:
	$(CC) -o evol $(SOURCES) $(CFLAGS)