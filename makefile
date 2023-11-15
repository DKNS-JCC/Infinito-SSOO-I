CC = gcc
CFLAGS = -Wall -Wextra -std=c11
SRC = infinito.c
OBJ = $(SRC:.c=.o)
EXECUTABLE = infinito

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ) 
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXECUTABLE)
