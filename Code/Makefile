EXEC= Navy

LSFML = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
CFLAGS = -Wall -g3

all: $(EXEC)

Navy : Navy.o 
	g++ -o $@ $< `pkg-config opencv --libs` $(LSFML) $(CFLAGS)

Navy.o : Navy.c 
	g++ -o $@ -c $< `pkg-config opencv --cflags` $(CFLAGS)

clean :
	rm -f *.o $(EXEC)
