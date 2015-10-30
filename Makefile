SDIR=src
UDIR=utils
IDIR=includes
ODIR=build
BDIR=bin

_BUILD = QUEUE.o commandlinereader.o process_info.o Auxiliares.o threadFunction.o par-shell.o
BUILD = $(patsubst %,$(ODIR)/%,$(_BUILD))

_DEPS = Auxiliares.h globalVariables.h QUEUE.h commandlinereader.h process_info.h threadFunction.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

MKDIR = build bin

all: directories par-shell

par-shell: $(BDIR)/par-shell

$(BDIR)/par-shell: $(BUILD) 
	gcc $(BUILD) -o $@ -lpthread

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	gcc -I./$(IDIR) -c -o $@ $<

$(ODIR)/%.o: $(UDIR)/%.c $(DEPS)
	gcc -I./$(IDIR) -c -o $@ $<

directories: ${MKDIR}

${MKDIR}:
	mkdir ${MKDIR}

.PHONY: clean

test1: fibonacci
	$(BDIR)/par-shell < test/input1.test

test2: fibonacci div0
	$(BDIR)/par-shell < test/i2.test

fibonacci: test/fibonacci.c
	gcc -o test/fibonacci test/fibonacci.c

div0: test/div.c
	gcc -o test/div0 test/div.c

clean:
	rm -f -r $(ODIR) $(BDIR)

