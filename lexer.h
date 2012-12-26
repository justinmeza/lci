/**
 * Structures and functions for separating a character buffer into lexemes --
 * groups of characters. The lexer reads through a buffer of characters
 * (themselves typically read from standard input), strips whitespace, and
 * breaks them up into logical atoms of character strings which, in turn, may be
 * passed on to later processes (such as a tokenizer).
 *
 * \file   lexer.h
 *
 * \author Justin J. Meza
 *
 * \date   2010-2012
 */

#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "error.h"

#undef DEBUG

/**
 * Stores a lexeme.  A lexeme is a group of contiguous characters, stripped of
 * surrounding whitespace or other lexemes.
 */
typedef struct {
	char *image;       /**< The string that identifies the lexeme. */
	const char *fname; /**< The name of the file containing the lexeme. */
	unsigned int line; /**< The line number the lexeme occurred on. */
} Lexeme;

/**
 * Stores a list of lexemes.
 */
typedef struct {
	unsigned int num; /**< The number of lexemes stored. */
	Lexeme **lexemes; /**< The array of stored lexemes. */
} LexemeList;

/**
 * \name Lexeme modifiers
 *
 * Functions for performing helper tasks.
 */
/**@{*/
Lexeme *createLexeme(char *, const char *, unsigned int);
void deleteLexeme(Lexeme *);
LexemeList *createLexemeList(void);
Lexeme *addLexeme(LexemeList *, Lexeme*);
void deleteLexemeList(LexemeList *);
/**@}*/

/**
 * \name Buffer lexer
 *
 * Generates lexemes from a character buffer.
 */
/**@{*/
LexemeList *scanBuffer(const char *, unsigned int, const char *);
/**@}*/

#endif /* __LEXER_H__ */
