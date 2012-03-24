#include "error.h"

static const char *err_msgs[] = {
	/* MN_ERROR_OPENING_FILE */
	"Error opening file '%s'.\n",
	/* MN_ERROR_CLOSING_FILE */
	"Error closing file '%s'.\n",

	/* LX_LINE_CONTINUATION */
	"%s:%d: a line with continuation may not be followed by an empty line\n",
	/* LX_MULTIPLE_LINE_COMMENT */
	"%s:%d: multiple line comment may not appear on the same line as code\n",
	/* LX_EXPECTED_TOKEN_DELIMITER */
	"%s:%d: expected token delimiter after string literal\n",

	/* TK_EXPECTED_FLOATING_POINT */
	"%s:%u: expected floating point decimal value\n",
	/* TK_EXPECTED_INTEGER */
	"%s:%u: expected integer value\n",
	/* TK_UNKNOWN_TOKEN */
	"%s:%u: unknown token at: %s\n",

	/* PR_UNKNOWN_IDENTIFIER_TYPE */
	"%s:%u: unable to delete unknown identifier type\n",
	/* PR_UNKNOWN_STATEMENT_TYPE */
	"unable to delete unknown statement type\n",
	/* PR_UNKNOWN_EXPRESSION_TYPE */
	"unable to delete unknown expression type\n",
	/* PR_EXPECTED_BOOLEAN */
	"%s:%u: expected integer at: %s\n",
	/* PR_EXPECTED_INTEGER */
	"%s:%u: expected integer at: %s\n",
	/* PR_EXPECTED_FLOAT */
	"%s:%u: expected float at: %s\n",
	/* PR_EXPECTED_STRING */
	"%s:%u: expected string at: %s\n",
	/* PR_EXPECTED_CONSTANT */
	"%s:%u: expected constant value at: %s\n",
	/* PR_EXPECTED_TYPE */
	"%s:%u: expected type at: %s\n",
	/* PR_EXPECTED_IDENTIFIER */
	"%s:%u: expected identifier at: %s\n",
	/* PR_EXPECTED_TOKEN */
	"%s:%u: expected %s at: %s\n",
	/* PR_INVALID_OPERATOR */
	"%s:%u: invalid operator at: %s\n",
	/* PR_EXPECTED_EXPRESSION */
	"%s:%u: expected expression at: %s\n",
	/* PR_EXPECTED_END_OF_EXPRESSION */
	"%s:%u: expected end of expression at: %s\n",
	/* PR_EXPECTED_END_OF_STATEMENT */
	"%s:%u: expected end of statement at: %s\n",
	/* PR_CANNOT_USE_STR_AS_LITERAL */
	"%s:%u: cannot use an interpolated string as an OMG literal at: %s\n",
	/* PR_LITERAL_MUST_BE_UNIQUE */
	"%s:%u: OMG literal must be unique at: %s\n",
	/* PR_EXPECTED_LOOP_NAME */
	"%s:%u: expected loop name at: %s\n",
	/* PR_EXPECTED_EITHER_TOKEN */
	"%s:%u: expected %s or %s at: %s\n",
	/* PR_EXPECTED_UNARY_FUNCTION */
	"%s:%u: expected unary function at: %s\n",
	/* PR_EXPECTED_MATCHING_LOOP_NAME */
	"%s:%u: expected matching loop name at: %s\n",
	/* PR_EXPECTED_STATEMENT */
	"%s:%u: expected statement at: %s\n",
};

static const int err_codes[] = {
	/* The 100 block is for the main body */
	100, /* MN_ERROR_OPENING_FILE */
	101, /* MN_ERROR_CLOSING_FILE */

	/* The 200 block is for the lexer */
	200, /* LX_LINE_CONTINUATION */
	201, /* LX_MULTIPLE_LINE_COMMENT */
	202, /* LX_EXPECTED_TOKEN_DELIMITER */

	/* The 300 block is for the tokenizer */
	300, /* TK_EXPECTED_FLOATING_POINT */
	301, /* TK_EXPECTED_INTEGER */
	302, /* TK_UNKNOWN_TOKEN */

	/* The 400 block is for the parser */
	400, /* PR_UNKNOWN_IDENTIFIER_TYPE */
	401, /* PR_UNKNOWN_STATEMENT_TYPE */
	402, /* PR_UNKNOWN_EXPRESSION_TYPE */
	403, /* PR_EXPECTED_BOOLEAN */
	404, /* PR_EXPECTED_INTEGER */
	405, /* PR_EXPECTED_FLOAT */
	406, /* PR_EXPECTED_STRING */
	407, /* PR_EXPECTED_CONSTANT */
	408, /* PR_EXPECTED_TYPE */
	409, /* PR_EXPECTED_IDENTIFIER */
	410, /* PR_EXPECTED_TOKEN */
	411, /* PR_INVALID_OPERATOR */
	412, /* PR_EXPECTED_EXPRESSION */
	413, /* PR_EXPECTED_END_OF_EXPRESSION */
	414, /* PR_EXPECTED_END_OF_STATEMENT */
	415, /* PR_CANNOT_USE_STR_AS_LITERAL */
	416, /* PR_LITERAL_MUST_BE_UNIQUE */
	417, /* PR_EXPECTED_LOOP_NAME */
	418, /* PR_EXPECTED_ONE_OF */
	419, /* PR_EXPECTED_UNARY_FUNCTION */
	420, /* PR_EXPECTED_MATCHING_LOOP_NAME */
	421, /* PR_EXPECTED_STATEMENT */

	/* The 500 block is for the interpreter */
};

void error2(ErrorType e, ...)
{
	va_list args;
	va_start(args, e);
	vfprintf(stderr, err_msgs[e], args);
	va_end(args);

	exit(err_codes[e]);
}
