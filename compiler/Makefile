CC = g++
FLEX = flex
BISON = bison

TARGET = compiler
LEXER = lexer.l
PARSER = parser.y

OBJS = lex.yy.o parser.tab.o main.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

lex.yy.c: $(LEXER) parser.tab.h
	$(FLEX) $(LEXER)

parser.tab.c parser.tab.h: $(PARSER)
	$(BISON) -d $(PARSER)

%.o: %.c
	$(CC) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS) lex.yy.c parser.tab.c parser.tab.h