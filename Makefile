par-shell: commandlinereader.o par-shell.o process_info.o QUEUE.o Auxiliares.o
	gcc -o par-shell -lpthread par-shell.o commandlinereader.o process_info.o QUEUE.o Auxiliares.o

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc -Wall -g -c commandlinereader.c

par-shell.o: par-shell.c commandlinereader.h
	gcc -Wall -g -c par-shell.c

QUEUE.o: QUEUE.c QUEUE.h Auxiliares.h
	gcc -Wall -g -c QUEUE.c

process_info.o: process_info.c process_info.h Auxiliares.o
	gcc -Wall -g -c process_info.c

Auxiliares.o: Auxiliares.c Auxiliares.h
	gcc -Wall -g -c Auxiliares.c

clean:
	rm -f *.o par-shell