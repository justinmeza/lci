#include "lexer.h"

/** Creates a Lexeme structure.
  *
  * \return A pointer to a Lexeme structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteLexeme(Lexeme *) */
Lexeme *createLexeme(char *image,       /**< [in] An array of characters that describe the lexeme. */
                     const char *fname, /**< [in] A pointer to the name of the file containing the lexeme. */
                     unsigned int line) /**< [in] The line number from the source file that the lexeme occurred on. */
{
	Lexeme *ret = malloc(sizeof(Lexeme));
	if (!ret) {
		perror("malloc");
		return NULL;
	}
	ret->image = malloc(sizeof(char) * (strlen(image) + 1));
	if (!(ret->image)) {
		free(ret);
		perror("malloc");
		return NULL;
	}
	strcpy(ret->image, image);
	/** \note fname is not copied because it would only one copy is stored
	  *       for all Lexeme structures that share it. */
	ret->fname = fname;
	ret->line = line;
#ifdef DEBUG
	fprintf(stderr, "Creating lexeme [%s]\n", image);
#endif
	return ret;
}

/** Deletes a Lexeme structure.
  *
  * \pre \a lexeme points to a Lexeme structure created by createLexeme(char *, const char *, unsigned int).
  *
  * \post The memory at \a lexeme and all of its elements will be freed.
  *
  * \see createLexeme(char *, const char *, unsigned int) */
void deleteLexeme(Lexeme *lexeme)
{
	if (!lexeme) return;
	free(lexeme->image);
	/** \note We do not free (*lex)->fname because it is shared between many
	  *       Lexeme structures and is free'd by whoever created them. */
	free(lexeme);
}

/** Creates a LexemeList structure.
  *
  * \return A pointer to a LexemeList structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteLexemeList(LexemeList *) */
LexemeList *createLexemeList(void)
{
	LexemeList *p = malloc(sizeof(LexemeList));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->num = 0;
	p->lexemes = NULL;
	return p;
}

/** Adds a Lexeme structure to a LexemeList structure.
  *
  * \pre \a list was created by createLexemeList(void).
  * \pre \a lexeme was created by createLexeme(char *, const char *, unsigned int).
  *
  * \post \a lexeme will be added on to the end of \a list and the size of
  *       \a list will be updated accordingly.
  *
  * \return A pointer to the added Lexeme structure (will be the same as
  *         \a lexeme).
  *
  * \retval NULL realloc was unable to allocate memory. */
Lexeme *addLexeme(LexemeList *list, /**< [in,out] A pointer to the LexemeList structure to add \a lex to. */
                  Lexeme *lexeme)   /**< [in] A pointer to the Lexeme structure to add to \a list. */
{
	unsigned int newsize;
	void *mem = NULL;
	if (!list) return NULL;
	newsize = list->num + 1;
	mem = realloc(list->lexemes, sizeof(Lexeme *) * newsize);
	if (!mem) {
		perror("realloc");
		return NULL;
	}
	list->lexemes = mem;
	list->lexemes[list->num] = lexeme;
	list->num = newsize;
	return lexeme;
}

/** Deletes a LexemeList structure.
  *
  * \pre \a list was created by createLexemeList(void) and contains
  *      items added by addLexeme(LexemeList *, Lexeme *).
  *
  * \post The memory at \a list and any of its associated members will be
  *       freed.
  *
  * \see createLexemeList(void) */
void deleteLexemeList(LexemeList *list) /**< [in,out] A pointer to the LexemeList structure to delete. */
{
	unsigned int n;
	if (!list) return;
	for (n = 0; n < list->num; n++)
		deleteLexeme(list->lexemes[n]);
	free(list->lexemes);
	free(list);
}

/** Scans through a character buffer, removing unecessary characters and
  * generating lexemes.  Lexemes are separated by whitespace (but newline
  * characters are kept as their own lexeme).  String literals are handled a
  * bit differently: starting at the first quotation character, characters are
  * collected until either an unescaped quotation character is read (that is, a
  * quotation character not preceeded by a colon which itself is not proceeded
  * by a colon) or a newline or carriage return character is read, whichever
  * comes first.  This handles the odd case of strings such as "::" which print
  * out a single colon.  Also handled are the effects of commas, ellipses, and
  * bangs (!).
  *
  * \pre \a size is the number of characters starting at the memory location
  *      pointed to by \a buffer.
  *
  * \return A pointer to a LexemeList structure. */
LexemeList *scanBuffer(const char *buffer, /**< [in] An array of characters to tokenize. */
                       unsigned int size,  /**< [in] The number of characters in \a buffer. */
                       const char *fname)  /**< [in] An array of characters representing the name of the file used to read \a buffer. */
{
	const char *start = buffer;
	LexemeList *list = NULL;
	unsigned int line = 1;
	list = createLexemeList();
	if (!list) return NULL;
	while (start < buffer + size) {
		char *temp = NULL;
		unsigned int len = 1;
		/* Comma (,) is a soft newline */
		if (*start == ',') {
			Lexeme *lex = createLexeme("\n", fname, line);
			if (!lex) {
				deleteLexemeList(list);
				return NULL;
			}
			if (!addLexeme(list, lex)) {
				deleteLexeme(lex);
				deleteLexemeList(list);
				return NULL;
			}
			start++;
			continue;
		}
		/* Bang (!) is its own lexeme */
		if (*start == '!') {
			Lexeme *lex = createLexeme("!", fname, line);
			if (!lex) {
				deleteLexemeList(list);
				return NULL;
			}
			if (!addLexeme(list, lex)) {
				deleteLexeme(lex);
				deleteLexemeList(list);
				return NULL;
			}
			start++;
			continue;
		}
		/* Skip over leading whitespace */
		while (isspace(*start)) {
			unsigned int newline = 0;
			/* Newline is its own lexeme */
			if (!strncmp(start, "\r\n", 2)) {
				newline = 1;
				start++;
			}
			else if (*start == '\r' || *start == '\n') {
				newline = 1;
			}
			if (newline) {
				Lexeme *lex = createLexeme("\n", fname, line);
				if (!lex) {
					deleteLexemeList(list);
					return NULL;
				}
				if (!addLexeme(list, lex)) {
					deleteLexeme(lex);
					deleteLexemeList(list);
					return NULL;
				}
				line++;
			}
			start++;
			continue;
		}
		/* Skip over ellipses (...) and newline */
		if ((!strncmp(start, "\xE2\x80\xA6\r\n", 5) && (start += 5))
				|| (!strncmp(start, "\xE2\x80\xA6\r", 4) && (start += 4))
				|| (!strncmp(start, "\xE2\x80\xA6\n", 4) && (start += 4))
				|| (!strncmp(start, "...\r\n", 5) && (start += 5))
				|| (!strncmp(start, "...\r", 4) && (start += 4))
				|| (!strncmp(start, "...\n", 4) && (start += 4))) {
			const char *test = start;
			/* Make sure next line is not empty */
			while (*test && isspace(*test)) {
				if (*test == '\r' || *test == '\n') {
					fprintf(stderr, "%s:%d: a line with continuation may not be followed by an empty line\n", fname, line);
					deleteLexemeList(list);
					return NULL;
				}
				test++;
			}
			continue;
		}
		/* Skip over comments */
		if ((list->num == 0
				|| *(list->lexemes[list->num - 1]->image) == '\n')
				&& !strncmp(start, "OBTW", 4)) {
			start += 4;
			while (strncmp(start, "TLDR", 4)) {
				if ((!strncmp(start, "\r\n", 2) && (start += 2))
						|| (*start == '\r' && start++)
						|| (*start == '\n' && start++))
					line++;
				else
					start++;
			}
			start += 4;
			/* Must end in newline */
			while (*start && isspace(*start) && *start != '\r' && *start != '\n')
				start++;
			if (start == buffer || *start == ',' || *start == '\r' || *start == '\n')
				continue;
			fprintf(stderr, "%s:%d: multiple line comment may not appear on the same line as code\n", fname, line);
			deleteLexemeList(list);
			return NULL;
		}
		if (!strncmp(start, "BTW", 3)) {
			start += 3;
			while (*start && *start != '\r' && *start != '\n')
				start++;
			continue;
		}
		/* We have removed or processed any leading characters at this
		 * point */
		if (!*start) break;
		if (*start == '"') {
			/* Find the end of the string, watching for escape
			 * sequences */
			while ((start[len]
					&& *(start + len) != '\r'
					&& *(start + len) != '\n'
					&& *(start + len) != '"')
					|| (*(start + len) == '"'
					&& *(start + len - 1) == ':'
					&& *(start + len - 2) != ':'))
				len++;
			if (*(start + len) == '"') len++;
			/* Make sure this is the end of the token */
			if (start[len] && !isspace(start[len])
					&& *(start + len) != ','
					&& *(start + len) != '!'
					&& strncmp(start + len, "...", 3)
					&& strncmp(start + len, "\xE2\x80\xA6", 3)) {
				fprintf(stderr, "%s:%d: expected token delimiter after string literal\n", fname, line);
				deleteLexemeList(list);
				return NULL;
			}
		}
		else {
			/* Scan for the end of the token */
			while (start[len] && !isspace(start[len])
					&& *(start + len) != ','
					&& *(start + len) != '!'
					&& strncmp(start + len, "...", 3)
					&& strncmp(start + len, "\xE2\x80\xA6", 3))
				len++;
		}
		temp = malloc(sizeof(char) * (len + 1));
		if (!temp) {
			perror("malloc");
			deleteLexemeList(list);
			return NULL;
		}
		strncpy(temp, start, len);
		temp[len] = '\0';
		Lexeme *lex = createLexeme(temp, fname, line);
		if (!lex) {
			free(temp);
			deleteLexemeList(list);
			return NULL;
		}
		if (!addLexeme(list, lex)) {
			free(temp);
			deleteLexeme(lex);
			deleteLexemeList(list);
			return NULL;
		}
		free(temp);
		start += len;
	}
	/* Create an end-of-file lexeme */
	Lexeme *lex = createLexeme("$", fname, line);
	if (!lex) {
		deleteLexemeList(list);
		return NULL;
	}
	if (!addLexeme(list, lex)) {
		deleteLexeme(lex);
		deleteLexemeList(list);
		return NULL;
	}
	return list;
}
