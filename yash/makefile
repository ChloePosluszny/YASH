IDIR=include
CC=gcc
CFLAGS=-I$(IDIR) -lreadline

ODIR=src/obj

SDIR=src

_DEPS = parser.h execute.h common.h signal_handlers.h jobs.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = yash.o parser.o execute.o signal_handlers.o jobs.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

yash: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ 
