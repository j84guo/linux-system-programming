BINDIR := bin
SRCDIR := .

SOURCES := $(shell find . -type f -name '*.c')
EXECUTABLES := $(patsubst ${SRCDIR}/%, ${BINDIR}/%,$(SOURCES:.c=))

all: ${EXECUTABLES}
	@echo "executables: $^"

${BINDIR}/%: ${SRCDIR}/%.c
	@mkdir -p ${BINDIR}	
	@echo "compiling: $@"
	gcc -o $@ $<
