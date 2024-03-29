CC=g++
CFLAGS=-I$(IDIR) -Wall -Wextra -pedantic
IDIR=include
SRCDIR=src

ODIR=build


_SRC = chip8.cpp cpu.cpp
SRC = $(patsubst %,$(SRCDIR)/%,$(_SRC))

_DEPS = cpu.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = chip8.o cpu.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SRCDIR)/%.cpp $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)

chip8: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) 

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ chip8


