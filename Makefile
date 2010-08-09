TARGET = lci
LIBS = -lm
OBJS = lexer.o tokenizer.o parser.o interpreter.o unicode.o main.o
SRCS = lexer.c tokenizer.c parser.c interpreter.c unicode.c main.c
INSTALL = /usr/local/bin/install -c
CPPFLAGS = -O2

prefix = /usr/local
bindir = $(prefix)/bin
testdir = ./test

all: $(TARGET)

$(TARGET): $(OBJS) $(LIBS)
	$(CC) $(CPPFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

pedantic: $(OBJS) $(LIBS)
	$(CC) -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wundef -W -Wall -ansi -pedantic -g -o $(TARGET) $(OBJS) $(LIBS)

check: all
	@cd $(testdir) && ./testDir.sh -q ../$(TARGET) 1.2-Tests/

check-mem: all
	@echo "This will take a long time!  Be patient!"
	@cd $(testdir) && ./testDir.sh -q -m ../$(TARGET) 1.2-Tests/

install: all
	$(INSTALL) $(TARGET) $(bindir)/$(TARGET)

TAGS: $(SRCS)
	ctags $(SRCS)

docs: Doxyfile $(SRCS)
	doxygen

clean:
	-rm -f $(OBJS)
	-rm -f $(TARGET)

distclean: clean
	-rm -f tags
	-rm -rf html
