/* A Bison parser, made by GNU Bison 1.875.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_MAINMENU = 258,
     T_MENU = 259,
     T_ENDMENU = 260,
     T_SOURCE = 261,
     T_CHOICE = 262,
     T_ENDCHOICE = 263,
     T_COMMENT = 264,
     T_CONFIG = 265,
     T_HELP = 266,
     T_HELPTEXT = 267,
     T_IF = 268,
     T_ENDIF = 269,
     T_DEPENDS = 270,
     T_REQUIRES = 271,
     T_OPTIONAL = 272,
     T_PROMPT = 273,
     T_DEFAULT = 274,
     T_TRISTATE = 275,
     T_BOOLEAN = 276,
     T_STRING = 277,
     T_INT = 278,
     T_HEX = 279,
     T_WORD = 280,
     T_WORD_QUOTE = 281,
     T_UNEQUAL = 282,
     T_EOF = 283,
     T_EOL = 284,
     T_CLOSE_PAREN = 285,
     T_OPEN_PAREN = 286,
     T_ON = 287,
     T_OR = 288,
     T_AND = 289,
     T_EQUAL = 290,
     T_NOT = 291
   };
#endif
#define T_MAINMENU 258
#define T_MENU 259
#define T_ENDMENU 260
#define T_SOURCE 261
#define T_CHOICE 262
#define T_ENDCHOICE 263
#define T_COMMENT 264
#define T_CONFIG 265
#define T_HELP 266
#define T_HELPTEXT 267
#define T_IF 268
#define T_ENDIF 269
#define T_DEPENDS 270
#define T_REQUIRES 271
#define T_OPTIONAL 272
#define T_PROMPT 273
#define T_DEFAULT 274
#define T_TRISTATE 275
#define T_BOOLEAN 276
#define T_STRING 277
#define T_INT 278
#define T_HEX 279
#define T_WORD 280
#define T_WORD_QUOTE 281
#define T_UNEQUAL 282
#define T_EOF 283
#define T_EOL 284
#define T_CLOSE_PAREN 285
#define T_OPEN_PAREN 286
#define T_ON 287
#define T_OR 288
#define T_AND 289
#define T_EQUAL 290
#define T_NOT 291




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 33 "zconf.y"
typedef union YYSTYPE {
	int token;
	char *string;
	struct symbol *symbol;
	struct expr *expr;
	struct menu *menu;
} YYSTYPE;
/* Line 1240 of yacc.c.  */
#line 116 "zconf.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE zconflval;



