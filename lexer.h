/** Structures and functions for separating a character buffer into lexemes. The
  * lexer reads through a buffer of characters (themselves typically read from
  * standard input), strips whitespace, and breaks them up into logical atoms of
  * character strings which, in turn, may be passed on to later processes (such
  * as a tokenizer).
  *
  * \file   lexer.h
  *
  * \author Justin J. Meza
  *
  * \date   2010 */

#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#undef DEBUG

/** Stores a lexeme.  A lexeme is the smallest unit of contiguous characters,
  * namely, it has been stripped of surrounding whitespace.
  *
  * \note This structure does not have any list structure to hold groups of it.
  *       Instead, pointers to arrays of these structures are employed to allow
  *       for easier tokenizing.
  *
  * \see createLexeme(char *, unsigned int) */
typedef struct {
	char *image;       /**< An array of characters that describe the lexeme. */
	const char *fname; /**< A pointer to the name of the file containing the lexeme. */
	unsigned int line; /**< The line number from the source file that the lexeme occurred on. */
} Lexeme;

/** Stores a list of lexemes.  This structure allows sets of lexemes to be
  * grouped together.
  *
  * \see createLexemeList(void)
  * \see addLexeme(LexemeList *, Lexeme *)
  * \see deleteLexemeList(LexemeList *) */
typedef struct {
	unsigned int num; /**< The number of Lexeme structures stored. */
	Lexeme **lexemes; /**< A pointer to the array of Lexeme structures. */
} LexemeList;

Lexeme *createLexeme(char *, const char *, unsigned int);
void deleteLexeme(Lexeme *);
LexemeList *createLexemeList(void);
Lexeme *addLexeme(LexemeList *, Lexeme*);
void deleteLexemeList(LexemeList *);
LexemeList *scanBuffer(const char *, unsigned int, const char *);

#endif /* __LEXER_H__ */
