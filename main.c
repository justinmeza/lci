/**
 * \mainpage lci Documentation
 *
 * \section license License
 *
 * lci - a LOLCODE interpreter written in C.
 * Copyright (C) 2010-2012 Justin J. Meza
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * \section maintainer Maintainer
 *
 *   - The lead maintainer for this project is Justin J. Meza
 *   (justin.meza@gmail.com).
 *
 *   - For more information, check this project's webpage at
 *   http://lolcode.org .
 *
 * \section about About
 *
 * lci is a LOLCODE interpreter written in C and is designed to be correct,
 * portable, fast, and precisely documented.
 *
 *   - correct: Every effort has been made to test lci's conformance to the
 *   LOLCODE language specification. Unit tests come packaged with the lci
 *   source code.
 *
 *   - portable: lci follows the widely ported ANSI C specification allowing it
 *   to compile on a broad range of systems.
 *
 *   - fast: Much effort has gone into producing simple and efficient code
 *   whenever possible to the extent that the above points are not compromized.
 *
 *   - precisely documented: lci uses Doxygen to generate literate code
 *   documentation, browsable here.
 *
 * \section organization Organization
 *
 * lci employs several different modules which each perform a specific task
 * during interpretation of code:
 *
 *   - \b lexer (lexer.c, lexer.h)- The lexer takes an array of characters and
 *   splits it up into individual \e lexemes.  Lexemes are divided by whitespace
 *   and other rules of the language.
 *
 *   - \b tokenizer (tokenizer.c, tokenizer.h) - The tokenizer takes the output
 *   of the lexer and converts it into individual \e tokens.  Tokens are
 *   different from lexemes in that a single token may be made up of multiple
 *   lexemes.  Also, the contents of some tokens are evaluated (such as integers
 *   and floats) for later use.
 *
 *   - \b parser (parser.c, parser.h) - The parser takes the output of the
 *   tokenizer and analyzes it semantically to turn it into a parse tree.
 *
 *   - \b interpreter (interpreter.c, interpreter.h) - The interpreter takes the
 *   output of the parser and executes it.
 *
 * Each of these modules is contained within its own C header and source code
 * files of the same name.
 *
 * To handle the conversion of Unicode code points and normative names to bytes,
 * two additional files, unicode.c and unicode.h are used.
 * 
 * Finally, main.c ties all of these modules together and handles the initial
 * loading of input data for the lexer.
 */

/**
 * \page varscope Variable Scope
 *
 * The specification states that variables are local to the scope of the main
 * block or any function they are contained within--except for temporary loop
 * variables which are local to the loop they are instantiated within.  This
 * behavior, combined with the fact that variables must be declared before being
 * used, means that variables may not be shadowed in different control scopes
 * (such as loops and conditional statements) and, more importantly, programmers
 * must keep track of whether variables have been previously declared within
 * conditionally executed code (for example, under this scoping if a variable is
 * declared in a conditional block it cannot be safely used in later code).
 *
 * One advantage of a flat scoping scheme is that nearly everything can be
 * stored in a single structure, making lookups faster.  However, I believe that
 * this advantage is not worth the extra frustration transferred to the
 * programmer and so scoping in lci is done in a similar manner to other
 * programming languages, to wit, within
 *
 *   - the main block of code,
 *   - the body of functions,
 *   - the body of loop statements, and
 *   - the bodies of conditional statements.
 *
 * This should alleviate any confusion which may have been caused by using a
 * completely local free-for-all scope.  Also, there seems to be a general
 * consensus on the LOLCODE forums that this is the way to go and flat scoping
 * causes too many problems for the programmer.
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "lexer.h"
#include "tokenizer.h"
#include "parser.h"
#include "interpreter.h"
#include "error.h"

#define READSIZE 512

static char *program_name;

static char *shortopt = "hvi";
static struct option longopt[] = {
	{ "help", no_argument, NULL, (int)'h' },
	{ "version", no_argument, NULL, (int)'v' },
	{ "interactive", no_argument, NULL, (int)'i' },
	{ 0, 0, 0, 0 }
};

static void help(void) {
	fprintf(stderr, "\
Usage: %s [FILE] ... \n\
Interpret FILE(s) as LOLCODE. Let FILE be '-' for stdin.\n\
  -h, --help\t\toutput this help\n\
  -v, --version\t\tprogram version\n\
  -i, --interactive\tinteractive prompt\n", program_name);
}

static void version (char *revision) {
	fprintf(stderr, "%s %s\n", program_name, revision);
}

int pipeline(char *buffer, unsigned int length, const char *fname, ScopeObject *scope)
{
	LexemeList *lexemes = NULL;
	Token **tokens = NULL;
	MainNode *node = NULL;
	if (!(lexemes = scanBuffer(buffer, length, fname))) {
		free(buffer);
		return 1;
	}
	free(buffer);
	if (!(tokens = tokenizeLexemes(lexemes))) {
		deleteLexemeList(lexemes);
		return 1;
	}
	deleteLexemeList(lexemes);
	if (!(node = parseMainNode(tokens))) {
		deleteTokens(tokens);
		return 1;
	}
	deleteTokens(tokens);
	if (interpretMainNodeScope(node, NULL)) {
		deleteMainNode(node);
		return 1;
	}
	deleteMainNode(node);
	return 0;
}

int main(int argc, char **argv)
{
	unsigned int size = 0;
	unsigned int length = 0;
	char *buffer = NULL;
	char *fname = NULL;
	FILE *file = NULL;
	int ch;

	char *revision = "v0.11.2";
	program_name = argv[0];

	while ((ch = getopt_long(argc, argv, shortopt, longopt, NULL)) != -1) {
		switch (ch) {
			default:
				help();
				exit(EXIT_FAILURE);
			case 'h':
				help();
				exit(EXIT_SUCCESS);
			case 'v':
				version(revision);
				exit(EXIT_SUCCESS);
			case 'i':
				{
					char *line = NULL;
					/* Save state between lines using a ScopeObject structure */
					ScopeObject *scope = createScopeObject(NULL);
					if (!scope) return 1;
					while ((line = readline("lci> "))) {
						char *pre = "HAI 1.4\n";
						char *post = "\n\nKTHXBYE\n";
						char *code = NULL;
						size = strlen(line);
						buffer = realloc(buffer, sizeof(char) * (length + size + 1));
						strncpy(buffer + length, line, size);
						buffer[length + size] = '\n';
						length += size + 1;
						add_history(line);
						/* Intercept KTHXBYE to quit */
						if (!strcmp(line, "KTHXBYE")) {
							break;
						}
						/* Intercept HALP to display help message */
						else if (!strcmp(line, "HALP")) {
							version(revision);
							help();
							continue;
						}
						/* Create staged code file */
						code = malloc(sizeof(char) * (strlen(pre) + size + strlen(post) + 1));
						strcpy(code, pre);
						strncpy(code + strlen(pre), line, size);
						strcpy(code + strlen(pre) + size, post);
						code[strlen(pre) + size + strlen(post)] = '\0';
						pipeline(code, strlen(code), "interactive", scope);
					}
					free(buffer);
					deleteScopeObject(scope);
					exit(EXIT_SUCCESS);
				}
		}
	}

	for (; optind < argc; optind++) {
		size = length = 0;
		buffer = fname = NULL;
		file = NULL;

		if (!strncmp(argv[optind],"-\0",2)) {
			file = stdin;
			fname = "stdin";
		}
		else {
			file = fopen(argv[optind], "r");
			fname = argv[optind];
		}

		if (!file) {
			error(MN_ERROR_OPENING_FILE, argv[optind]);
			return 1;
		}

		while (!feof(file)) {
			size += READSIZE;
			buffer = realloc(buffer, sizeof(char) * size);
			length += fread((buffer + size) - READSIZE,
					1,
					READSIZE,
					file);
		}

		if (fclose(file) != 0) {
			error(MN_ERROR_CLOSING_FILE, argv[optind]);
			if (buffer) free(buffer);
			return 1;
		}
		if (!buffer) return 1;
		buffer[length] = '\0';

		/* Remove hash bang line if run as a standalone script */
		if (buffer[0] == '#' && buffer[1] == '!') {
			unsigned int n;
			for (n = 0; buffer[n] != '\n' && buffer[n] != '\r'; n++)
				buffer[n] = ' ';
		}

		/*
		 * Remove UTF-8 BOM if present and add it to the output stream
		 * (we assume here that if a BOM is present, the system will
		 * also expect the output to include a BOM).
		 */
		if (buffer[0] == (char)0xef
				|| buffer[1] == (char)0xbb
				|| buffer[2] == (char)0xbf) {
			buffer[0] = ' ';
			buffer[1] = ' ';
			buffer[2] = ' ';
			printf("%c%c%c", 0xef, 0xbb, 0xbf);
		}

		return pipeline(buffer, length, fname, NULL);

	}

	return 0;
}
