par-shell: commandlinereader.o process_info.o QUEUE.o Auxiliares.o threadFunction.o par-shell.o
	gcc -o par-shell par-shell.o commandlinereader.o process_info.o QUEUE.o Auxiliares.o threadFunction.o

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc -Wall -g -c commandlinereader.c

par-shell.o: par-shell.c globalVariables.h threadFunction.o commandlinereader.o QUEUE.o process_info.o Auxiliares.o
	gcc -Wall -g -c par-shell.c

QUEUE.o: QUEUE.c QUEUE.h Auxiliares.o
	gcc -Wall -g -c QUEUE.c

process_info.o: process_info.c process_info.h 
	gcc -Wall -g -c process_info.c

Auxiliares.o: Auxiliares.c Auxiliares.h process_info.o
	gcc -Wall -g -c Auxiliares.c

threadFunction.o: threadFunction.c threadFunction.h QUEUE.o process_info.o Auxiliares.o
	gcc -Wall -g -c threadFunction.c

clean:
	rm -f *.o par-shell

