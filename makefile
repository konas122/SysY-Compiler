CC := g++
CFLAGS := -w -Wextra -std=c++11
TARGET := ./build/main

all: run

src/main.l.yy.c: src/sysy.l
	flex --noyywrap -o src/main.l.yy.cpp  src/sysy.l

src/main.tab.c: src/sysy.y
	bison -o src/main.tab.cpp --defines=src/main.tab.h -v src/sysy.y

src/global.h.gch: src/global.h
	g++ -x c++-header -o src/global.h.gch -c src/global.h

lex: src/main.l.yy.c

yacc: src/main.tab.c

main: src/global.h.gch
	$(CC) $(CFLAGS) $(shell ls ./src/*.cpp) -o ./build/main

.PHONY: all clean main run lex yacc debug

run: lex yacc main

clean:
	rm -f src/*.output src/main.l.yy.cpp src/main.tab.cpp src/main.tab.h src/main.output src/global.h.gch $(TARGET) *.o ./build/* 

syntax: run
	./build/main test/syntax/test.c > result.txt

syntax0: run
	./build/main test/syntax/test0.c > result.txt

syntax1: run
	./build/main test/syntax/test1.c > result.txt

syntax2: run
	./build/main test/syntax/test2.c > result.txt

syntax3: run
	./build/main test/syntax/test3.c > result.txt

syntax4: run
	./build/main test/syntax/test4.c > result.txt

syntax5: run
	./build/main test/syntax/test5.c > result.txt


check1: run
	./build/main test/semantic/test1.c > result.txt

check2: run
	./build/main test/semantic/test2.c > result.txt

check3: run
	./build/main test/semantic/test3.c > result.txt

check4: run
	./build/main test/semantic/test4.c > result.txt

check5: run
	./build/main test/semantic/test5.c > result.txt

check6: run
	./build/main test/semantic/test6.c > result.txt

check7: run
	./build/main test/semantic/test7.c > result.txt
