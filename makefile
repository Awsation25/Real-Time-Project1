

LIBS = -lglut -lGLU -lGL -lm
CC = gcc
OBJECTS = draw referee players

all: $(OBJECTS)

LIBS = -lglut -lGLU -lGL -lm
CC = gcc
OBJECTS = draw referee players

all: $(OBJECTS)

draw: draw.c
	$(CC) draw.c -o draw $(LIBS)

referee: referee.c
	$(CC) -g referee.c -o referee

players: players.c
	$(CC) -g players.c -o players


run: draw

	./draw

clean:
	rm -f *.o $(OBJECTS)

