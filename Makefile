OBJS = main.o global.o cons.o hash.o sym.o str.o fun.o print.o eval.o token.o read.o y.tab.o lex.yy.o
CC = cc -g -c

lisp: $(OBJS)
	cc -o $@ -g $(OBJS)

cell.h: hash.h
	touch $@

main.o: main.c cell.h token.h print.h eval.h read.h global.h 
	$(CC) $<

global.o: global.c cell.h token.h read.h global.h read.h print.h
	$(CC) $<

cons.o: cons.c cell.h global.h
	$(CC) $<

sym.o: sym.c cell.h global.h
	$(CC) $<

hash.o: hash.c hash.h global.h
	$(CC) $<

str.o: str.c cell.h global.h
	$(CC) $<

fun.o: fun.c cell.h global.h eval.h
	$(CC) $<

print.o: print.c cell.h print.h
	$(CC) $<

eval.o: eval.c cell.h global.h print.h
	$(CC) $<

token.o: token.c cell.h token.h global.h
	$(CC) $<

read.o: read.c cell.h token.h read.h global.h
	$(CC) $<


y.tab.c: yacc.y
	yacc -d $<

lex.yy.c: lex.l y.tab.h 
	flex $<

clean:
	rm -f $(OBJS) *.tar

wc:
	wc main.c global.c cons.c hash.c sym.c str.c fun.c print.c eval.c token.c read.c lex.l yacc.y cell.h eval.h global.h hash.h print.h read.h token.h 


tar: clean
	tar fc - *.c *.h Makefile *.l > "lisp-`date '+%Y-%m-%d.%H.%M.%S'`.tar"


# $@ : ターゲットファイル名
# $% : ターゲットがアーカイブメンバだったときのターゲットメンバ名
# $< : 最初の依存するファイルの名前
# $? : ターゲットより新しいすべての依存するファイル名
# $^ : すべての依存するファイルの名前
# $+ : Makefileと同じ順番の依存するファイルの名前
# $* : サフィックスを除いたターゲットの名前
