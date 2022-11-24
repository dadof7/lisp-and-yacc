/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     CREATE = 258,
     TABLE = 259,
     DEFAULT = 260,
     NOT = 261,
     DBNULL = 262,
     AUTOINCREMENT = 263,
     REFERENCES = 264,
     PRIMARY = 265,
     FOREIGN = 266,
     KEY = 267,
     CHAR = 268,
     VARCHAR = 269,
     DECIMAL = 270,
     NUMERIC = 271,
     INT = 272,
     SMALLINT = 273,
     MONEY = 274,
     DROP = 275,
     UNIQUE = 276,
     INDEX = 277,
     ON = 278,
     INSERT = 279,
     INTO = 280,
     VALUES = 281,
     ALTER = 282,
     ADD = 283,
     NUMINT = 284,
     NUMFLOAT = 285,
     ID = 286,
     STRING = 287,
     DATE = 288,
     DATETIME = 289
   };
#endif
/* Tokens.  */
#define CREATE 258
#define TABLE 259
#define DEFAULT 260
#define NOT 261
#define DBNULL 262
#define AUTOINCREMENT 263
#define REFERENCES 264
#define PRIMARY 265
#define FOREIGN 266
#define KEY 267
#define CHAR 268
#define VARCHAR 269
#define DECIMAL 270
#define NUMERIC 271
#define INT 272
#define SMALLINT 273
#define MONEY 274
#define DROP 275
#define UNIQUE 276
#define INDEX 277
#define ON 278
#define INSERT 279
#define INTO 280
#define VALUES 281
#define ALTER 282
#define ADD 283
#define NUMINT 284
#define NUMFLOAT 285
#define ID 286
#define STRING 287
#define DATE 288
#define DATETIME 289




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

