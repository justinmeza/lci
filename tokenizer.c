#include "tokenizer.h"

/**
 * Checks if a string follows the format for an integer.  Specifically, it
 * checks if the string matches the regular expression: (-?[1-9][0-9]*|0).
 *
 * \param [in] image The string to check.
 *
 * \retval 0 \a image does not match the pattern for an integer.
 *
 * \retval 1 \a image matches the pattern for an integer.
 */
int isInteger(const char *image)
{
	const char *cur = image;
	if (*cur == '-'
			|| (isdigit(*cur) && *cur != '0')
			|| (*cur == '0' && *(cur + 1) == '\0')) {
		cur++;
		while (isdigit(*cur)) cur++;
		if (*cur == '\0') return 1;
	}
	return 0;
}

/**
 * Checks if a string follows the format for a decimal.  Specifically, it checks
 * if the string matches the regular expression: (-?[0-9].[0-9]*).
 *
 * \param [in] image The string to check.
 *
 * \retval 0 \a image does not match the pattern for a decimal.
 *
 * \retval 1 \a image matches the pattern for a decimal.
 */
int isFloat(const char *image)
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

/**
 * Checks if a string follows the format for a string literal.  Specifically, it
 * checks if the string matches the regular expression: (".*").
 *
 * \param [in] image The string to check.
 *
 * \retval 0 \a image does not match the pattern for a string.
 *
 * \retval 1 \a image matches the pattern for a string.
 */
int isString(const char *image)
{
	size_t len = strlen(image);
	return (len >= 2 && image[0] == '"' && image[len - 1] == '"');
}

/**
 * Checks if a string follows the format for an identifier.  Specifically, it
 * checks if the string matches the regular expression: ([a-zA-Z][a-zA-Z0-9_]*).
 *
 * \param image [in] The string to check.
 *
 * \retval 0 \a image does not match the pattern for an identifier.
 *
 * \retval 1 \a image matches the pattern for an identifier.
 */
int isIdentifier(const char *image)
{
	const char *cur = image;
	/* First character must be alphabetic */
	if (!cur || !isalpha(*cur)) return 0;
	cur++;
	while (*cur) {
		if (isalnum(*cur) || *cur == '_') cur++;
		else return 0;
	}
	return 1;
}

/**
 * Creates a token.
 *
 * \param [in] type The type of token to create.
 *
 * \param [in] image The string that represents the token.
 *
 * \param [in] fname The name of the file containing the token.
 *
 * \param [in] line The number of the line containing the token.
 *
 * \return A pointer to a new token with the desired properties.
 *
 * \retval NULL Memory allocation failed.
 */
Token *createToken(TokenType type,
                   const char *image,
                   const char *fname,
                   unsigned int line)
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
	/**
	 * \note fname is not copied because only one copy is stored for all
	 * Token structures that share it.
	 */
	ret->fname = fname;
	ret->line = line;
	return ret;
}

/**
 * Deletes a token.
 *
 * \param [in,out] token The token to delete.
 *
 * \post The memory at \a token and all of its members will be freed.
 */
void deleteToken(Token *token)
{
	if (!token) return;
	free(token->image);
	free(token);
}

/**
 * Adds a token to a list.
 *
 * \param [in,out] list The list of tokens to add \a token to.
 *
 * \param [in,out] num The number of tokens in \a list.
 *
 * \param [in] token The token to add to \a list.
 *
 * \post \a token will be added to the end of \a list and the size of \a list
 * will be updated.
 *
 * \retval 0 Memory allocation failed.
 *
 * \retval 1 \a token was added to \a list.
 */
int addToken(Token ***list,
             unsigned int *num,
             Token *token)
{
	unsigned int newsize = *num + 1;
	void *mem = realloc(*list, sizeof(Token *) * newsize);
	if (!mem) {
		perror("realloc");
		return 0;
	}
	*list = mem;
	(*list)[*num] = token;
	*num = newsize;
#ifdef DEBUG
	fprintf(stderr, "Adding token type %d [%s]\n", token->type, token->image);
#endif
	return 1;
}

/**
 * Deletes a list of tokens.
 *
 * \param list [in,out] The list of tokens to delete.
 *
 * \post The memory at \a list and all of its members will be freed.
 */
void deleteTokens(Token **list)
{
	Token **tok = list;
	while (*tok) {
		deleteToken(*tok);
		tok++;
	}
	free(list);
}

/**
 * Matches lexemes against a string.  Traverses \a lexemes starting at \a start
 * and compares lexeme images to space-delimited substrings from \a match.
 *
 * \param lexemes [in] The list of lexemes to match from.
 *
 * \param start [in] The index within \a lexemes to start matching at.
 * 
 * \param match [in] A string of space-delimited substrings to match.
 *
 * \return The number of lexemes matched.
 */
unsigned int acceptLexemes(LexemeList *lexemes,
                           unsigned int start,
                           const char *match)
{
	unsigned int offset = 0;
	unsigned int n;
	unsigned int i;
	for (n = 0, i = 0;
			match[n] || lexemes->lexemes[start + offset]->image[i];
			n++) {
		if (match[n] == ' ') {
			offset++;
			i = 0;
			continue;
		}
		if (lexemes->lexemes[start + offset]->image[i] != match[n])
			return 0;
		i++;
	}
	return offset + 1;
}

/**
 * Checks if the next lexemes in a list comprise a keyword and, if so, generates
 * a new token representing that keyword.  Specifically, \a lexemes is searched,
 * starting at \a start for keywords.  If one is found, an appropriate token is
 * created and returned and \a start is incremented by the number of lexemes
 * matched minus one.
 *
 * \param lexemes [in] A list of lexemes to search for keywords in.
 *
 * \param start [in,out] The position within \a lexemes to begin searching for
 * keywords.
 *
 * \post If a keyword is not found, \a start will not be modified.  Otherwise,
 * \a start will be incremented by the number of lexemes matched minus one.
 *
 * \return A pointer to the token containing the matched keyword.
 *
 * \retval NULL No keywords were found or there was an error allocating memory.
 */
Token *isKeyword(LexemeList *lexemes,
                 unsigned int *start)
{
	Token *token = NULL;
	TokenType type;
	const char *fname = lexemes->lexemes[*start]->fname;
	unsigned int line = lexemes->lexemes[*start]->line;
	/* For each keyword, */
	for (type = 0; type != TT_ENDOFTOKENS; type++) {
		/* Check if the start of lexemes match */
		unsigned int num = acceptLexemes(lexemes,
				*start, keywords[type]);
		if (!num) continue;
		/* If so, create a new token for the keyword */
		token = createToken(type, keywords[type], fname, line);
		/* And advance the start */
		*start += (num - 1);
		break;
	}
	return token;
}

/**
 * Converts a list of lexemes into tokens.  Also parses integers, floats, and
 * strings into tokens with semantic meaning.
 *
 * \param list [in] A list of lexemes to tokenize.
 *
 * \return A list of tokens generated from \a list.
 *
 * \retval NULL An unrecognized token was encounteres or memory allocation
 * failed.
 */
Token **tokenizeLexemes(LexemeList *list)
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
			if (sscanf(lexeme->image, "%f", &(token->data.f)) != 1)
				error(TK_EXPECTED_FLOATING_POINT, fname, line);
		}
		/* Integer */
		else if (isInteger(image)) {
			token = createToken(TT_INTEGER, image, fname, line);
			if (sscanf(lexeme->image, "%lli", &(token->data.i)) != 1)
				error(TK_EXPECTED_INTEGER, fname, line);
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
		/* Newline */
		/* Note that the spec is unclear as to whether a command *must*
		 * follow a comma.  For now, we let commas end a line. */
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
		/* This must be placed after keyword parsing or else most
		 * keywords would be tokenized as identifiers. */
		else if (isIdentifier(image)) {
			token = createToken(TT_IDENTIFIER, image, fname, line);
		}
		/* EOF */
		else if (!strcmp(image, "$")) {
			token = createToken(TT_EOF, "end of file", fname, line);
		}
		else {
			error(TK_UNKNOWN_TOKEN, fname, line, image);
			/* Clean up */
			deleteToken(ret[retsize - 1]);
			ret[retsize - 1] = NULL;
			deleteTokens(ret);
			return NULL;
		}

		/* Add the token to the token array */
		if (!addToken(&ret, &retsize, token)) {
			/* Clean up */
			if (token) deleteToken(token);
			deleteToken(ret[retsize - 1]);
			ret[retsize - 1] = NULL;
			deleteTokens(ret);
			return NULL;
		}
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
