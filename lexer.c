#include "lexer.h"

/**
 * Creates a lexeme.
 *
 * \param [in] image The string that identifies the lexeme.
 *
 * \param [in] fname The name of the file containing the lexeme.
 *
 * \param [in] line  The line number the lexeme occurred on.
 *
 * \return A new lexeme with the desired properties.
 *
 * \retval NULL Memory allocation failed.
 */
Lexeme *createLexeme(char *image, const char *fname, unsigned int line)
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
	/**
	 * \note \a fname is not copied because it only one copy is stored for
	 * all lexemes from the same file.  This is simply to avoid large
	 * numbers of lexemes storing duplicate file name strings.
	 */
	ret->fname = fname;
	ret->line = line;
#ifdef DEBUG
	fprintf(stderr, "Creating lexeme [%s]\n", image);
#endif
	return ret;
}

/**
 * Deletes a lexeme.
 *
 * \param [in,out] lexeme The lexeme to delete.
 */
void deleteLexeme(Lexeme *lexeme)
{
	if (!lexeme) return;
	free(lexeme->image);
	/**
	 * \note We do not free the file name because it is shared between many
	 * lexemes and is freed by whomever created the file name string.
	 */
	free(lexeme);
}

/**
 * Creates a list of lexemes.
 *
 * \return An empty lexeme list.
 *
 * \retval NULL Memory allocation failed.
 */
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

/**
 * Adds a lexeme to a list of lexemes.
 *
 * \param [in,out] list The list of lexemes to add \a lexeme to.
 *
 * \param [in] lexeme The lexeme to add to \a list.
 *
 * \post \a lexeme will be added to the end of \a list and the size of \a list
 * will be updated.
 *
 * \return A pointer to the added lexeme (will be the same as \a lexeme).
 *
 * \retval NULL Memory allocation failed.
 */
Lexeme *addLexeme(LexemeList *list, Lexeme *lexeme)
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

/**
 * Deletes a list of lexemes.
 *
 * \param [in,out] list The lexeme list to delete.
 *
 * \post The memory at \a list and all of its members will be freed.
 */
void deleteLexemeList(LexemeList *list)
{
	unsigned int n;
	if (!list) return;
	for (n = 0; n < list->num; n++)
		deleteLexeme(list->lexemes[n]);
	free(list->lexemes);
	free(list);
}

/**
 * Scans a buffer, removing unnecessary characters and grouping characters into
 * lexemes.  Lexemes are strings of characters separated by whitespace (although
 * newline characters are considered separate lexemes).  String literals are
 * handled a bit differently:  Starting at the first quotation character,
 * characters are collected until either a non-escaped quotation character is
 * read (i.e., a quotation character not preceded by a colon which itself is not
 * preceded by a colon) or a newline or carriage return character is read,
 * whichever comes first.  This handles the odd (but possible) case of strings
 * such as "::" which print out a single colon.  Also handled are the effects of
 * commas, ellipses, bangs (!), and array accesses ('Z).
 *
 * \param [in] buffer The characters to turn into lexemes.
 *
 * \param [in] size The number of characters in \a buffer.
 *
 * \param [in] fname The name of the file \a buffer was read from.
 *
 * \return A list of lexemes created from the contents of \a buffer.
 */
LexemeList *scanBuffer(const char *buffer, unsigned int size, const char *fname)
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
		/* Apostrophe Z ('Z) is its own lexeme */
		if (!strncmp(start, "'Z", 2)) {
			Lexeme *lex = createLexeme("'Z", fname, line);
			if (!lex) {
				deleteLexemeList(list);
				return NULL;
			}
			if (!addLexeme(list, lex)) {
				deleteLexeme(lex);
				deleteLexemeList(list);
				return NULL;
			}
			start += 2;
			continue;
		}
		/* Question mark (?) is its own lexeme */
		if (*start == '?') {
			Lexeme *lex = createLexeme("?", fname, line);
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
					error(LX_LINE_CONTINUATION, fname, line);
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
			error(LX_MULTIPLE_LINE_COMMENT, fname, line);
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
					&& *(start + len) != '?'
					&& strncmp(start + len, "'Z", 2)
					&& strncmp(start + len, "...", 3)
					&& strncmp(start + len, "\xE2\x80\xA6", 3)) {
				error(LX_EXPECTED_TOKEN_DELIMITER, fname, line);
				deleteLexemeList(list);
				return NULL;
			}
		}
		else {
			/* Scan for the end of the token */
			while (start[len] && !isspace(start[len])
					&& *(start + len) != ','
					&& *(start + len) != '!'
					&& *(start + len) != '?'
					&& strncmp(start + len, "'Z", 2)
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
