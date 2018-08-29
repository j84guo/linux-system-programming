.PHONY: all clean

SRCEXT := c
SRCDIR := src
BINDIR := bin
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
EXECUTABLES := $(patsubst $(SRCDIR)/%, $(BINDIR)/%, $(SOURCES:.$(SRCEXT)=))

all: $(EXECUTABLES)

$(BINDIR)/%: $(SRCDIR)/%.$(SRCEXT)
	mkdir -p bin/
	gcc -lpthread -o $@ $<

clean:
	rm -rf bin
