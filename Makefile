par-shell: commandlinereader.o par-shell.o process_info.o QUEUE.o Xmalloc.o
	gcc -o par-shell par-shell.o commandlinereader.o process_info.o QUEUE.o Xmalloc.o

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc -Wall -g -c commandlinereader.c

par-shell.o: par-shell.c commandlinereader.h
	gcc -Wall -g -c par-shell.c

QUEUE.o: QUEUE.c QUEUE.h Xmalloc.o
	gcc -Wall -g -c QUEUE.c

process_info.o: process_info.c process_info.h Xmalloc.o
	gcc -Wall -g -c process_info.c

Xmalloc.o: Xmalloc.c Xmalloc.h
	gcc -Wall -g -c Xmalloc.c

clean:
	rm -f *.o par-shell