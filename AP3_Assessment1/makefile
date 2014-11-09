CC=gcc
CFLAGS =-c -W -Wall
LDFLAGS=
SOURCES=finddupl.c mlist.c mentry.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=finddupl

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(PROGS)
	find . -perm +100 -type f -delete
