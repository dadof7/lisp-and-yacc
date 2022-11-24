%{
#include "cell.h"
#include "y.tab.h"

extern int yylineno;
%}

%x IN_COMMENT

%%

<INITIAL>{
"/*"       BEGIN(IN_COMMENT);
}
<IN_COMMENT>{
"*/"      BEGIN(INITIAL);
[^*\n]+   // eat comment in chunks
"*"       // eat the lone star
\n        yylineno++;
}

\n    { yylineno++; }
--.*\n { yylineno++; }

[)(,] { return yytext[0]; }

(?i:create) { return CREATE; }
(?i:table) { return TABLE; }
(?i:default) { return DEFAULT; }
(?i:not) { return NOT; }
(?i:null) { return DBNULL; }
(?i:autoincrement) { return AUTOINCREMENT; }
(?i:references) { return REFERENCES; }
(?i:primary) { return PRIMARY; }
(?i:foreign) { return FOREIGN; }
(?i:key) { return KEY; }
(?i:char) { return CHAR; }
(?i:varchar) { return VARCHAR; }
(?i:decimal) { return DECIMAL; }
(?i:numeric) { return NUMERIC; }
(?i:date) { return DATE; }
(?i:datetime) { return DATETIME; }
(?i:int) { return INT; }
(?i:smallint) { return SMALLINT; }
(?i:money) { return MONEY; }

(?i:drop) { return DROP; }
(?i:unique) { return UNIQUE; }
(?i:index) { return INDEX; }
(?i:on) { return ON; }
(?i:insert) { return INSERT; }
(?i:into) { return INTO; }
(?i:values) { return VALUES; }

(?i:alter) { return ALTER; }
(?i:add) { return ADD; }

[0-9]+ { int v; sscanf(yytext, "%d",  &v); yylval = mk_intcell(v); return NUMINT; }
[0-9]*\.[0-9]+ { /* sscanf(yytext, "%f",  &yylval.float_val); */ return NUMFLOAT; }

[a-zA-Z][_a-zA-Z0-9]* { yylval = mk_symcell(yytext); return ID; }

'[^']*' { return STRING; }

. { ; }
%%

int yywrap()
{
  return 1;
}

void reset_flex()
{
  YY_FLUSH_BUFFER;
  BEGIN(INITIAL);
}
