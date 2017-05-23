EXEC= KirbyTrack Navy

LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio `pkg-config opencv --libs`
CFLAGS = -Wall -g3 -pg `pkg-config opencv --cflags`

all: $(EXEC)

KirbyTrack : KirbyTrack.o 
	g++ -o $@ $< $(LDFLAGS) $(CFLAGS)

KirbyTrack.o : KirbyTrack.c 
	g++ -o $@ -c $<  $(CFLAGS)

Navy : Navy.o fonction.o
	g++ -o $@ $^  $(LDFLAGS) $(CFLAGS)

Navy.o : Navy.c fonction.h
	g++ -o $@ -c $<  $(CFLAGS)

fonction.o : fonction.c fonction.h
	g++ -o $@ -c $<  $(CFLAGS)

clean :
	rm -f *.o $(EXEC)
