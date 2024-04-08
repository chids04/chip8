CC = g++
CFLAGS= -I$(IDIR) -Wall -Wextra -pedantic
IDIR = include
SRCDIR = src
LFLAGS = -lSDL2
ODIR = build


_SRC = chip8.cpp cpu.cpp gpu.cpp keyboard.cpp
SRC = $(patsubst %,$(SRCDIR)/%,$(_SRC))

_DEPS = cpu.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = chip8.o cpu.o gpu.o keyboard.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SRCDIR)/%.cpp $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)

chip8: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ chip8


