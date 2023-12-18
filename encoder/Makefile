CC = gcc
CFLAGS = -Wall -Wextra -fsanitize=address
LDLIBS = -lSDL2 -lSDL2_image -lm

OBJECTS = encoder.o

main: main $(OBJECTS)

clean:
	rm *.[od]
	rm main
