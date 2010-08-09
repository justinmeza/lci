/** Structures and functions for interpreting a parse tree. The interpreter
  * traverses a parse tree in a depth-first manner, interpreting each node it
  * reaches along the way.  This is the last stage of the processing of a source
  * code file.
  *
  * \file   interpreter.h
  *
  * \author Justin J. Meza
  *
  * \date   2010 */

#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include "parser.h"
#include "unicode.h"

/** Gets the integer data associated with a ValueObject structure. */
#define getInteger(value) (value->data.i)
/** Gets the floating point data associated with a ValueObject structure. */
#define getFloat(value) (value->data.f)
/** Gets the string data associated with a ValueObject structure. */
#define getString(value) (value->data.s)

/** Denotes the type of a value. */
typedef enum {
	VT_INTEGER, /**< An integer value. */
	VT_FLOAT,   /**< A floating point decimal value. */
	VT_BOOLEAN, /**< A true/false value. */
	VT_STRING,  /**< A character string value. */
	VT_NIL      /**< Represents no value. */
} ValueType;

/** Stores the data associated with a ValueObject structure. */
typedef union {
	int i;   /**< Integer data. */
	float f; /**< Floating point data. */
	char *s; /**< Character string data. */
} ValueData;

/** Increments the semaphore of a ValueObject structure. */
#define V(value) (value->semaphore++)

/** Decrements the semaphore of a ValueObject structure. */
#define P(value) (value->semaphore--)

/** Stores a value.
  * 
  * \see copyValueObject(ValueObject *)
  * \see deleteValueObject(ValueObject *) */
typedef struct {
	ValueType type;           /**< The type of value stored. */
	ValueData data;           /**< The stored data. */
	unsigned short semaphore; /**< A semaphore for value usage. */
} ValueObject;

/** Denotes the type of return encountered. */
typedef enum {
	RT_DEFAULT, /**< A block of code returned after evaluating all of its statements. */
	RT_BREAK,   /**< A block of code within a LoopStmtNode or SwitchStmtNode returned via a break statement. */
	RT_RETURN   /**< A block of code within a FuncDefStmtNode called by a FuncCallExprNode returned (either with or without a value). */
} ReturnType;

/** Stores a return state.  Returns are encountered when
  *   - a block of code evaluates all of its statements,
  *   - a block of code within a LoopStmt or SwitchStmt encountered a break statement, or
  *   - a block of code within a FunctionDefStmt called by a FunctionCallExpr encounters a ReturnStmt. */
typedef struct {
	ReturnType type;    /**< The type of return encountered. */
	ValueObject *value; /**< The optional return value. */
} ReturnObject;

/** Stores the variables in a particular scope.  Scopes are arranged
  * heirarchically from global (the ancestor of all other scopes) to local (the
  * temporary scope of a BlockNode).
  *
  * \see createScopeObject(ScopeObject *)
  * \see deleteScopeObject(ScopeObject *) */
typedef struct scopeobject {
	struct scopeobject *parent; /**< A pointer to the parent ScopeObject. */
	ValueObject *impvar;        /**< A pointer to the ValueObject representing the implicit variable for this scope. */
	unsigned int numvals;       /**< The number of ValueObject structures in \a values. */
	IdentifierNode **names;     /**< A pointer to the IdentifierNode structures naming the values in the scope. */
	ValueObject **values;       /**< A pointer to an array of ValueObject structures in the scope. */
} ScopeObject;

char *createString(char *);
ValueObject *createNilValueObject(void);
ValueObject *createBooleanValueObject(int);
ValueObject *createIntegerValueObject(int);
ValueObject *createFloatValueObject(float);
ValueObject *createStringValueObject(char *);
ValueObject *copyValueObject(ValueObject *);
void deleteValueObject(ValueObject *);
ReturnObject *createReturnObject(ReturnType, ValueObject *);
void deleteReturnObject(ReturnObject *);
ScopeObject *createScopeObject(ScopeObject *);
void deleteScopeObject(ScopeObject *);
ValueObject *getScopeValue(ScopeObject *, IdentifierNode *);
ValueObject *getLocalScopeValue(ScopeObject *, IdentifierNode *);
ValueObject *createScopeValue(ScopeObject *, IdentifierNode *);
ValueObject *updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *);
unsigned int isNumString(const char *);
unsigned int isHexString(const char *);
ValueObject *castBooleanExplicit(ValueObject *, ScopeObject *);
ValueObject *castIntegerExplicit(ValueObject *, ScopeObject *);
ValueObject *castFloatExplicit(ValueObject *, ScopeObject *);
ValueObject *castStringExplicit(ValueObject *, ScopeObject *);
ValueObject *castBooleanImplicit(ValueObject *, ScopeObject *);
ValueObject *castIntegerImplicit(ValueObject *, ScopeObject *);
ValueObject *castFloatImplicit(ValueObject *, ScopeObject *);
ValueObject *castStringImplicit(ValueObject *, ScopeObject *);
ValueObject *interpretExprNode(ExprNode *, ScopeObject *);
ReturnObject *interpretStmtNode(StmtNode *, ScopeObject *);
ReturnObject *interpretStmtNodeList(StmtNodeList *, ScopeObject *);
ReturnObject *interpretBlockNode(BlockNode *, ScopeObject *);
int interpretMainNode(MainNode *);

ValueObject *interpretImpVarExprNode(ExprNode *, ScopeObject *);
ValueObject *interpretCastExprNode(ExprNode *, ScopeObject *);
ValueObject *interpretFuncCallExprNode(ExprNode *, ScopeObject *);
ValueObject *interpretIdentifierExprNode(ExprNode *, ScopeObject *);
ValueObject *interpretConstantExprNode(ExprNode *, ScopeObject *);

ValueObject *interpretNotOpExprNode(OpExprNode *, ScopeObject *);
ValueObject *interpretArithOpExprNode(OpExprNode *, ScopeObject *);
ValueObject *interpretBoolOpExprNode(OpExprNode *, ScopeObject *);
ValueObject *interpretEqualityOpExprNode(OpExprNode *, ScopeObject *);
ValueObject *interpretConcatOpExprNode(OpExprNode *, ScopeObject *);
ValueObject *interpretOpExprNode(ExprNode *, ScopeObject *);

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
ReturnObject *interpretFuncDefStmtNode(StmtNode *, ScopeObject *);
ReturnObject *interpretExprStmtNode(StmtNode *, ScopeObject *);

ValueObject *opAddIntegerInteger(ValueObject *, ValueObject *);
ValueObject *opSubIntegerInteger(ValueObject *, ValueObject *);
ValueObject *opMultIntegerInteger(ValueObject *, ValueObject *);
ValueObject *opDivIntegerInteger(ValueObject *, ValueObject *);
ValueObject *opMaxIntegerInteger(ValueObject *, ValueObject *);
ValueObject *opMinIntegerInteger(ValueObject *, ValueObject *);
ValueObject *opModIntegerInteger(ValueObject *, ValueObject *);

ValueObject *opAddIntegerFloat(ValueObject *, ValueObject *);
ValueObject *opSubIntegerFloat(ValueObject *, ValueObject *);
ValueObject *opMultIntegerFloat(ValueObject *, ValueObject *);
ValueObject *opDivIntegerFloat(ValueObject *, ValueObject *);
ValueObject *opMaxIntegerFloat(ValueObject *, ValueObject *);
ValueObject *opMinIntegerFloat(ValueObject *, ValueObject *);
ValueObject *opModIntegerFloat(ValueObject *, ValueObject *);

ValueObject *opAddFloatInteger(ValueObject *, ValueObject *);
ValueObject *opSubFloatInteger(ValueObject *, ValueObject *);
ValueObject *opMultFloatInteger(ValueObject *, ValueObject *);
ValueObject *opDivFloatInteger(ValueObject *, ValueObject *);
ValueObject *opMaxFloatInteger(ValueObject *, ValueObject *);
ValueObject *opMinFloatInteger(ValueObject *, ValueObject *);
ValueObject *opModFloatInteger(ValueObject *, ValueObject *);

ValueObject *opAddFloatFloat(ValueObject *, ValueObject *);
ValueObject *opSubFloatFloat(ValueObject *, ValueObject *);
ValueObject *opMultFloatFloat(ValueObject *, ValueObject *);
ValueObject *opDivFloatFloat(ValueObject *, ValueObject *);
ValueObject *opMaxFloatFloat(ValueObject *, ValueObject *);
ValueObject *opMinFloatFloat(ValueObject *, ValueObject *);
ValueObject *opModFloatFloat(ValueObject *, ValueObject *);

ValueObject *opEqBooleanBoolean(ValueObject *, ValueObject *);
ValueObject *opNeqBooleanBoolean(ValueObject *, ValueObject *);

ValueObject *opEqIntegerInteger(ValueObject *, ValueObject *);
ValueObject *opNeqIntegerInteger(ValueObject *, ValueObject *);

ValueObject *opEqIntegerFloat(ValueObject *, ValueObject *);
ValueObject *opNeqIntegerFloat(ValueObject *, ValueObject *);

ValueObject *opEqFloatInteger(ValueObject *, ValueObject *);
ValueObject *opNeqFloatInteger(ValueObject *, ValueObject *);

ValueObject *opEqFloatFloat(ValueObject *, ValueObject *);
ValueObject *opNeqFloatFloat(ValueObject *, ValueObject *);

ValueObject *opEqStringString(ValueObject *, ValueObject *);
ValueObject *opNeqStringString(ValueObject *, ValueObject *);

ValueObject *opEqNilNil(ValueObject *, ValueObject *);
ValueObject *opNeqNilNil(ValueObject *, ValueObject *);

#endif /* __INTERPRETER_H__ */
