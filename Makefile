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

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS) directories
	gcc -I./$(IDIR) -c -o $@ $<

$(ODIR)/%.o: $(UDIR)/%.c $(DEPS) directories
	gcc -I./$(IDIR) -c -o $@ $<

par-shell: $(BUILD) directories
	gcc $(BUILD) -o $(addprefix $(BDIR)/,$@) -lpthread

directories: ${MKDIR}

${MKDIR}:
	mkdir ${MKDIR}

.PHONY: clean

clean:
	rm -f -r $(ODIR) $(BDIR)