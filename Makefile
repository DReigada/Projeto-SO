SDIR=src
UDIR=utils
IDIR=includes
ODIR=build
BDIR=bin
TDIR=test

_BUILD1 = QUEUE.o commandlinereader.o process_info.o Auxiliares.o threadFunction.o par-shell.o
BUILD1 = $(patsubst %,$(ODIR)/%,$(_BUILD1))

_BUILD2 = QUEUE.o process_info.o Auxiliares.o par-shell-terminal.o
BUILD2 = $(patsubst %,$(ODIR)/%,$(_BUILD2))

_DEPS = Auxiliares.h globalVariables.h QUEUE.h commandlinereader.h process_info.h threadFunction.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

MKDIR = build bin

all: directories par-shell par-shell-terminal

par-shell: $(BDIR)/par-shell

par-shell-terminal: $(BDIR)/par-shell-terminal

$(BDIR)/par-shell: $(BUILD1)
	gcc $(BUILD1) -o $@ -lpthread

$(BDIR)/par-shell-terminal: $(BUILD2)
	gcc $(BUILD2) -o $@ 

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	gcc -I./$(IDIR) -c -o $@ $<

$(ODIR)/%.o: $(UDIR)/%.c $(DEPS)
	gcc -I./$(IDIR) -c -o $@ $<

directories: ${MKDIR}

${MKDIR}:
	mkdir ${MKDIR}

run:
	./bin/par-shell

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
	rm -f -r $(ODIR) $(BDIR) $(TDIR)/fibonacci $(TDIR)/div0 log.txt par-shell-out-*.txt par-shell-in
