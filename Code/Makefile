EXEC= KirbyTrack

LSFML = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
CFLAGS = -Wall -g3

all: $(EXEC)

KirbyTrack : KirbyTrack.o 
	g++ -o $@ $< `pkg-config opencv --libs` $(LSFML) $(CFLAGS)

KirbyTrack.o : KirbyTrack.c 
	g++ -o $@ -c $< `pkg-config opencv --cflags` $(CFLAGS)

clean :
	rm -f *.o $(EXEC)
