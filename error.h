/**
 * Error type definitions.
 *
 * \file   error.h
 *
 * \author Justin J. Meza
 *
 * \date   2012
 */

#ifndef __ERROR_H__
#define __ERROR_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

/**
 * Represents an error type.  The error types are organized based on which
 * module they occur in:
 *
 *   - MN_* for the main body,
 *   - LX_* for the lexer,
 *   - TK_* for the tokenizer,
 *   - PR_* for the parser,
 *   - IN_* for the interpreter
 *
 * \note Remember to update the error message and error code arrays (in the
 * error C file) with the appropriate error message and code.
 */
typedef enum {
	MN_ERROR_OPENING_FILE,
	MN_ERROR_CLOSING_FILE,

	LX_LINE_CONTINUATION,
	LX_MULTIPLE_LINE_COMMENT,
	LX_EXPECTED_TOKEN_DELIMITER,

	TK_EXPECTED_FLOATING_POINT,
	TK_EXPECTED_INTEGER,
	TK_UNKNOWN_TOKEN,

	PR_UNKNOWN_IDENTIFIER_TYPE,
	PR_UNKNOWN_STATEMENT_TYPE,
	PR_UNKNOWN_EXPRESSION_TYPE,
	PR_EXPECTED_BOOLEAN,
	PR_EXPECTED_INTEGER,
	PR_EXPECTED_FLOAT,
	PR_EXPECTED_STRING,
	PR_EXPECTED_CONSTANT,
	PR_EXPECTED_TYPE,
	PR_EXPECTED_IDENTIFIER,
	PR_EXPECTED_TOKEN,
	PR_INVALID_OPERATOR,
	PR_EXPECTED_EXPRESSION,
	PR_EXPECTED_END_OF_EXPRESSION,
	PR_EXPECTED_END_OF_STATEMENT,
	PR_CANNOT_USE_STR_AS_LITERAL,
	PR_LITERAL_MUST_BE_UNIQUE,
	PR_EXPECTED_LOOP_NAME,
	PR_EXPECTED_EITHER_TOKEN,
	PR_EXPECTED_UNARY_FUNCTION,
	PR_EXPECTED_MATCHING_LOOP_NAME,
	PR_EXPECTED_STATEMENT,
} ErrorType;

void error2(ErrorType, ...);

#endif /* __ERROR_H__ */
