CC=gcc
CFLAGS=-Wall -g
LDFLAGS=
EXEC=moteur

all: $(EXEC)

moteur : moteur.c
	gcc -o $@ $^ 

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
