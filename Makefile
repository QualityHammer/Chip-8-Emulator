BDIR = build
IDIR = include
SDIR = src
ODIR = $(SDIR)/obj
LDIR = lib
LIBS = -lSDL2

CXX = g++
CXXFLAGS = -Wall -g -I$(IDIR) -std=c++17

_DEPS = chip8.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o chip8.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(BDIR)/chip8: $(OBJ)
	if [ ! -d "build" ]; then mkdir build; fi
	$(CXX) $(CXXFLAGS) $(LIBS) -o $@ $^

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~

