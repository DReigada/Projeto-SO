VPATH = utils src includes

organize: organize_executables
	mkdir -p build
	mv *.o build

organize_executables: par-shell
	mkdir -p bin
	mv $< bin/

par-shell: commandlinereader.o QUEUE.o process_info.o Auxiliares.o threadFunction.o par-shell.o
	gcc $^ -o $@ -lpthread

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc -I./includes -Wall -g -c $< 

QUEUE.o: QUEUE.c QUEUE.h 
	gcc -I./includes -Wall -g -c $< 

process_info.o: process_info.c process_info.h 
	gcc -I./includes -Wall -g -c $<

Auxiliares.o: Auxiliares.c Auxiliares.h
	gcc -I./includes -Wall -g -c $<

threadFunction.o: threadFunction.c threadFunction.h 
	gcc -I./includes -Wall -g -c $<

par-shell.o: par-shell.c globalVariables.h 
	gcc -I./includes -Wall -g -c $<

clean:
	rm -f -r bin/ build/

