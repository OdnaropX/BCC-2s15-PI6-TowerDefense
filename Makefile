ifeq ($(OS),Windows_NT)
SHELL = cmd
endif

CC= gcc -std=c99

CFLAGS= -W -Wall -pedantic

ifeq ($(OS),Windows_NT)
LIBS = -lmingw32 -lSDL2main -lSDL2_image -lSDL2_ttf  -lSDL2 
else
LIBS = -lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2
endif

SOURCEDIR = src
ESTRUTURA = Estruturas

SOURCES = $(wildcard $(SOURCEDIR)/*/*.c $(SOURCEDIR)/*.c)
SUB_SOURCES = 
BUILDDIR = bin

ifeq ($(OS),Windows_NT)
	EXECUTABLE = main.exe
else
	EXECUTABLE = main
endif

OBJECTS = $(patsubst $(SOURCEDIR)/%.c,$(BUILDDIR)/%.o,$(SOURCES))

all: dir $(BUILDDIR)/$(EXECUTABLE)

ifeq ($(OS),Windows_NT)
dir:
else
dir:
		mkdir -p $(BUILDDIR)
endif

$(BUILDDIR)/$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $^ $(LIBS)

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

ifeq ($(OS),Windows_NT)

.PHONY: cls

clean:
		RMDIR /S $(BUILDDIR)/*o $(BUILDDIR)/$(EXECUTABLE)
else

.PHONY: clean

clean:
		rm -f $(BUILDDIR)/*o $(BUILDDIR)/$(EXECUTABLE)
endif