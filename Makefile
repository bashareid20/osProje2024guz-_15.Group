HEADERS = shell.h
SOURCES = shell.c
OBJECTS = shell.o
TARGET  = shell
CC      = gcc
CFLAGS  = -Wall -g

default: $(TARGET)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)
