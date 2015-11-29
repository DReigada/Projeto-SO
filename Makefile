SDIR=src
UDIR=utils
IDIR=includes
ODIR=build
BDIR=bin
TDIR=test

_SHELL-BUILD = QUEUE.o commandlinereader.o process_info.o Auxiliares.o Auxiliares-par-shell.o threadFunction.o par-shell.o
SHELL-BUILD = $(patsubst %,$(ODIR)/%,$(_SHELL-BUILD))

_TERMINAL-BUILD = Auxiliares.o par-shell-terminal.o
TERMINAL-BUILD =  $(patsubst %,$(ODIR)/%,$(_TERMINAL-BUILD))

_DEPS = Auxiliares.h Auxiliares-par-shell.h globalVariables.h QUEUE.h commandlinereader.h process_info.h threadFunction.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

MKDIR = build bin

all: directories execs

execs: $(BDIR)/par-shell $(BDIR)/par-shell-terminal

$(BDIR)/par-shell: $(SHELL-BUILD)
	gcc $(SHELL-BUILD) -o $@ -lpthread

$(BDIR)/par-shell-terminal: $(TERMINAL-BUILD)
	gcc $(TERMINAL-BUILD) -o $@ -lpthread

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	gcc -I./$(IDIR) -c -o $@ $<

$(ODIR)/%.o: $(UDIR)/%.c $(DEPS)
	gcc -I./$(IDIR) -c -o $@ $<

directories: ${MKDIR}

${MKDIR}:
	mkdir ${MKDIR}

server:
	./bin/par-shell

client:
	./bin/par-shell-terminal ./bin/par-shell-in

test1: all $(TDIR)/fibonacci
	$(BDIR)/par-shell < $(TDIR)/input1.test

test2: all $(TDIR)/fibonacci $(TDIR)/div0
	$(BDIR)/par-shell < $(TDIR)/input2.test

$(TDIR)/fibonacci: $(TDIR)/fibonacci.c
	gcc -o $(TDIR)/fibonacci $(TDIR)/fibonacci.c

$(TDIR)/div0: $(TDIR)/div.c
	gcc -o $(TDIR)/div0 $(TDIR)/div.c

.PHONY: clean

clean:
	rm -f -r $(ODIR) $(BDIR) $(TDIR)/fibonacci $(TDIR)/div0 log.txt par-shell-out-*.txt
