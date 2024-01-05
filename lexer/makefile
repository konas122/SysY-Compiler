.PHONY: lc, test1, test2, test3, clean, distclean

lc:
	flex lexer.l
	gcc lex.yy.c -o lc.out
	./lc.out

test1:
	flex lexer.l
	gcc lex.yy.c -o lc.out
	./lc.out < test/test1.c

test2:
	flex lexer.l
	gcc lex.yy.c -o lc.out
	./lc.out < test/test2.c

test3:
	flex lexer.l
	gcc lex.yy.c -o lc.out
	./lc.out < test/test3.c

test4:
	flex lexer.l
	gcc lex.yy.c -o lc.out
	./lc.out < test/test4.c

clean_out:
	rm -f *.out

clean:
	rm -f lex.yy.c
	rm -f *.out

# debug:
# 	flex sysy.l
# 	gcc lex.yy.c -o lc.out
# 	./lc.out < test4.c
