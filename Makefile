ODIR=build
SDIR=src
UDIR=utils
IDIR=includes

_BUILD = QUEUE.o commandlinereader.o process_info.o Auxiliares.o threadFunction.o par-shell.o
BUILD = $(patsubst %,$(ODIR)/%,$(_BUILD))

_DEPS = Auxiliares.h globalVariables.h QUEUE.h commandlinereader.h process_info.h threadFunction.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	gcc -I./$(IDIR) -c -o $@ $<

$(ODIR)/%.o: $(UDIR)/%.c $(DEPS)
	gcc -I./$(IDIR) -c -o $@ $<

par-shell: $(BUILD)
	gcc $^ -o $@ -lpthread

clean:
	rm -f $(ODIR)/* par-shell