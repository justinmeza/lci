/**
 * Structures and functions for interpreting a parse tree.  The interpreter
 * traverses a parse tree in a depth-first manner, interpreting each node it
 * reaches along the way.  This is the last stage of the processing of a source
 * code file.
 *
 * \file   interpreter.h
 *
 * \author Justin J. Meza
 *
 * \date   2010-2012
 */

#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include "parser.h"
#include "unicode.h"

/**
 * Retrieves a value's integer data.
 */
#define getInteger(value) (value->data.i)

/**
 * Retrieves a value's decimal data.
 */
#define getFloat(value) (value->data.f)

/**
 * Retrieves a value's string data.
 */
#define getString(value) (value->data.s)

/**
 * Retrieves a value's function data.
 */
#define getFunction(value) (value->data.fn)

/**
 * Retrieves a value's array data.
 */
#define getArray(value) (value->data.a)

/**
 * Retrieves a value's blob data.
 */
#define getBlob(value) (value->data.b)

/**
 * Represents a value type.
 */
typedef enum {
	VT_INTEGER, /**< An integer value. */
	VT_FLOAT,   /**< A decimal value. */
	VT_BOOLEAN, /**< A boolean value. */
	VT_STRING,  /**< A string value. */
	VT_NIL,     /**< Represents no value. */
	VT_FUNC,    /**< A function. */
	VT_ARRAY,   /**< An array. */
	VT_BLOB     /**< A binary blob of data. */
} ValueType;

/**
 * Stores value data.
 */
typedef union {
	long long i;                 /**< Integer data. */
	float f;               /**< Decimal data. */
	char *s;               /**< String data. */
	FuncDefStmtNode *fn;   /**< Function data. */
	struct scopeobject *a; /**< Array data. */
	void *b;               /**< Binary blob data. */
} ValueData;

/**
 * Increments a value's semaphore.
 */
#define V(value) (value->semaphore++)

/**
 * Decrements a value's semaphore.
 */
#define P(value) (value->semaphore--)

/**
 * Stores a value.
 */
typedef struct {
	ValueType type;           /**< The type of value stored. */
	ValueData data;           /**< The value data. */
	unsigned short semaphore; /**< A semaphore for value usage. */
} ValueObject;

/**
 * Represents the return type.
 */
typedef enum {
	RT_DEFAULT, /**< Code block completed successfully. */
	RT_BREAK,   /**< Broke out of a loop or switch statement. */
	RT_RETURN   /**< Returned from a function. */
} ReturnType;

/**
 * Stores return state.
 */
typedef struct returnobject {
	ReturnType type;    /**< The type of return encountered. */
	ValueObject *value; /**< The optional return value. */
} ReturnObject;

/**
 * Stores a set of variables hierarchically.
 */
typedef struct scopeobject {
	struct scopeobject *parent; /**< The parent scope. */
	struct scopeobject *caller; /**< The caller scope (if in a function). */
	ValueObject *impvar;        /**< The \ref impvar "implicit variable". */
	unsigned int numvals;       /**< The number of values in the scope. */
	char **names;               /**< The names of the values. */
	ValueObject **values;       /**< The values in the scope. */
} ScopeObject;

/**
 * \name Utilities
 *
 * Functions for performing helper tasks.
 */
/**@{*/
void printInterpreterError(const char *, IdentifierNode *, ScopeObject *);
char *copyString(char *);
unsigned int isDecString(const char *);
unsigned int isHexString(const char *);
char *resolveIdentifierName(IdentifierNode *, ScopeObject *);
int resolveTerminalSlot(ScopeObject *, ScopeObject *, IdentifierNode *, ScopeObject **, IdentifierNode **);
/**@}*/

/**
 * \name Value object modifiers
 *
 * Functions for creating, copying, and deleting value objects.
 */
/**@{*/
ValueObject *createNilValueObject(void);
ValueObject *createBooleanValueObject(int);
ValueObject *createIntegerValueObject(long long);
ValueObject *createFloatValueObject(float);
ValueObject *createStringValueObject(char *);
ValueObject *createFunctionValueObject(FuncDefStmtNode *);
ValueObject *createArrayValueObject(ScopeObject *);
ValueObject *createBlobValueObject(void *);
ValueObject *copyValueObject(ValueObject *);
void deleteValueObject(ValueObject *);
/**@}*/

/**
 * \name Scope object modifiers
 *
 * Functions for manipulating scope objects and their data.
 */
/**@{*/
ScopeObject *createScopeObject(ScopeObject *);
ScopeObject *createScopeObjectCaller(ScopeObject *, ScopeObject *);
void deleteScopeObject(ScopeObject *);
ValueObject *createScopeValue(ScopeObject *, ScopeObject *, IdentifierNode *);
ValueObject *updateScopeValue(ScopeObject *, ScopeObject *, IdentifierNode *, ValueObject *);
ValueObject *getScopeValue(ScopeObject *, ScopeObject *, IdentifierNode *);
ValueObject *getScopeValueLocal(ScopeObject *, ScopeObject *, IdentifierNode *);
ScopeObject *getScopeObject(ScopeObject *, ScopeObject *, IdentifierNode *);
ScopeObject *getScopeObjectLocal(ScopeObject *, ScopeObject *, IdentifierNode *);
void deleteScopeValue(ScopeObject *, ScopeObject *, IdentifierNode *);
/**@}*/

/**
 * \name Return object modifiers
 *
 * Functions for creating and deleting return objects.
 */
/**@{*/
ReturnObject *createReturnObject(ReturnType, ValueObject *);
void deleteReturnObject(ReturnObject *);
/**@}*/

/**
 * \name Casts
 *
 * Functions for performing casts between different types of values.
 */
/**@{*/
ValueObject *castBooleanImplicit(ValueObject *, ScopeObject *);
ValueObject *castIntegerImplicit(ValueObject *, ScopeObject *);
ValueObject *castFloatImplicit(ValueObject *, ScopeObject *);
ValueObject *castStringImplicit(ValueObject *, ScopeObject *);
ValueObject *castBooleanExplicit(ValueObject *, ScopeObject *);
ValueObject *castIntegerExplicit(ValueObject *, ScopeObject *);
ValueObject *castFloatExplicit(ValueObject *, ScopeObject *);
ValueObject *castStringExplicit(ValueObject *, ScopeObject *);
/**@}*/

/**
 * \name Node interpreters
 *
 * Functions for interpreting basic parse tree nodes.
 */
/**@{*/
ValueObject *interpretExprNode(ExprNode *, ScopeObject *);
ReturnObject *interpretStmtNode(StmtNode *, ScopeObject *);
ReturnObject *interpretStmtNodeList(StmtNodeList *, ScopeObject *);
ReturnObject *interpretBlockNode(BlockNode *, ScopeObject *);
int interpretMainNodeScope(MainNode *, ScopeObject *);
/**@}*/

/**
 * \name Expression interpreters
 *
 * Functions for interpreting expression parse tree nodes.
 */
/**@{*/
ValueObject *interpretImpVarExprNode(ExprNode *, ScopeObject *);
ValueObject *interpretCastExprNode(ExprNode *, ScopeObject *);
ValueObject *interpretFuncCallExprNode(ExprNode *, ScopeObject *);
ValueObject *interpretIdentifierExprNode(ExprNode *, ScopeObject *);
ValueObject *interpretConstantExprNode(ExprNode *, ScopeObject *);
ValueObject *interpretSystemCommandExprNode(ExprNode *, ScopeObject *);
/**@}*/

/**
 * \name Operation interpreters
 *
 * Functions for interpreting operation parse tree nodes.
 */
/**@{*/
ValueObject *interpretNotOpExprNode(OpExprNode *, ScopeObject *);
ValueObject *interpretArithOpExprNode(OpExprNode *, ScopeObject *);
ValueObject *interpretBoolOpExprNode(OpExprNode *, ScopeObject *);
ValueObject *interpretEqualityOpExprNode(OpExprNode *, ScopeObject *);
ValueObject *interpretConcatOpExprNode(OpExprNode *, ScopeObject *);
ValueObject *interpretOpExprNode(ExprNode *, ScopeObject *);
/**@}*/

/**
 * \name Statement interpreters
 *
 * Functions for interpreting statement parse tree nodes.
 */
/**@{*/
ReturnObject *interpretCastStmtNode(StmtNode *, ScopeObject *);
ReturnObject *interpretPrintStmtNode(StmtNode *, ScopeObject *);
ReturnObject *interpretInputStmtNode(StmtNode *, ScopeObject *);
ReturnObject *interpretAssignmentStmtNode(StmtNode *, ScopeObject *);
ReturnObject *interpretDeclarationStmtNode(StmtNode *, ScopeObject *);
ReturnObject *interpretIfThenElseStmtNode(StmtNode *, ScopeObject *);
ReturnObject *interpretSwitchStmtNode(StmtNode *, ScopeObject *);
ReturnObject *interpretBreakStmtNode(StmtNode *, ScopeObject *);
ReturnObject *interpretReturnStmtNode(StmtNode *, ScopeObject *);
ReturnObject *interpretLoopStmtNode(StmtNode *, ScopeObject *);
ReturnObject *interpretDeallocationStmtNode(StmtNode *, ScopeObject *);
ReturnObject *interpretFuncDefStmtNode(StmtNode *, ScopeObject *);
ReturnObject *interpretExprStmtNode(StmtNode *, ScopeObject *);
ReturnObject *interpretAltArrayDefStmtNode(StmtNode *, ScopeObject *);
ReturnObject *interpretBindingStmtNode(StmtNode *, ScopeObject *);
/* Forward declaration of binding.h function (to break circular dependence) */
void loadLibrary(ScopeObject *, IdentifierNode *);
ReturnObject *interpretImportStmtNode(StmtNode *, ScopeObject *);
/**@}*/

/**
 * \name Arithmetic operations (integer-integer)
 *
 * Functions for performing integer-integer operations on values.
 */
/**@{*/
ValueObject *opAddIntegerInteger(ValueObject *, ValueObject *);
ValueObject *opSubIntegerInteger(ValueObject *, ValueObject *);
ValueObject *opMultIntegerInteger(ValueObject *, ValueObject *);
ValueObject *opDivIntegerInteger(ValueObject *, ValueObject *);
ValueObject *opMaxIntegerInteger(ValueObject *, ValueObject *);
ValueObject *opMinIntegerInteger(ValueObject *, ValueObject *);
ValueObject *opModIntegerInteger(ValueObject *, ValueObject *);
/**@}*/

/**
 * \name Arithmetic operations (integer-float)
 *
 * Functions for performing integer-float operations on values.
 */
/**@{*/
ValueObject *opAddIntegerFloat(ValueObject *, ValueObject *);
ValueObject *opSubIntegerFloat(ValueObject *, ValueObject *);
ValueObject *opMultIntegerFloat(ValueObject *, ValueObject *);
ValueObject *opDivIntegerFloat(ValueObject *, ValueObject *);
ValueObject *opMaxIntegerFloat(ValueObject *, ValueObject *);
ValueObject *opMinIntegerFloat(ValueObject *, ValueObject *);
ValueObject *opModIntegerFloat(ValueObject *, ValueObject *);
/**@}*/

/**
 * \name Arithmetic operations (float-integer)
 *
 * Functions for performing float-integer operations on values.
 */
/**@{*/
ValueObject *opAddFloatInteger(ValueObject *, ValueObject *);
ValueObject *opSubFloatInteger(ValueObject *, ValueObject *);
ValueObject *opMultFloatInteger(ValueObject *, ValueObject *);
ValueObject *opDivFloatInteger(ValueObject *, ValueObject *);
ValueObject *opMaxFloatInteger(ValueObject *, ValueObject *);
ValueObject *opMinFloatInteger(ValueObject *, ValueObject *);
ValueObject *opModFloatInteger(ValueObject *, ValueObject *);
/**@}*/

/**
 * \name Arithmetic operations (float-float)
 *
 * Functions for performing float-float operations on values.
 */
/**@{*/
ValueObject *opAddFloatFloat(ValueObject *, ValueObject *);
ValueObject *opSubFloatFloat(ValueObject *, ValueObject *);
ValueObject *opMultFloatFloat(ValueObject *, ValueObject *);
ValueObject *opDivFloatFloat(ValueObject *, ValueObject *);
ValueObject *opMaxFloatFloat(ValueObject *, ValueObject *);
ValueObject *opMinFloatFloat(ValueObject *, ValueObject *);
ValueObject *opModFloatFloat(ValueObject *, ValueObject *);
/**@}*/

/**
 * \name Equality operations (boolean-boolean)
 *
 * Functions for performing boolean-boolean operations on values.
 */
/**@{*/
ValueObject *opEqBooleanBoolean(ValueObject *, ValueObject *);
ValueObject *opNeqBooleanBoolean(ValueObject *, ValueObject *);
/**@}*/

/**
 * \name Equality operations (integer-integer)
 *
 * Functions for performing integer-integer operations on values.
 */
/**@{*/
ValueObject *opEqIntegerInteger(ValueObject *, ValueObject *);
ValueObject *opNeqIntegerInteger(ValueObject *, ValueObject *);
/**@}*/

/**
 * \name Equality operations (integer-float)
 *
 * Functions for performing integer-float operations on values.
 */
/**@{*/
ValueObject *opEqIntegerFloat(ValueObject *, ValueObject *);
ValueObject *opNeqIntegerFloat(ValueObject *, ValueObject *);
/**@}*/

/**
 * \name Equality operations (float-integer)
 *
 * Functions for performing float-integer operations on values.
 */
/**@{*/
ValueObject *opEqFloatInteger(ValueObject *, ValueObject *);
ValueObject *opNeqFloatInteger(ValueObject *, ValueObject *);
/**@}*/

/**
 * \name Equality operations (float-float)
 *
 * Functions for performing float-float operations on values.
 */
/**@{*/
ValueObject *opEqFloatFloat(ValueObject *, ValueObject *);
ValueObject *opNeqFloatFloat(ValueObject *, ValueObject *);
/**@}*/

/**
 * \name Equality operations (string-string)
 *
 * Functions for performing string-string operations on values.
 */
/**@{*/
ValueObject *opEqStringString(ValueObject *, ValueObject *);
ValueObject *opNeqStringString(ValueObject *, ValueObject *);
/**@}*/

/**
 * \name Equality operations (nil-nil)
 *
 * Functions for performing nil-nil operations on values.
 */
/**@{*/
ValueObject *opEqNilNil(ValueObject *, ValueObject *);
ValueObject *opNeqNilNil(ValueObject *, ValueObject *);
/**@}*/

#endif /* __INTERPRETER_H__ */
