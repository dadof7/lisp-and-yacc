#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include "cell.h"
#include "token.h"
#include "global.h"

static char* token_buff;
static int token_buff_ptr;
static int get_token_state;
#define is_digit(c) ('0' <= c && c <= '9')
#define is_alpha(c) (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c=='!' || c=='<' || c=='>' || c=='+' || c=='-' || c==':' || c=='*' || c=='/' || c=='=' || c=='#' || c=='_' || c=='?' || c=='@' || c=='&' || c=='$' || c=='%')
#define is_ws(c) (c==' ' || c=='\t' || c=='\n')

static FILE* input_file;
int _stdnextchar() 
{
  int c;
  c = fgetc(input_file);
  // if(c == '\n') printf("> ");
  return c;
}

int _buffnextchar()
{
  int c;
  c = token_buff[token_buff_ptr++];
  return c;
}

static int (*nextchar)() = &_stdnextchar;

void set_token_fromf(FILE* inf)
{
  input_file = inf;
  nextchar = &_stdnextchar;
  get_token_state = 0;
}

void set_token_buff(char* buff)
{
  nextchar = &_buffnextchar;
  token_buff = buff;
  token_buff_ptr = 0;
  get_token_state = 0;
}

token_t* get_token() {
  static token_t token;
  char c;
  char str[TOKEN_STR_SZ+1];
  int str_ptr;
  static int pushback= 0;

  while(1) {
    if(pushback) {
      c = pushback;
      pushback = 0;
    }
    else {
      c = (*nextchar)();
    }

    if(c == EOF) return (void*)0;

    switch(get_token_state) {
    case 0:
      if(c == '\0') {
	get_token_state = 0;
	pushback = 0;
	return (void*)0;
	// longjmp(top, e_eostr); // need more tokens
      }
      else if(is_ws(c)) {
	// if(c == '\n') printf(">> ");
      }
      else if(c == ',') {
	get_token_state = 200;
      }
      else if(c == '(' || c == ')' || c == '\'' || c == '.' || c =='`') {
	token.kind = c;
	return &token;
      }
      else if(is_digit(c) || is_alpha(c)) { // 普通の文字か数字を見た
	*str = c;
	str_ptr = 1;
	get_token_state = 1;
      }
      else if(c == '"') {
	str_ptr = 0;
	get_token_state = 100;
      }
      else if(c == ';') {
	get_token_state = 2;
      }
      else {
	get_token_state = 0;
	pushback = 0;
	longjmp(top, e_bogus); // bogus input
      }
      break; // 0

    case 200: // seen ,
      if(c == '@') {
	token.kind = '@'; // ,@
      }
      else {
	token.kind = ',';
	pushback = c;
      }
      get_token_state = 0;
      return &token;
      break;
      
    case 100: // 最初の"を見た後
      if(c != '"') {
	if(str_ptr < TOKEN_STR_SZ) str[str_ptr++] = c;
      }
      else { // 終わりの"を見た
	str[str_ptr] = '\0';
	get_token_state = 0;
	token.kind = 'G';
	strncpy(token.str, str, TOKEN_STR_SZ);
	token.str[TOKEN_STR_SZ] = '\0';
	return &token;
      }
      break; // 100
      
    case 1:
      if(is_digit(c) || is_alpha(c) || c=='.') {
	if(str_ptr < TOKEN_STR_SZ) str[str_ptr++] = c;
      }
      else { // symbolか数字が切り出せた, cには次の文字が入っている
	str[str_ptr] = '\0';
	get_token_state = 0;

	char *endptr;
	long lv = strtol(str, &endptr, 10);
	if(*endptr == '\0') { // 最後まで読み切って整数だった
	  token.kind = 'N';
	  token.lval = lv;
	  pushback = c;
	  return &token;
	}

	double dv = strtod(str, &endptr);
	if(*endptr == '\0') { // 最後まで読み切って実数だった
	  token.kind = 'R';
	  token.dval = dv;
	  pushback = c;
	  return &token;
	}
	
        // 数字じゃなかったのでsymbol
	token.kind = 'S';
	strncpy(token.str, str, TOKEN_STR_SZ);
	token.str[TOKEN_STR_SZ] = '\0';
	pushback = c;
	return &token;
      }
      break; // 1
      
    case 2:
      if(c == '\n') get_token_state = 0;
      break;

    default:
      get_token_state = 0;
      pushback = 0;
      longjmp(top, e_tokenp); // can't happen
      break;
    } // switch
  } // while
  
  return (void*)0;
}
