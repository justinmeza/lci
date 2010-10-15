#include "tokenizer.h"

/** Checks if a string of characters follows the format for an integer.
  * Specifically, it checks if the string of characters matches the regular
  * expression: [-]?[1-9][0-9]* | 0
  *
  * \retval 0 The string of characters is not an integer.
  * \retval 1 The string of characters is an integer.
  *
  * \see isFloat(const char *)
  * \see isString(const char *)
  * \see isIdentifier(const char *) */
int isInteger(const char *image) /**< [in] The string of characters to compare. */
{
	const char *cur = image;
	if (*cur == '-' || (isdigit(*cur) && *cur != '0') || (*cur == '0' && *(cur + 1) == '\0')) {
		cur++;
		while (isdigit(*cur)) cur++;
		if (*cur == '\0') return 1;
	}
	return 0;
}

/** Checks if a string of characters follows the format for a floating
  * point decimal.  Specifically, it checks if the string of characters matches
  * the regular expression: [-]?[0-9].[0-9]*
  *
  * \retval 0 The string of characters is not a floating point decimal.
  * \retval 1 The string of characters is a floating point decimal.
  *
  * \see isInteger(const char *)
  * \see isString(const char *)
  * \see isIdentifier(const char *) */
int isFloat(const char *image) /**< [in] The string of characters to compare. */
{
	const char *cur = image;
	if (*cur == '-' || isdigit(*cur)) {
		cur++;
		while (isdigit(*cur)) cur++;
		if (*cur == '.') {
			cur++;
			while (isdigit(*cur)) cur++;
			if (*cur == '\0') return 1;
		}
	}
	return 0;
}

/** Checks if a string of characters follows the format for a string.
  * Specifically, it checks if the string of characters begins and ends with a
  * quote character.
  *
  * \retval 0 The string of characters is not a string.
  * \retval 1 The string of characters is a string.
  *
  * \see isInteger(const char *)
  * \see isFloat(const char *)
  * \see isIdentifier(const char *) */
int isString(const char *image) /**< [in] The string of characters to compare. */
{
	size_t len = strlen(image);
	return (len >= 2 && image[0] == '"' && image[len - 1] == '"');
}

/** Checks if a string of characters follows the format for an identifier.
  * Specifically, it checks if the string of characters matches the regular
  * expression: [a-zA-Z][a-zA-Z0-9_]*
  *
  * \retval 0 The string of characters is not an identifier.
  * \retval 1 The string of characters is an identifier.
  *
  * \see isInteger(const char *)
  * \see isFloat(const char *)
  * \see isString(const char *) */
int isIdentifier(const char *image) /**< [in] The string of characters to compare. */
{
	const char *cur = image;
	/* First character must be alphabetic */
	if (!cur || !isalpha(*cur)) return 0;
	cur++;
	while (*cur) {
		if (isalnum(*cur) || *cur == '_') cur++;
		/* Proposed LOLCODE Version 1.3 identifiers
		 * Remember to update expression: [a-zA-Z][a-zA-Z0-9]*([!!|!?][a-zA-Z][a-zA-Z0-9]*)*
		else if (*cur == '!' && *(cur + 1) && *(cur + 1) == '!') cur += 2;
		else if (*cur == '!' && *(cur + 1) && *(cur + 1) == '?') cur += 2;
		*/
		else return 0;
	}
	return 1;
}

/** Creates a Token structure.
  *
  * \return A pointer to a Token structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteToken(Token *) */
Token *createToken(TokenType type,    /**< [in] The type of token to create. */
                   const char *image, /**< [in] The characters from the source file that represent the token. */
                   const char *fname, /**< [in] A pointer to the name of the file containing the token. */
                   unsigned int line) /**< [in] The line number from the source file that the token occurred on. */
{
	Token *ret = malloc(sizeof(Token));
	if (!ret) {
		perror("malloc");
		return NULL;
	}
	ret->type = type;
	ret->image = malloc(sizeof(char) * (strlen(image) + 1));
	if (!(ret->image)) {
		free(ret);
		perror("malloc");
		return NULL;
	}
	strcpy(ret->image, image);
	/** \note fname is not copied because it would only one copy is stored
	  *       for all Token structures that share it. */
	ret->fname = fname;
	ret->line = line;
	return ret;
}

/** Deletes a Token structure.
  *
  * \pre \a token points to a Token structure created by createToken(TokenType, const char *, const char *, unsigned int).
  *
  * \post The memory at \a token and all of its elements will be freed.
  *
  * \see createToken(TokenType, const char *, const char *, unsigned int) */
void deleteToken(Token *token)
{
	if (!token) return;
	free(token->image);
	free(token);
}

/** Adds a Token to an array of Token structures.
  *
  * \note \a list may be NULL in which case a new list is created.
  *
  * \pre \a num is the number of elements in \a list.
  *
  * \post \a token will be added on to the end of \a list and the value at \a num
  *       will be updated accordingly.
  *
  * \return A pointer to the added Token structure (will be the same as \a token).
  *
  * \retval NULL realloc was unable to allocate memory.
  *
  * \see deleteTokens(Token **) */
Token *addToken(Token ***list,     /**< [in,out] A pointer to a pointer to an array of Token structures to add the new Token onto. */
                unsigned int *num, /**< [in,out] A pointer to the number of elements in \a list. */
                Token *token)      /**< [in] A pointer to the Token structure to add to \a list. */
{
	unsigned int newsize = *num + 1;
	void *mem = realloc(*list, sizeof(Token *) * newsize);
	if (!mem) {
		perror("realloc");
		return NULL;
	}
	*list = mem;
	(*list)[*num] = token;
	*num = newsize;
#ifdef DEBUG
	fprintf(stderr, "Adding token type %d [%s]\n", token->type, token->image);
#endif
	return token;
}

/** Deletes an array of Token structures.
  *
  * \pre \a list was created by and contains items added by addToken(Token ***, unsigned int *, TokenType, const char *, unsigned int).
  *
  * \post The memory at \a list and all of its elements will be freed.
  *
  * \see addToken(Token ***, unsigned int *, TokenType, const char *, unsigned int) */
void deleteTokens(Token **list) /**< [in,out] A pointer to an array of Token structures to be deleted. */
{
	Token **tok = list;
	while (*tok) {
		deleteToken(*tok);
		tok++;
	}
	free(list);
}

/** Tries to match a sequence of lexemes.  Scans through \a lexemes starting at
  * \a start and tries to match space-delimited lexemes from \a match.
  *
  * \pre \a lexemes was created by scanBuffer(const char *, unsigned int, const char *).
  *
  * \return The number of lexemes matched. */
unsigned int acceptLexemes(LexemeList *lexemes, /**< [in] A pointer to a LexemeList structure to match lexemes from. */
                           unsigned int start,  /**< [in] The position within \a lexemes to start matching at. */
                           const char *match)   /**< [in] A pointer to a character array describing the sequence of lexemes to match. */
{
	unsigned int offset = 0;
	unsigned int n;
	unsigned int i;
	for (n = 0, i = 0; match[n] || lexemes->lexemes[start + offset]->image[i]; n++, i++) {
		if (match[n] == ' ') {
			offset++;
			i = -1;
			continue;
		}
		if (lexemes->lexemes[start + offset]->image[i] != match[n])
			return 0;
	}
	return offset + 1;
}

/** Checks if a sequence of lexemes is a keyword.  \a lexemes is searched
  * starting at \a start for keywords.  If one is found, the appropriate Token
  * structure is created and returned and the value of \a start is incremented
  * by the number of lexemes matched minus one.
  *
  * \pre \a lexemes was created by scanBuffer(const char *, unsigned int, const char *).
  *
  * \post If a keyword is not found, \a start will be unmodified.  Otherwise,
  *       \a start will be incremented by the number of lexemes matched minus
  *       one.
  *
  * \return A pointer to a newly created keyword Token structure.
  *
  * \retval NULL No keywords were matched or there was an error allocating
  *         memory. */
Token *isKeyword(LexemeList *lexemes, /**< [in] A pointer to a LexemeList structure to search for keywords in. */
                 unsigned int *start) /**< [in,out] A pointer to the position within \a lexemes to start checking at. */
{
	Token *token = NULL;
	TokenType type;
	const char *fname = lexemes->lexemes[*start]->fname;
	unsigned int line = lexemes->lexemes[*start]->line;
	for (type = 0; type != TT_ENDOFTOKENS; type++) {
		int num = acceptLexemes(lexemes, *start, keywords[type]);
		if (!num) continue;
		token = createToken(type, keywords[type], fname, line);
		*start += (num - 1);
		break;
	}
	return token;
}

/** Converts a list of lexemes into tokens.  Additionally parses the literal
  * values of integers, floating point decimals, and strings.
  *
  * \pre \a list was created by scanBuffer(const char *, unsigned int, const char *).
  *
  * \return A pointer to an array of Token structures representing the tokenized
  *         form of the input lexeme stream.
  *
  * \retval NULL An unrecognized token was encountered or memory allocation
  *         failed. */
Token **tokenizeLexemes(LexemeList *list) /**< [in] A pointer to a LexemeList structure to tokenize. */
{
	void *mem = NULL;
	Token **ret = NULL;
	unsigned int retsize = 0;
	unsigned int n;
	for (n = 0; n < list->num; n++) {
		Lexeme *lexeme = list->lexemes[n];
		const char *image = lexeme->image;
		const char *fname = lexeme->fname;
		unsigned int line = lexeme->line;
		Token *token = NULL;
		/* String */
		if (isString(image)) {
			token = createToken(TT_STRING, image, fname, line);
		}
		/* Float */
		else if (isFloat(image)) {
			token = createToken(TT_FLOAT, image, fname, line);
			sscanf(lexeme->image, "%f", &(token->data.f));
		}
		/* Integer */
		else if (isInteger(image)) {
			token = createToken(TT_INTEGER, image, fname, line);
			sscanf(lexeme->image, "%i", &(token->data.i));
		}
		/* FAIL */
		else if (!strcmp(image, "FAIL")) {
			token = createToken(TT_BOOLEAN, "FAIL", fname, line);
			token->data.i = 0;
		}
		/* WIN */
		else if (!strcmp(image, "WIN")) {
			token = createToken(TT_BOOLEAN, "WIN", fname, line);
			token->data.i = 1;
		}
		/* CAN HAS STDIO? */
		else if (n < list->num - 2
				&& !strcmp(lexeme->image, "CAN")
				&& !strcmp(list->lexemes[n + 1]->image, "HAS")
				&& !strcmp(list->lexemes[n + 2]->image, "STDIO?")) {
			n += 2;
			/* Just for fun; not actually in spec */
			continue;
		}
		/* Newline */
		/* Note that the spec is unclear as to whether a command *must* follow
		 * a comma.  For now, we let commas end a line. */
		else if (!strcmp(image, "\n")) {
			/* Note that we ignore any initial newlines */
			if (retsize < 1) {
#ifdef DEBUG
				fprintf(stderr, "Skipping initial newline.\n");
#endif
				continue;
			}
			else if (ret[retsize - 1]->type == TT_NEWLINE) {
#ifdef DEBUG
				fprintf(stderr, "Skipping duplicate newline.\n");
#endif
				continue;
			}
			else {
				token = createToken(TT_NEWLINE, "end of line", fname, line);
			}
		}
		/* Keyword */
		else if ((token = isKeyword(list, &n))) {
		}
		/* Identifier */
		/* This must be placed after keyword parsing because most
		 * keywords look like identifiers. */
		else if (isIdentifier(image)) {
			token = createToken(TT_IDENTIFIER, image, fname, line);
		}
		/* EOF */
		else if (!strcmp(image, "$")) {
			token = createToken(TT_EOF, "end of file", fname, line);
		}
		else {
			fprintf(stderr, "%s:%d: unknown token at: %s\n", fname, line, image);
			/* Clean up */
			deleteToken(ret[retsize - 1]);
			ret[retsize - 1] = NULL;
			deleteTokens(ret);
			return NULL;
		}
		addToken(&ret, &retsize, token);
	}
	mem = realloc(ret, sizeof(Token *) * ++retsize);
	if (!mem) {
		deleteToken(ret[retsize - 2]);
		ret[retsize - 2] = NULL;
		deleteTokens(ret);
		return NULL;
	}
	ret = mem;
	ret[retsize - 1] = NULL;
	return ret;
}
