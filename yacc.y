%{
#include <stdio.h>
#include <setjmp.h>
  
#include "cell.h"
#include "global.h"  
  
extern void yyerror(char*);
extern int yylex(void);

#define YYSTYPE cell_ptr
extern cell_ptr parse_result;
%}

%token CREATE
%token TABLE
%token DEFAULT
%token NOT
%token DBNULL
%token AUTOINCREMENT
%token REFERENCES
%token PRIMARY
%token FOREIGN
%token KEY
%token CHAR
%token VARCHAR
%token DECIMAL
%token NUMERIC
%token INT
%token SMALLINT
%token MONEY

%token DROP
%token UNIQUE
%token INDEX
%token ON
%token INSERT
%token INTO
%token VALUES
%token ALTER
%token ADD

%token NUMINT
%token NUMFLOAT
%token ID
%token STRING
%token DATE
%token DATETIME

%%
start: commands { parse_result = $1; }
     ;

commands: command { $$ = cons($1,NIL); }
        | command commands { $$ = cons($1,$2); }
        ;

command: CREATE TABLE ID '(' column_defs ')' { $$ = cons($3, $5); }
       | DROP TABLE ID { $$ = cons(NIL,mk_symcell("DROP")); }
       | CREATE unique INDEX ID ON ID '(' id_list ')' { $$ = cons(NIL,mk_symcell("INDEX")); }
       | INSERT INTO ID '(' id_list ')' VALUES '(' val_list ')' { $$ = cons(NIL,mk_symcell("INSERT")); }
       | ALTER TABLE ID ADD ID type_dec descriptions { $$ = cons(mk_symcell(":ALTER"), cons($3, cons($5, cons($6,$7)))); }
       ;

id_list: ID
       | ID ',' id_list
       ;

val_list: val
        | val ',' val_list
	;

val: NUMINT
   | STRING
   ;

unique: /* empty */
      | UNIQUE
      ;

column_defs: column_def { $$ = cons($1,NIL); }
           | column_def opt_comma column_defs { $$ = cons($1,$3); }
           ;

opt_comma: /* empty */
         | ','
	 ;

column_def: ID type_dec descriptions { $$ = cons($1, cons($2,$3)); }
          | PRIMARY KEY '(' ID ')' { $$ = cons(mk_symcell(":primary-key"), $4); }
          | FOREIGN KEY '(' ID ')' REFERENCES ID '(' ID ')' { $$ = cons(mk_symcell(":foreign-key"), cons($3,$6)); }
          ;

type_dec: CHAR '(' NUMINT ')' { $$ = cons(mk_symcell(":type"), cons(mk_symcell("char"), $3)); }
        | VARCHAR '(' NUMINT ')' { $$ = cons(mk_symcell(":type"), cons(mk_symcell("varchar"), $3)); }
	| DECIMAL '(' NUMINT ',' NUMINT ')' { $$ = cons(mk_symcell(":type"), cons(mk_symcell("decimal"), cons($3,$5))); }
	| NUMERIC '(' NUMINT ',' NUMINT ')' { $$ = cons(mk_symcell(":type"), cons(mk_symcell("numeric"), cons($3,$5))); }	
	| INT { $$ = cons(mk_symcell(":type"), cons(mk_symcell("int"),NIL)); }
	| SMALLINT { $$ = cons(mk_symcell(":type"), cons(mk_symcell("smallint"),NIL)); }
	| MONEY { $$ = cons(mk_symcell(":type"), cons(mk_symcell("money"),NIL)); }
	| DATE { $$ = cons(mk_symcell(":type"), cons(mk_symcell("date"),NIL)); }
	| DATETIME { $$ = cons(mk_symcell(":type"), cons(mk_symcell("datetime"),NIL)); }
        ;

descriptions: /* empty */ { $$ = NIL; }
            | description descriptions { $$ = cons($1,$2); }
	    ;

description: nullable { $$ = cons(mk_symcell(":not-null"),$1); }
           | DEFAULT AUTOINCREMENT { $$ = cons(mk_symcell(":autoincrement"),true_sym); }
           | DEFAULT val { $$ = cons(mk_symcell(":default"),true_sym); }	   
           | REFERENCES ID '(' ID ')' { $$ = cons(mk_symcell(":references"), cons($2,$4)); }
           ;

nullable: NOT DBNULL { $$ = true_sym; }
        | DBNULL { $$ = NIL; }
	;


%%

extern int yylineno;


void yyerror(s)
     char *s;
{
  fprintf(stderr, "*ERROR* %s in line %d\n", s, yylineno);
  parse_result = NIL;
}


/*
int main() {
  yyin = stdin;
  yyparse();
}
*/
