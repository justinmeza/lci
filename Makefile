TARGET = lci
LIBS = -lm
OBJS = lexer.o tokenizer.o parser.o interpreter.o unicode.o main.o
SRCS = lexer.c tokenizer.c parser.c interpreter.c unicode.c main.c
HDRS = lexer.h tokenizer.h parser.h interpreter.h unicode.h
INSTALL = /usr/local/bin/install -c
CPPFLAGS = -O3
LINT = splint -nullret -temptrans -compdestroy -usereleased -compdef -compmempass -mustfreefresh -boolops -predboolint -nullpass -nullderef +boolint -predboolothers -uniondef -unqualifiedtrans -nullstate -bufferoverflowhigh -branchstate -mustfreeonly -nullassign -shiftimplementation -exportlocal

prefix = /usr/local
bindir = $(prefix)/bin
testdir = ./test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CPPFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

pedantic: $(OBJS)
	$(CC) -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wundef -Wall -ansi -pedantic -o $(TARGET) $(SRCS) $(HDRS) $(LIBS)

lint: all
	$(LINT) $(SRCS)

debug: $(OBJS)
	$(CC) -g -o $(TARGET) $(SRCS) $(LIBS)

check: all
	@cd $(testdir) && ./testDir.sh -q ../$(TARGET) 1.3-Tests

check-mem: all
	@echo "This will take a long time!  Be patient!"
	@cd $(testdir) && ./testDir.sh -q -m ../$(TARGET) 1.3-Tests

install: all
	$(INSTALL) $(TARGET) $(bindir)/$(TARGET)

TAGS: $(SRCS) $(HDRS)
	ctags $(SRCS) $(HDRS)

docs: Doxyfile $(SRCS)
	doxygen

clean:
	-rm -f $(OBJS)
	-rm -f $(TARGET)

distclean: clean
	-rm -f tags
	-rm -rf html
