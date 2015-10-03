par-shell: commandlinereader.o par-shell.o
	gcc -o par-shell par-shell.o commandlinereader.o

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc -Wall -g -c commandlinereader.c

par-shell.o: par-shell.c commandlinereader.h
	gcc -Wall -g -c par-shell.c

clean:
	rm -f *.o par-shell