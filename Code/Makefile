EXEC= KirbyTrack

LSFML = -lsfml-graphics -lsfml-window -lsfml-system
CFLAGS = -Wall

all: $(EXEC)

KirbyTrack : KirbyTrack.o 
	g++ -o $@ $< `pkg-config opencv --libs` $(LSFML)

KirbyTrack.o : KirbyTrack.c 
	g++ -o $@ -c $< `pkg-config opencv --cflags`

clean :
	rm -f *.o $(EXEC)
