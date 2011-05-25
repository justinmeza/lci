#include "parser.h"

#ifdef DEBUG
static unsigned int shiftwidth = 0;
void shiftout(void) { shiftwidth += 4; }
void shiftin(void) { shiftwidth -= 4; }
void debug(const char *info)
{
	int n;
	for (n = 0; n < shiftwidth; n++)
		fprintf(stderr, " ");
	fprintf(stderr, "%s\n", info);
}
#endif

/** Creates a MainNode structure.
  *
  * \pre \a block was created by createBlockNode(StmtNodeList *).
  * \pre \a functab was created by createFunctionTable(void) and contains
  *      items added by addFuncDefStmtNode(FunctionTable *, FuncDefStmtNode *).
  *
  * \return A pointer to a MainNode structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteMainNode(MainNode *) */
MainNode *createMainNode(BlockNode *block,       /**< [in] A pointer to the block of code to execute first. */
                         FunctionTable *functab) /**< [in] A pointer to the function table associated with this block of code. */
{
	MainNode *p = malloc(sizeof(MainNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->block = block;
	p->functab = functab;
	return p;
}

/** Deletes a MainNode structure.
  *
  * \pre \a node was created by createMainNode(BlockNode *, FunctionTable *).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createMainNode(BlockNode *, FunctionTable *) */
void deleteMainNode(MainNode *node) /**< [in,out] A pointer to the MainNode structure to be deleted. */
{
	if (!node) return;
	deleteBlockNode(node->block);
	deleteFunctionTable(node->functab);
	free(node);
}

/** Creates a BlockNode structure.
  *
  * \pre \a stmts was created by createStmtNodeList(void) and contains contents
  *      added by addStmtNode(StmtNodeList *, StmtNode *).
  *
  * \return A pointer to a BlockNode structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteBlockNode(BlockNode *) */
BlockNode *createBlockNode(StmtNodeList *stmts) /**< [in] A pointer to the list of statements which comprise the block of code. */
{
	BlockNode *p = malloc(sizeof(BlockNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->stmts = stmts;
	return p;
}

/** Deletes a BlockNode structure.
  *
  * \pre \a node was created by createBlockNode(StmtNodeList *).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createBlockNode(StmtNodeList *) */
void deleteBlockNode(BlockNode *node) /**< [in,out] A pointer to the BlockNode structure to be deleted. */
{
	if (!node) return;
	deleteStmtNodeList(node->stmts);
	free(node);
}

/** Creates a BlockNodeList structure.
  *
  * \return A pointer to a BlockNodeList structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteBlockNodeList(BlockNodeList *) */
BlockNodeList *createBlockNodeList(void)
{
	BlockNodeList *p = malloc(sizeof(BlockNodeList));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->num = 0;
	p->blocks = NULL;
	return p;
}

/** Adds a BlockNode structure to a BlockNodeList structure.
  *
  * \pre \a list was created by createBlockNodeList(void).
  * \pre \a node was created by createBlockNode(StmtNodeList *).
  *
  * \post \a node will be added on to the end of \a list and the size of
  *       \a list will be updated accordingly.
  *
  * \return A pointer to the added BlockNode structure (will be the same as \a node).
  *
  * \retval NULL realloc was unable to allocate memory. */
BlockNode *addBlockNode(BlockNodeList *list,  /**< [in,out] A pointer to the BlockNodeList structure to add \a node to. */
                        BlockNode *node)     /**< [in] A pointer to the BlockNode structure to add to \a list. */
{
	unsigned int newsize = list->num + 1;
	void *mem = realloc(list->blocks, sizeof(BlockNode *) * newsize);
	if (!mem) {
		perror("realloc");
		return NULL;
	}
	list->blocks = mem;
	list->blocks[list->num] = node;
	list->num = newsize;
	return node;
}

/** Deletes a BlockNodeList structure.
  *
  * \pre \a list was created by createBlockNodeList(void) and contains items
  *      added by addBlockNode(BlockNodeList *, BlockNode *).
  *
  * \post The memory at \a list and any of its associated members will be
  *       freed.
  *
  * \see createBlockNodeList(void) */
void deleteBlockNodeList(BlockNodeList *list) /**< [in,out] A pointer to the BlockNodeList structure to delete. */
{
	unsigned int n;
	if (!list) return;
	for (n = 0; n < list->num; n++)
		deleteBlockNode(list->blocks[n]);
	free(list->blocks);
	free(list);
}

/** Creates a boolean type ConstantNode structure.
  *
  * \return A pointer to a boolean type ConstantNode structure with value
  *         \c 0 if \a data equals 0 and \c 1 otherwise.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteConstantNode(ConstantNode *) */
ConstantNode *createBooleanConstantNode(int data) /**< [in] The constant boolean data. */
{
	ConstantNode *p = malloc(sizeof(ConstantNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->type = CT_BOOLEAN;
	p->data.i = (data != 0);
	return p;
}

/** Creates an integer type ConstantNode structure.
  *
  * \return A pointer to an integer type ConstantNode storing the desired value.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteConstantNode(ConstantNode *) */
ConstantNode *createIntegerConstantNode(int data) /**< [in] The constant integer data. */
{
	ConstantNode *p = malloc(sizeof(ConstantNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->type = CT_INTEGER;
	p->data.i = data;
	return p;
}

/** Creates a floating point decimal type ConstantNode structure.
  *
  * \return A pointer to a floating point decimal type ConstantNode storing the
  *         desired value.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteConstantNode(ConstantNode *) */
ConstantNode *createFloatConstantNode(float data) /**< [in] The constant floating point decimal data. */
{
	ConstantNode *p = malloc(sizeof(ConstantNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->type = CT_FLOAT;
	p->data.f = data;
	return p;
}

/** Creates a string type ConstantNode structure.
  *
  * \return A pointer to a string type ConstantNode storing the desired value.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteConstantNode(ConstantNode *) */
ConstantNode *createStringConstantNode(char *data) /**< [in] The constant character string data. */
{
	ConstantNode *p = malloc(sizeof(ConstantNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->type = CT_STRING;
	p->data.s = data;
	return p;
}

/** Deletes a ConstantNode structure.
  *
  * \pre \a node was created by either createBooleanConstantNode(int), createIntegerConstantNode(int),
  *      createFloatConstantNode(float), or createStringConstantNode(char *).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createBooleanConstantNode(int)
  * \see createIntegerConstantNode(int)
  * \see createFloatConstantNode(float)
  * \see createStringConstantNode(char *) */
void deleteConstantNode(ConstantNode *node) /**< [in,out] A pointer to the ConstantNode structure to be deleted. */
{
	if (!node) return;
	if (node->type == CT_STRING) free(node->data.s);
	free(node);
}

/** Creates an IdentifierNode structure.
  *
  * \note \a image is \b copied for use within the structure so it must be freed
  *       by the caller.
  *
  * \return A pointer to an IdentifierNode structure with the desired
  *         properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteIdentifierNode(IdentifierNode *) */
IdentifierNode *createIdentifierNode(char *image,       /**< [in] An array of characters that name the identifier. */
                                     const char *fname, /**< [in] A pointer to the name of the file containing the identifier. */
                                     unsigned int line) /**< [in] The line number from the source file that the identifier occurred on. */
{
	IdentifierNode *p = malloc(sizeof(IdentifierNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->image = malloc(sizeof(char) * (strlen(image) + 1));
	if (!p->image) {
		free(p);
		perror("malloc");
		return NULL;
	}
	strcpy(p->image, image);
	p->fname = fname;
	p->line = line;
	return p;
}

/** Deletes an IdentifierNode structure.
  *
  * \pre \a node was created by createIdentifierNode(char *, const char *, unsigned int).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createIdentifierNode(char *, const char *, unsigned int) */
void deleteIdentifierNode(IdentifierNode *node) /**< [in,out] A pointer to the IdentifierNode structure to be deleted. */
{
	if (!node) return;
	free(node->image);
	free(node);
}

/** Creates an IdentifierNodeList structure.
  *
  * \return A pointer to a IdentifierNodeList structure with the desired
  *         properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteIdentifierNodeList(IdentifierNodeList *) */
IdentifierNodeList *createIdentifierNodeList(void)
{
	IdentifierNodeList *p = malloc(sizeof(IdentifierNodeList));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->num = 0;
	p->ids = NULL;
	return p;
}

/** Adds an IdentifierNode structure to an IdentifierNodeList structure.
  *
  * \pre \a list was created by createIdentifierNodeList(void).
  * \pre \a node was created by createIdentifierNode(char *, const char *, unsigned int).
  *
  * \post \a node will be added on to the end of \a list and the size of
  *       \a list will be updated accordingly.
  *
  * \return A pointer to the added IdentifierNode structure (will be the same as
  *         \a node).
  *
  * \retval NULL realloc was unable to allocate memory. */
IdentifierNode *addIdentifierNode(IdentifierNodeList *list,   /**< [in,out] A pointer to the IdentifierNodeList structure to add \a node to. */
                                  IdentifierNode *node) /**< [in] A pointer to the IdentifierNode structure to add to \a list. */
{
	unsigned int newsize = list->num + 1;
	void *mem = realloc(list->ids, sizeof(IdentifierNode *) * newsize);
	if (!mem) {
		perror("realloc");
		return NULL;
	}
	list->ids = mem;
	list->ids[list->num] = node;
	list->num = newsize;
	return node;
}

/** Deletes an IdentifierNodeList structure.
  *
  * \pre \a list was created by createIdentifierNodeList(void) and contains
  *      items added by addIdentifierNode(IdentifierNodeList *, IdentifierNode *).
  *
  * \post The memory at \a list and any of its associated members will be
  *       freed.
  *
  * \see createIdentifierNodeList(void) */
void deleteIdentifierNodeList(IdentifierNodeList *list) /**< [in,out] A pointer to the IdentifierNodeList structure to delete. */
{
	unsigned int n;
	if (!list) return;
	for (n = 0; n < list->num; n++)
		deleteIdentifierNode(list->ids[n]);
	free(list->ids);
	free(list);
}

/** Creates a TypeNode structure.
  *
  * \return A pointer to a TypeNode structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteTypeNode(TypeNode *) */
TypeNode *createTypeNode(ConstantType type) /**< [in] The type of value. */
{
	TypeNode *p = malloc(sizeof(TypeNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->type = type;
	return p;
}

/** Deletes a TypeNode structure.
  *
  * \pre \a node was created by createTypeNode(ConstantType).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createTypeNode(ConstantType) */
void deleteTypeNode(TypeNode *node) /**< [in,out] A pointer to the TypeNode structure to be deleted. */
{
	if (!node) return;
	free(node);
}

/** Creates a StmtNode structure.
  *
  * \pre \a stmt contains a structure created corresponding to \a type:
  *      - ST_CAST: createCastStmtNode(IdentifierNode *, TypeNode *)
  *      - ST_PRINT: createPrintStmtNode(ExprNodeList *, int)
  *      - ST_INPUT: createInputStmtNode(IdentifierNode *)
  *      - ST_ASSIGNMENT: createAssignmentStmtNode(IdentifierNode *, ExprNode *)
  *      - ST_DECLARATION: createDeclarationStmtNode(IdentifierNode *, IdentifierNode *, ExprNode *)
  *      - ST_IFTHENELSE: createIfThenElseStmtNode(BlockNode *, BlockNode *, ExprNodeList *, BlockNodeList *)
  *      - ST_SWITCH: createSwitchStmtNode(ExprNodeList *, BlockNodeList *, BlockNode *)
  *      - ST_BREAK: no structure needed, use \c NULL
  *      - ST_RETURN: createReturnStmtNode(ExprNode *)
  *      - ST_LOOP: createLoopStmtNode(IdentifierNode *, IdentifierNode *, ExprNode *, ExprNode *, BlockNode *)
  *      - ST_FUNCDEF: createFuncDefStmtNode(IdentifierNode *, IdentifierNode *, IdentifierNodeList *, BlockNode *)
  *      - ST_EXPR: createExprNode(ExprType, void *)
  *
  * \return A pointer to a StmtNode structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteStmtNode(StmtNode *) */
StmtNode *createStmtNode(StmtType type, /**< [in] The type of statement stored in \a node. */
                         void *stmt)    /**< [in] A pointer to the particular statement structure. */
{
	StmtNode *p = malloc(sizeof(StmtNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->type = type;
	p->stmt = stmt;
	return p;
}

/** Deletes a StmtNode structure.
  *
  * \pre \a node was created by createStmtNode(StmtType, void *).
  *
  * \post The memory at \a stmt and any of its associated members will be
  *       freed.
  *
  * \see createStmtNode(StmtType, void *) */
void deleteStmtNode(StmtNode *node) /**< [in, out] A pointer to the StmtNode structure to be deleted. */
{
	if (!node) return;
	switch (node->type) {
		case ST_CAST: {
			CastStmtNode *stmt = (CastStmtNode *)node->stmt;
			deleteCastStmtNode(stmt);
			break;
		}
		case ST_PRINT: {
			PrintStmtNode *stmt = (PrintStmtNode *)node->stmt;
			deletePrintStmtNode(stmt);
			break;
		}
		case ST_INPUT: {
			InputStmtNode *stmt = (InputStmtNode *)node->stmt;
			deleteInputStmtNode(stmt);
			break;
		}
		case ST_ASSIGNMENT: {
			AssignmentStmtNode *stmt = (AssignmentStmtNode *)node->stmt;
			deleteAssignmentStmtNode(stmt);
			break;
		}
		case ST_DECLARATION: {
			DeclarationStmtNode *stmt = (DeclarationStmtNode *)node->stmt;
			deleteDeclarationStmtNode(stmt);
			break;
		}
		case ST_IFTHENELSE: {
			IfThenElseStmtNode *stmt = (IfThenElseStmtNode *)node->stmt;
			deleteIfThenElseStmtNode(stmt);
			break;
		}
		case ST_SWITCH: {
			SwitchStmtNode *stmt = (SwitchStmtNode *)node->stmt;
			deleteSwitchStmtNode(stmt);
			break;
		}
		case ST_BREAK:
			break; /* This statement type does not have any content */
		case ST_RETURN: {
			ReturnStmtNode *stmt = (ReturnStmtNode *)node->stmt;
			deleteReturnStmtNode(stmt);
			break;
		}
		case ST_LOOP: {
			LoopStmtNode *stmt = (LoopStmtNode *)node->stmt;
			deleteLoopStmtNode(stmt);
			break;
		}
		case ST_EXPR: {
			ExprNode *expr = (ExprNode *)node->stmt;
			deleteExprNode(expr);
			break;
		}
		case ST_FUNCDEF: {
			FuncDefStmtNode *stmt = (FuncDefStmtNode *)node->stmt;
			deleteFuncDefStmtNode(stmt);
			break;
		}
		default:
			fprintf(stderr, "Unable to delete unknown statement type\n");
			break;
	}
	free(node);
}

/** Creates a StmtNodeList structure.
  *
  * \return A pointer to a StmtNodeList structure with no elements.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteStmtNodeList(StmtNodeList *) */
StmtNodeList *createStmtNodeList(void)
{
	StmtNodeList *p = malloc(sizeof(StmtNodeList));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->num = 0;
	p->stmts = NULL;
	return p;
}

/** Adds a StmtNode to a StmtNodeList structure.
  *
  * \pre \a list was created by createStmtNodeList(void).
  * \pre \a node was created by createStmtNode(StmtType, void *).
  *
  * \post \a node will be added on to the end of \a list and the size of
  *       \a list will be updated accordingly.
  *
  * \return A pointer to the added StmtNode (will be the same as \a node).
  *
  * \retval NULL malloc was unable to allocate memory. */
StmtNode *addStmtNode(StmtNodeList *list, /**< [in,out] A pointer to the StmtNodeList structure to add \a node to. */
                      StmtNode *node)     /**< [in] A pointer to the StmtNode structure to add to \a list. */
{
	unsigned int newsize = list->num + 1;
	void *mem = realloc(list->stmts, sizeof(StmtNode *) * newsize);
	if (!mem) {
		perror("realloc");
		return NULL;
	}
	list->stmts = mem;
	list->stmts[list->num] = node;
	list->num = newsize;
	return node;
}

/** Deletes a StmtNodeList structure.
  *
  * \pre \a list was created by createStmtNodeList(void) and contains items added
  *      by addStmtNode(StmtNodeList *, StmtNode *).
  *
  * \post The memory at \a list and any of its associated members will be
  *       freed.
  *
  * \see createStmtNodeList(void) */
void deleteStmtNodeList(StmtNodeList *list) /**< [in,out] A pointer to the StmtNodeList structure to delete. */
{
	unsigned int n;
	if (!list) return;
	for (n = 0; n < list->num; n++)
		deleteStmtNode(list->stmts[n]);
	free(list->stmts);
	free(list);
}

/** Creates a CastStmtNode structure.
  *
  * \pre \a target was created by createIdentifierNode(char *, const char *, unsigned int).
  * \pre \a newtype was created by createTypeNode(ConstantType).
  *
  * \return A pointer to a CastStmtNode structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteCastStmtNode(CastStmtNode *) */
CastStmtNode *createCastStmtNode(IdentifierNode *target, /**< [in] A pointer to the name of the variable whose type is to be changed to \a newtype. */
                                 TypeNode *newtype)      /**< [in] A pointer to the type to change \a target to. */
{
	CastStmtNode *p = malloc(sizeof(CastStmtNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->target = target;
	p->newtype = newtype;
	return p;
}

/** Deletes a CastStmtNode structure.
  *
  * \pre \a node was created by createCastStmtNode(IdentifierNode *, TypeNode *).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createCastStmtNode(IdentifierNode *, TypeNode *) */
void deleteCastStmtNode(CastStmtNode *node) /**< [in,out] A pointer to the CastStmtNode structure to be deleted. */
{
	if (!node) return;
	deleteIdentifierNode(node->target);
	deleteTypeNode(node->newtype);
	free(node);
}

/** Creates a PrintStmtNode structure.
  *
  * \pre \a args was created by createExprNodeList(void) and contains items
  *      added by addExprNode(ExprNodeList *, ExprNode *).
  *
  * \return A pointer to a PrintStmtNode structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deletePrintStmtNode(PrintStmtNode *) */
PrintStmtNode *createPrintStmtNode(ExprNodeList *args, /**< [in] A pointer to the list of expressions to evaluate and print. */
                                   int nonl)           /**< [in] Denotes an ending newline should be surpressed if not \c 0 and printed if \c 0. */
{
	PrintStmtNode *p = malloc(sizeof(PrintStmtNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->args = args;
	p->nonl = nonl;
	return p;
}

/** Deletes a PrintStmtNode structure.
  *
  * \pre \a node was created by createPrintStmtNode(ExprNodeList *, int).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createPrintStmtNode(ExprNodeList *, int) */
void deletePrintStmtNode(PrintStmtNode *node) /**< [in,out] A pointer to the PrintStmtNode structure to be deleted. */
{
	if (!node) return;
	deleteExprNodeList(node->args);
	free(node);
}

/** Creates an InputStmtNode structure.
  *
  * \pre \a target was created by createIdentifierNode(char *, const char *, unsigned int).
  *
  * \return A pointer to an InputStmtNode structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteInputStmtNode(InputStmtNode *) */
InputStmtNode *createInputStmtNode(IdentifierNode *target) /**< [in] A pointer to the name of the variable to store the input in. */
{
	InputStmtNode *p = malloc(sizeof(InputStmtNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->target = target;
	return p;
}

/** Deletes an InputStmtNode structure.
  *
  * \pre \a node was created by createInputStmtNode(IdentifierNode *).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createInputStmtNode(IdentifierNode *) */
void deleteInputStmtNode(InputStmtNode *node) /**< [in,out] A pointer to the InputStmtNode structure to be deleted. */
{
	if (!node) return;
	deleteIdentifierNode(node->target);
	free(node);
}

/** Creates an AssignmentStmtNode structure.
  *
  * \pre \a target was created by createIdentifierNode(char *, const char *, unsigned int).
  * \pre \a expr was created by createExprNode(ExprType, void *).
  *
  * \return A pointer to an AssignmentStmtNode structure with the desired
  *         properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteAssignmentStmtNode(AssignmentStmtNode *) */
AssignmentStmtNode *createAssignmentStmtNode(IdentifierNode *target, /**< [in] A pointer to the name of the variable to store the evaluated contents of a \a expr into. */
                                             ExprNode *expr)         /**< [in] A pointer to the expression to evaluate and store in \a target. */
{
	AssignmentStmtNode *p = malloc(sizeof(AssignmentStmtNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->target = target;
	p->expr = expr;
	return p;
}

/** Deletes an AssignmentStmtNode structure.
  *
  * \pre \a node was created by createAssignmentStmtNode(IdentifierNode *, ExprNode *).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createAssignmentStmtNode(IdentifierNode *, ExprNode *) */
void deleteAssignmentStmtNode(AssignmentStmtNode *node) /**< [in,out] A pointer to the AssignmentStmtNode structure to be deleted. */
{
	if (!node) return;
	deleteIdentifierNode(node->target);
	deleteExprNode(node->expr);
	free(node);
}

/** Creates a DeclarationStmtNode structure.
  *
  * \pre \a scope was created by createIdentifierNode(char *, const char *, unsigned int).
  * \pre \a target was created by createIdentifierNode(char *, const char *, unsigned int).
  * \pre \a expr was created by createExprNode(ExprType, void *).
  *
  * \return A pointer to a DeclarationStmtNode structure with the desired
  *         properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteDeclarationStmtNode(DeclarationStmtNode *) */
DeclarationStmtNode *createDeclarationStmtNode(IdentifierNode *scope,  /**< [in] A pointer to the scope to create the variable in. */
                                               IdentifierNode *target, /**< [in] A pointer to the name of the variable to create. */
                                               ExprNode *expr)         /**< [in] An optional pointer to the expression to initialize \a target to. */
{
	DeclarationStmtNode *p = malloc(sizeof(DeclarationStmtNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->scope = scope;
	p->target = target;
	p->expr = expr;
	return p;
}

/** Deletes a DeclarationStmtNode structure.
  *
  * \pre \a node was created by createDeclarationStmtNode(IdentifierNode *, IdentifierNode *, ExprNode *).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createDeclarationStmtNode(IdentifierNode *, IdentifierNode *, ExprNode *) */
void deleteDeclarationStmtNode(DeclarationStmtNode *node) /**< [in,out] A pointer to the DeclarationStmtNode structure to be deleted. */
{
	if (!node) return;
	deleteIdentifierNode(node->scope);
	deleteIdentifierNode(node->target);
	deleteExprNode(node->expr);
	free(node);
}

/** Creates an IfThenElseStmtNode structure.
  *
  * \pre \a yes was created by createBlockNode(StmtNodeList *).
  * \pre \a no was created by createBlockNode(StmtNodeList *).
  * \pre \a guards was created by createExprNodeList(void) and contains items
  *      added by addExprNode(ExprNodeList *, ExprNode *).
  * \pre \a blocks was created by createBlockNodeList(void) and contains items
  *      added by addBlockNode(BlockNodeList *, BlockNode *).
  *
  * \return A pointer to a IfThenElseStmtNode structure with the desired
  *         properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteIfThenElseStmtNode(IfThenElseStmtNode *) */
IfThenElseStmtNode *createIfThenElseStmtNode(BlockNode *yes,        /**< [in] A pointer to the block of code to execute if the \ref impvar "implicit variable" casts to \c false. */
                                             BlockNode *no,         /**< [in] A pointer to the block of code to execute if the \ref impvar "implicit variable" casts to \c false \b and the evaluations of all of the \a guards cast to \c false. */
                                             ExprNodeList *guards,  /**< [in] A pointer to the expressions to test if the \ref impvar "implicit variable" casts to \c false. */
                                             BlockNodeList *blocks) /**< [in] A pointer to the respective blocks of code to execute if one of the evaluated \a guards casts to \c true. */
{
	IfThenElseStmtNode *p = malloc(sizeof(IfThenElseStmtNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->yes = yes;
	p->no = no;
	p->guards = guards;
	p->blocks = blocks;
	return p;
}

/** Deletes a IfThenElseStmtNode structure.
  *
  * \pre \a node was created by createIfThenElseStmtNode(BlockNode *, BlockNode *, ExprNodeList *, BlockNodeList *).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createIfThenElseStmtNode(BlockNode *, BlockNode *, ExprNodeList *, BlockNodeList *) */
void deleteIfThenElseStmtNode(IfThenElseStmtNode *node) /**< [in,out] A pointer to the IfThenElseStmtNode structure to be deleted. */
{
	if (!node) return;
	deleteBlockNode(node->yes);
	deleteBlockNode(node->no);
	deleteExprNodeList(node->guards);
	deleteBlockNodeList(node->blocks);
	free(node);
}

/** Creates a SwitchStmtNode structure.
  *
  * \pre \a guards was created by createExprNodeList(void) and contains items
  *      added by addExprNode(ExprNodeList *, ExprNode *).
  * \pre \a blocks was created by createBlockNodeList(void) and contains items
  *      added by addBlockNode(BlockNodeList *, BlockNode *).
  * \pre \a def was created by createBlockNode(StmtNodeList *).
  *
  * \return A pointer to a SwitchStmtNode structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteSwitchStmtNode(SwitchStmtNode *) */
SwitchStmtNode *createSwitchStmtNode(ExprNodeList *guards,  /**< [in] A pointer to the expressions to evaluate and compare to the \ref impvar "implicit variable". */
                                     BlockNodeList *blocks, /**< [in] A pointer to the respective blocks of code to execute if one of the \a guards matches the \ref impvar "implicit variable". */
                                     BlockNode *def)        /**< [in] A pointer to the default block of code to execute if none of the \a guards match the \ref impvar "implicit variable". */
{
	SwitchStmtNode *p = malloc(sizeof(SwitchStmtNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->guards = guards;
	p->blocks = blocks;
	p->def = def;
	return p;
}

/** Deletes a SwitchStmtNode structure.
  *
  * \pre \a node was created by createSwitchStmtNode(ExprNodeList *, BlockNodeList *, BlockNode *).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createSwitchStmtNode(ExprNodeList *, BlockNodeList *, BlockNode *) */
void deleteSwitchStmtNode(SwitchStmtNode *node) /**< [in,out] A pointer to the SwitchStmtNode structure to be deleted. */
{
	if (!node) return;
	deleteExprNodeList(node->guards);
	deleteBlockNodeList(node->blocks);
	deleteBlockNode(node->def);
	free(node);
}

/** Creates a ReturnStmtNode structure.
  *
  * \pre \a value was created by createExprNode(ExprType, void *).
  *
  * \return A pointer to a ReturnStmtNode structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteReturnStmtNode(ReturnStmtNode *) */
ReturnStmtNode *createReturnStmtNode(ExprNode *value) /**< [in] A pointer to the value to return. */
{
	ReturnStmtNode *p = malloc(sizeof(ReturnStmtNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->value = value;
	return p;
}

/** Deletes a ReturnStmtNode structure.
  *
  * \pre \a node was created by createReturnStmtNode(ExprNode *).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createReturnStmtNode(ExprNode *) */
void deleteReturnStmtNode(ReturnStmtNode *node) /**< [in,out] A pointer to the ReturnStmtNode structure to be deleted. */
{
	if (!node) return;
	deleteExprNode(node->value);
	free(node);
}

/** Creates a LoopStmtNode structure.
  *
  * \pre \a name was created by createIdentifierNode(char *, const char *, unsigned int).
  * \pre \a var was created by createIdentifierNode(char *, const char *, unsigned int).
  * \pre \a guard was created by createExprNode(ExprType, void *).
  * \pre \a update was created by createExprNode(ExprType, void *).
  * \pre \a body was created by createBlockNode(StmtNodeList *).
  *
  * \return A pointer to a LoopStmtNode structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteLoopStmtNode(LoopStmtNode *) */
LoopStmtNode *createLoopStmtNode(IdentifierNode *name, /**< [in] A pointer to the name of the loop. */
                                 IdentifierNode *var,  /**< [in] A pointer to the name of the variable to be updated by \a update. */
                                 ExprNode *guard,      /**< [in] A pointer to the expression to determine if the loop will continue. */
                                 ExprNode *update,     /**< [in] A pointer to the expression to evaluate to update \a var. */
                                 BlockNode *body)      /**< [in] A pointer to the block of code to be executed with each iteration of the loop. */
{
	LoopStmtNode *p = malloc(sizeof(LoopStmtNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->name = name;
	p->var = var;
	p->guard = guard;
	p->update = update;
	p->body = body;
	return p;
}

/** Deletes a LoopStmtNode structure.
  *
  * \pre \a node was created by createLoopStmtNode(IdentifierNode *, IdentifierNode *, ExprNode *, ExprNode *, BlockNode *).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createLoopStmtNode(IdentifierNode *, IdentifierNode *, ExprNode *, ExprNode *, BlockNode *) */
void deleteLoopStmtNode(LoopStmtNode *node) /**< [in,out] A pointer to the LoopStmtNode structure to be deleted. */
{
	if (!node) return;
	deleteIdentifierNode(node->name);
	deleteIdentifierNode(node->var);
	deleteExprNode(node->guard);
	deleteExprNode(node->update);
	deleteBlockNode(node->body);
	free(node);
}

/** Creates a FuncDefStmtNode structure.
  *
  * \pre \a scope was created by createIdentifierNode(char *, const char *, unsigned int).
  * \pre \a name was created by createIdentifierNode(char *, const char *, unsigned int).
  * \pre \a args was created by createIdentifierNodeList(void) and contains
  *      items added by addIdentifierNode(IdentifierNodeList *, IdentifierNode *).
  * \pre \a body was created by createBlockNode(StmtNodeList *).
  *
  * \return A pointer to a FuncDefStmtNode structure with the desired
  *         properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteFuncDefStmtNode(FuncDefStmtNode *) */
FuncDefStmtNode *createFuncDefStmtNode(IdentifierNode *scope,    /**< [in] A pointer to the scope to define the function in. */
                                       IdentifierNode *name,     /**< [in] A pointer to the name of the function. */
                                       IdentifierNodeList *args, /**< [in] A pointer to an array of the names of the arguments of the function. */
                                       BlockNode *body)          /**< [in] A pointer to the block of code defined by the function. */
{
	FuncDefStmtNode *p = malloc(sizeof(FuncDefStmtNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->scope = scope;
	p->name = name;
	p->args = args;
	p->body = body;
	return p;
}

/** Deletes a FuncDefStmtNode structure.
  *
  * \pre \a node was created by createFuncDefStmtNode(IdentifierNode *, IdentifierNode *, IdentifierNodeList *, BlockNode *).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createFuncDefStmtNode(IdentifierNode *, IdentifierNode *, IdentifierNodeList *, BlockNode *) */
void deleteFuncDefStmtNode(FuncDefStmtNode *node) /**< [in,out] A pointer to the FuncDefStmtNode structure to be deleted. */
{
	if (!node) return;
	deleteIdentifierNode(node->scope);
	deleteIdentifierNode(node->name);
	deleteIdentifierNodeList(node->args);
	deleteBlockNode(node->body);
	free(node);
}

/** Creates an ExprNode structure.
  *
  * \pre \a expr contains a structure created corresponding to \a type:
  *      - ET_CAST: createCastExprNode(ExprNode *, TypeNode *)
  *      - ET_CONSTANT: createBooleanConstantNode(int), createIntegerConstantNode(int),
  *        createFloatConstantNode(float), or createStringConstantNode(char *).
  *      - ET_IDENTIFIER: createIdentifierNode(char *, const char *, unsigned int)
  *      - ET_FUNCCALL: createFuncCallExprNode(FuncDefStmtNode *, ExprNodeList *)
  *      - ET_OP: createOpExprNode(OpType, ExprNodeList *)
  *      - ET_IMPVAR: (for the \ref impvar "implicit variable") no structure needed, use \c NULL
  *
  * \return A pointer to an ExprNode structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteExprNode(ExprNode *) */
ExprNode *createExprNode(ExprType type, /**< [in] The type of expression stored in \a expr. */
                         void *expr)    /**< [in] A pointer to the particular expression structure. */
{
	ExprNode *p = malloc(sizeof(ExprNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->type = type;
	p->expr = expr;
	return p;
}

/** Deletes an ExprNode structure.
  *
  * \pre \a node was created by createExprNode(ExprType, void *).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createExprNode(ExprType, void *) */
void deleteExprNode(ExprNode *node) /**< [in,out] A pointer to the ExprNode structure to be deleted. */
{
	if (!node) return;
	switch (node->type) {
		case ET_CAST:
			deleteCastExprNode((CastExprNode *)node->expr);
			break;
		case ET_CONSTANT:
			deleteConstantNode((ConstantNode *)node->expr);
			break;
		case ET_IDENTIFIER:
			deleteIdentifierNode((IdentifierNode *)node->expr);
			break;
		case ET_FUNCCALL:
			deleteFuncCallExprNode((FuncCallExprNode *)node->expr);
			break;
		case ET_OP:
			deleteOpExprNode((OpExprNode *)node->expr);
			break;
		case ET_IMPVAR:
			break; /* This expression type does not have any content */
		default:
			fprintf(stderr, "Unable to delete unknown expression type\n");
			break;
	}
	free(node);
}

/** Creates an ExprNodeList structure.
  *
  * \return A pointer to an ExprNodeList structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteExprNodeList(ExprNodeList *) */
ExprNodeList *createExprNodeList(void)
{
	ExprNodeList *p = malloc(sizeof(ExprNodeList));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->num = 0;
	p->exprs = NULL;
	return p;
}

/** Adds an ExprNode structure to an ExprNodeList structure.
  *
  * \pre \a list was created by createExprNodeList(void).
  * \pre \a node was created by createExprNode(ExprType, void *).
  *
  * \post \a node will be added on to the end of \a list and the size of
  *       \a list will be updated accordingly.
  *
  * \return A pointer to the added ExprNode structure (will be the same as
  *         \a node).
  *
  * \retval NULL realloc was unable to allocate memory. */
ExprNode *addExprNode(ExprNodeList *list, /**< [in,out] A pointer to the ExprNodeList structure to add \a node to. */
                      ExprNode *node)     /**< [in] A pointer to the ExprNode structure to add to \a list. */
{
	unsigned int newsize = list->num + 1;
	void *mem = realloc(list->exprs, sizeof(ExprNode *) * newsize);
	if (!mem) {
		perror("realloc");
		return NULL;
	}
	list->exprs = mem;
	list->exprs[list->num] = node;
	list->num = newsize;
	return node;
}

/** Deletes an ExprNodeList structure.
  *
  * \pre \a list was created by createExprNodeList(void) and contains items
  *      added by addExprNode(ExprNodeList *, ExprNode *).
  *
  * \post The memory at \a list and any of its associated members will be
  *       freed.
  *
  * \see createExprNodeList(void) */
void deleteExprNodeList(ExprNodeList *list) /**< [in,out] A pointer to the ExprNodeList structure to delete. */
{
	unsigned int n;
	if (!list) return;
	for (n = 0; n < list->num; n++)
		deleteExprNode(list->exprs[n]);
	free(list->exprs);
	free(list);
}

/** Creates a CastExprNode structure.
  *
  * \pre \a target was created by createExprNode(ExprType, void *).
  * \pre \a newtype was created by createTypeNode(ConstantType).
  *
  * \return A pointer to a CastExprNode structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteCastExprNode(CastExprNode *) */
CastExprNode *createCastExprNode(ExprNode *target,  /**< [in] A pointer to the expression to cast. */
                                 TypeNode *newtype) /**< [in] A pointer to the type to cast the copy of \a target to. */
{
	CastExprNode *p = malloc(sizeof(CastExprNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->target = target;
	p->newtype = newtype;
	return p;
}

/** Deletes a CastExprNode structure.
  *
  * \pre \a node was created by createCastExprNode(ExprNode *, TypeNode *).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createCastExprNode(ExprNode *, TypeNode *) */
void deleteCastExprNode(CastExprNode *node) /**< [in,out] A pointer to the CastExprNode structure to be deleted. */
{
	if (!node) return;
	deleteExprNode(node->target);
	deleteTypeNode(node->newtype);
	free(node);
}

/** Creates a FuncCallExprNode structure.
  *
  * \pre \a def was created by createFuncDefStmtNode(IdentifierNode *, IdentifierNode *, IdentifierNodeList *, BlockNode *).
  * \pre \a args was created by createExprNodeList(void) and contains items
  *      added by addExprNode(ExprNodeList *, ExprNode *).
  *
  * \return A pointer to a FuncCallExprNode structure with the desired
  *         properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteFuncCallExprNode(FuncCallExprNode *) */
FuncCallExprNode *createFuncCallExprNode(FuncDefStmtNode *def, /**< [in] A pointer to the function definition to call. */
                                         ExprNodeList *args)   /**< [in] A pointer to an ExprNodeList structure of arguments to be supplied to the function defined by \a def. */
{
	FuncCallExprNode *p = malloc(sizeof(FuncCallExprNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->def = def;
	p->args = args;
	return p;
}

/** Deletes a FuncCallExprNode structure.
  *
  * \pre \a node was created by createFuncCallExprNode(FuncDefStmtNode *, ExprNodeList *).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createFuncCallExprNode(FuncDefStmtNode *, ExprNodeList *) */
void deleteFuncCallExprNode(FuncCallExprNode *node) /**< [in,out] A pointer to the FuncCallExprNode structure to be deleted. */
{
	if (!node) return;
	deleteExprNodeList(node->args);
	free(node);
}

/** Creates an OpExprNode structure.
  *
  * \pre \a args was created by createExprNodeList(void) and contains items
  *      added by addExprNode(ExprNodeList *, ExprNode *).
  *
  * \return A pointer to an OpExprNode structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteOpExprNode(OpExprNode *) */
OpExprNode *createOpExprNode(OpType type,        /**< [in] The type of operation to perform on \a args. */
                             ExprNodeList *args) /**< [in] A pointer to the arguments to perform the operation on. */
{
	OpExprNode *p = malloc(sizeof(OpExprNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->type = type;
	p->args = args;
	return p;
}

/** Deletes an OpExprNode structure.
  *
  * \pre \a node was created by createOpExprNode(OpType, ExprNodeList *).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createOpExprNode(OpType, ExprNodeList *) */
void deleteOpExprNode(OpExprNode *node) /**< [in,out] A pointer to the OpExprNode structure to be deleted. */
{
	if (!node) return;
	deleteExprNodeList(node->args);
	free(node);
}

/** Creates a FunctionTable structure.
  *
  * \return A pointer to a FunctionTable structure with no elements.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteFunctionTable(FunctionTable *) */
FunctionTable *createFunctionTable(void)
{
	FunctionTable *p = malloc(sizeof(FunctionTable));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->num = 0;
	p->funcs = NULL;
	return p;
}

/** Adds a FuncDefStmtNode to a FunctionTable structure.
  *
  * \pre \a table was created by createFunctionTable(void).
  * \pre \a node was created by createFuncDefStmtNode(IdentifierNode *, IdentifierNode *, IdentifierNodeList *, BlockNode *).
  *
  * \post \a node will be added on to the end of \a table and the size of
  *       \a table will be updated accordingly.
  *
  * \return A pointer to the added FuncDefStmtNode structure (will be the same
  *         as \a node).
  *
  * \retval NULL realloc was unable to allocate memory.
  *
  * \see lookupFuncDefStmtNode(FunctionTable *, const char *) */
FuncDefStmtNode *addFuncDefStmtNode(FunctionTable *table,  /**< [in,out] A pointer to the FunctionTable structure to add \a node to. */
                                    FuncDefStmtNode *node) /**< [in] A pointer to the FuncDefStmtNode structure to add to \a table. */
{
	unsigned int newsize = table->num + 1;
	void *mem = realloc(table->funcs, sizeof(FuncDefStmtNode *) * newsize);
	if (!mem) {
		perror("realloc");
		return NULL;
	}
	table->funcs = mem;
	table->funcs[table->num] = node;
	table->num = newsize;
	return node;
}

/** Deletes a FunctionTable structure.
  *
  * \pre \a table was created by createFunctionTable(void).
  *
  * \post The memory of the function table and any of its associated members
  *      will be freed.
  *
  * \see createFunctionTable(void) */
void deleteFunctionTable(FunctionTable *table) /**< [in,out] A pointer to the FunctionTable structure to delete. */
{
	if (!table) return;
	/* Don't delete the FuncDefStmtNode structures because they will be
         * deleted by deleteMainNode. */
	free(table->funcs);
	free(table);
}

/** Looks up a function definition by its name.
  *
  * \pre \a table was created by createFunctionTable(void) and contains items
  *      added by addFuncDefStmtNode(FunctionTable *, FuncDefStmtNode *).
  *
  * \return A pointer to the FuncDefStmtNode structure named \a name.
  *
  * \retval NULL No such FuncDefStmtNode structure exists.
  *
  * \see addFuncDefStmtNode(FunctionTable *, FuncDefStmtNode *) */
FuncDefStmtNode *lookupFuncDefStmtNode(FunctionTable *table, /**< [in] A pointer to the FunctionTable structure to search for \a name. */
                                       const char *name)  /**< [in] The name of the function to look up. */
{
	unsigned int n;
	for (n = 0; n < table->num; n++) {
		FuncDefStmtNode *fun = (table->funcs)[n];
		if (!strcmp(fun->name->image, name)) return fun;
	}
	return NULL;
}

/** Checks if the token pointed to by \a tokenp matches \a token and if it does,
  * advances the token pointer to the next token in the array.
  *
  * \note Unlike peekToken(Token ***, TokenType) and nextToken(Token ***, TokenType), this
  *       function \b does modify \a tokenp.
  *
  * \pre \a tokenp points to a position in an array of tokens created by
  *      tokenizeLexemes(Lexeme **).
  *
  * \post If the token pointed to by \a tokenp does not match \a token,
  *       \a tokenp will point to the same token as when the function was
  *       called.
  * \post If the token pointed to by \a tokenp matches \a token, \a tokenp will
  *       point to the next token \b after the one matched.
  *
  * \retval 0 The token does not match \a token.
  * \retval 1 The token matches \a token.
  *
  * \see peekToken(Token ***, TokenType)
  * \see nextToken(Token ***, TokenType) */
int acceptToken(Token ***tokenp, /**< [in,out] A pointer to the position of the next token to parse in an array of Token structures. */
                TokenType token) /**< [in] The type of token to match. */
{
	Token **tokens = *tokenp;
	if ((*tokens)->type != token) return 0;
	tokens++;
	*tokenp = tokens;
	return 1;
}

/** Checks if the token pointed to by \a tokenp matches \a token.
  *
  * \note Unlike acceptToken(Token ***, TokenType), this function does not
  *       modify \a tokenp.
  *
  * \pre \a tokenp points to a position in an array of tokens created by
  *      tokenizeLexemes(Lexeme **).
  *
  * \post \a tokenp will point to the same token as when the function was
  *       called.
  *
  * \retval 0 The token does not match \a token.
  * \retval 1 The token matches \a token.
  *
  * \see acceptToken(Token ***, TokenType)
  * \see nextToken(Token ***, TokenType) */
int peekToken(Token ***tokenp, /**< [in] A pointer to the position of the next token to parse in an array of Token structures. */
              TokenType token) /**< [in] The type of token to match. */
{
	Token **tokens = *tokenp;
	if ((*tokens)->type != token) return 0;
	return 1;
}

/** Checks if the token \b after the one pointed to by \a tokenp matches
  * \a token.
  *
  * \note Unlike acceptToken(Token ***, TokenType), this function does not
  *       modify \a tokenp.
  *
  * \pre \a tokenp points to a position in an array of tokens created by
  *      tokenizeLexemes(Lexeme **).
  *
  * \post \a tokenp will point to the same token as when the function was
  *       called.
  *
  * \retval 0 The next token does not match \a token.
  * \retval 1 The next token matches \a token.
  *
  * \see acceptToken(Token ***, TokenType)
  * \see peekToken(Token ***, TokenType) */
int nextToken(Token ***tokenp, /**< [in] A pointer to the position of the next token to parse in an array of Token structures. */
         TokenType token) /**< [in] The type of token to match. */
{
	Token **tokens = *tokenp;
	if ((*(tokens + 1))->type != token) return 0;
	return 1;
}

/** Prints an error message of the form "LINE: INFO before: NEXT.\n", where LINE
  * is the line the next token appears on, INFO is \a info and NEXT is the image
  * of the next token.
  *
  * \pre \a tokens was created by tokenizeLexemes(Lexeme **). */
void error(const char *info, /**< [in] The array of characters to print. */
           Token **tokens)   /**< [in] A pointer to an array of tokens to parse. */
{
	fprintf(stderr, "%s:%d: %s at: %s\n", (*tokens)->fname, (*tokens)->line, info, (*tokens)->image);
}

/** Parses a set of tokens into a ConstantNode structure.  Parsing begins at the
  * token pointed to by \a tokenp.
  *
  * \pre \a tokenp points to a position in an array of tokens created by
  *      tokenizeLexemes(Lexeme **).
  *
  * \post \a tokenp will point to the next \b unparsed token in the array.
  *
  * \return A pointer to the generated ConstantNode structure.
  *
  * \retval NULL An error occurred during parsing.
  *
  * \see parseTypeNode(Token ***)
  * \see parseIdentifierNode(Token ***)
  * \see parseExprNode(Token ***, FunctionTable *)
  * \see parseStmtNode(Token ***, FunctionTable *)
  * \see parseBlockNode(Token ***, FunctionTable *)
  * \see parseMainNode(Token **, FunctionTable *) */
ConstantNode *parseConstantNode(Token ***tokenp)
{
	Token **tokens = *tokenp;
	ConstantNode *ret = NULL;
#ifdef DEBUG
	shiftout();
#endif
	/* Boolean */
	if (peekToken(&tokens, TT_BOOLEAN)) {
#ifdef DEBUG
		debug("CT_BOOLEAN");
#endif
		ret = createBooleanConstantNode((*tokens)->data.i);
		if (!ret) return NULL;
		acceptToken(&tokens, TT_BOOLEAN); /* Will succeed, checked for this earlier */
	}
	/* Integer */
	else if (peekToken(&tokens, TT_INTEGER)) {
#ifdef DEBUG
		debug("CT_INTEGER");
#endif
		ret = createIntegerConstantNode((*tokens)->data.i);
		if (!ret) return NULL;
		acceptToken(&tokens, TT_INTEGER); /* Will succeed, checked for this above */
	}
	/* Float */
	else if (peekToken(&tokens, TT_FLOAT)) {
#ifdef DEBUG
		debug("CT_FLOAT");
#endif
		ret = createFloatConstantNode((*tokens)->data.f);
		if (!ret) return NULL;
		acceptToken(&tokens, TT_FLOAT); /* Will succeed, checked for this above */
	}
	/* String */
	else if (peekToken(&tokens, TT_STRING)) {
		unsigned int len = strlen((*tokens)->image);
		char *data = malloc(sizeof(char) * (len - 1));
		if (!data) {
			perror("malloc");
			return NULL;
		}
		strncpy(data, (*tokens)->image + 1, len - 2);
		data[len - 2] = '\0';
#ifdef DEBUG
		debug("CT_STRING");
#endif
		ret = createStringConstantNode(data);
		if (!ret) {
			free(data);
			return NULL;
		}
		acceptToken(&tokens, TT_STRING); /* Will succeed, checked for this above */
	}
	else error("expected constant value", tokens);
#ifdef DEBUG
	shiftin();
#endif
	*tokenp = tokens;
	return ret;
}

/** Parses a set of tokens into a TypeNode structure.  Parsing begins at the
  * token pointed to by \a tokenp.
  *
  * \pre \a tokenp points to a position in an array of tokens created by
  *      tokenizeLexemes(Lexeme **).
  *
  * \post \a tokenp will point to the next \b unparsed token in the array.
  *
  * \return A pointer to the generated ExprNode structure.
  *
  * \retval NULL An error occurred during parsing.
  *
  * \see parseConstantNode(Token ***)
  * \see parseIdentifierNode(Token ***)
  * \see parseExprNode(Token ***, FunctionTable *)
  * \see parseStmtNode(Token ***, FunctionTable *)
  * \see parseBlockNode(Token ***, FunctionTable *)
  * \see parseMainNode(Token **, FunctionTable *) */
TypeNode *parseTypeNode(Token ***tokenp) /**< [in,out] A pointer to the position of the next token to parse. */
{
	Token **tokens = *tokenp;
	TypeNode *ret = NULL;
#ifdef DEBUG
	shiftout();
#endif
	/* Nil */
	if (acceptToken(&tokens, TT_NOOB)) {
#ifdef DEBUG
		debug("CT_NIL");
#endif
		ret = createTypeNode(CT_NIL);
		if (!ret) return NULL;
	}
	/* Boolean */
	else if (acceptToken(&tokens, TT_TROOF)) {
#ifdef DEBUG
		debug("CT_BOOLEAN");
#endif
		ret = createTypeNode(CT_BOOLEAN);
		if (!ret) return NULL;
	}
	/* Integer */
	else if (acceptToken(&tokens, TT_NUMBR)) {
#ifdef DEBUG
		debug("CT_INTEGER");
#endif
		ret = createTypeNode(CT_INTEGER);
		if (!ret) return NULL;
	}
	/* Float */
	else if (acceptToken(&tokens, TT_NUMBAR)) {
#ifdef DEBUG
		debug("CT_FLOAT");
#endif
		ret = createTypeNode(CT_FLOAT);
		if (!ret) return NULL;
	}
	/* String */
	else if (acceptToken(&tokens, TT_YARN)) {
#ifdef DEBUG
		debug("CT_STRING");
#endif
		ret = createTypeNode(CT_STRING);
		if (!ret) return NULL;
	}
	else error("expected type", tokens);
#ifdef DEBUG
	shiftin();
#endif
	*tokenp = tokens;
	return ret;
}

/** Parses a set of tokens into an IdentifierNode structure.  Parsing begins at
  * the token pointed to by \a tokenp.
  *
  * \pre \a tokenp points to a position in an array of tokens created by
  *     tokenizeLexemes(Lexeme **).
  *
  * \post \a tokenp will point to the next \b unparsed token in the array.
  *
  * \return A pointer to the generated ExprNode structure.
  *
  * \retval NULL An error occurred during parsing.
  *
  * \see parseConstantNode(Token ***)
  * \see parseTypeNode(Token ***)
  * \see parseExprNode(Token ***, FunctionTable *)
  * \see parseStmtNode(Token ***, FunctionTable *)
  * \see parseBlockNode(Token ***, FunctionTable *)
  * \see parseMainNode(Token **, FunctionTable *) */
IdentifierNode *parseIdentifierNode(Token ***tokenp) /**< [in,out] A pointer to the position of the next token to parse. */
{
	Token **tokens = *tokenp;
	IdentifierNode *ret = NULL;
#ifdef DEBUG
	shiftout();
#endif
	if (peekToken(&tokens, TT_IDENTIFIER)) {
#ifdef DEBUG
		debug("IDENTIFIER");
#endif
		ret = createIdentifierNode((*tokens)->image, (*tokens)->fname, (*tokens)->line);
		if (!ret) return NULL;
		acceptToken(&tokens, TT_IDENTIFIER); /* Will succeed, checked for this above */
	}
	else error("expected identifier", tokens);
#ifdef DEBUG
	shiftin();
#endif
	*tokenp = tokens;
	return ret;
}

/** Parses a set of tokens into an ExprNode structure.  Parsing begins at the
  * token pointed to by \a tokenp.
  *
  * \pre \a tokenp points to a position in an array of tokens created by
  *     tokenizeLexemes(Lexeme **).
  *
  * \post \a tokenp will point to the next \b unparsed token in the array.
  *
  * \return A pointer to the generated ExprNode structure.
  *
  * \retval NULL An error occurred during parsing.
  *
  * \see parseConstantNode(Token ***)
  * \see parseTypeNode(Token ***)
  * \see parseIdentifierNode(Token ***)
  * \see parseStmtNode(Token ***, FunctionTable *)
  * \see parseBlockNode(Token ***, FunctionTable *)
  * \see parseMainNode(Token **, FunctionTable *) */
ExprNode *parseExprNode(Token ***tokenp,        /**< [in,out] A pointer to the position of the next token to parse. */
                        FunctionTable *functab) /**< [in,out] A pointer to the table of defined functions. */
{
	Token **tokens = *tokenp;
	ExprNode *ret = NULL;
#ifdef DEBUG
	shiftout();
#endif
	/* Cast */
	if (acceptToken(&tokens, TT_MAEK)) {
		ExprNode *target = NULL;
		TypeNode *newtype = NULL;
		CastExprNode *expr = NULL;
#ifdef DEBUG
		debug("ET_CAST");
#endif
		target = parseExprNode(&tokens, functab);
		if (!target) return NULL;
		acceptToken(&tokens, TT_A);
		newtype = parseTypeNode(&tokens);
		if (!newtype) {
			deleteExprNode(target);
			return NULL;
		}
		expr = createCastExprNode(target, newtype);
		if (!expr) {
			deleteExprNode(target);
			deleteTypeNode(newtype);
			return NULL;
		}
		ret = createExprNode(ET_CAST, expr);
		if (!ret) {
			deleteExprNode(target);
			deleteTypeNode(newtype);
			deleteCastExprNode(expr);
			return NULL;
		}
	}
	/* Implicit variable */
	else if (acceptToken(&tokens, TT_IT)) {
		ret = createExprNode(ET_IMPVAR, NULL);
		if (!ret) return NULL;
	}
	/* Constant value */
	else if (peekToken(&tokens, TT_BOOLEAN)
			|| peekToken(&tokens, TT_INTEGER)
			|| peekToken(&tokens, TT_FLOAT)
			|| peekToken(&tokens, TT_STRING)) {
		ConstantNode *node = NULL;
#ifdef DEBUG
		debug("ET_CONSTANT");
#endif
		node = parseConstantNode(&tokens);
		if (!node) return NULL;
		ret = createExprNode(ET_CONSTANT, node);
		if (!ret) {
			deleteConstantNode(node);
			return NULL;
		}
	}
	/* Note that identifiers may refer to function calls and if so, must
	 * contain an appropriate number of arguments. */
	else if (peekToken(&tokens, TT_IDENTIFIER)) {
		FuncDefStmtNode *fun = NULL;
		/* Function Call */
		if ((fun = lookupFuncDefStmtNode(functab, (*tokens)->image))) {
			FuncCallExprNode *node = NULL;
			ExprNodeList *args = NULL;
			unsigned int n;
			acceptToken(&tokens, TT_IDENTIFIER); /* Will succeed, checked for this above */
#ifdef DEBUG
			debug("ET_FUNCCALL");
#endif
			args = createExprNodeList();
			for (n = 0; n < fun->args->num; n++) {
				ExprNode *arg = parseExprNode(&tokens, functab);
				if (!arg) {
					deleteExprNodeList(args);
					return NULL;
				}
				addExprNode(args, arg);
			}
			node = createFuncCallExprNode(fun, args);
			if (!node) {
				deleteExprNodeList(args);
				return NULL;
			}
			ret = createExprNode(ET_FUNCCALL, node);
			if (!ret) {
				deleteExprNodeList(args);
				deleteFuncCallExprNode(node);
				return NULL;
			}
		}
		/* Identifier */
		else {
			IdentifierNode *node = NULL;
#ifdef DEBUG
			debug("ET_IDENTIFIER");
#endif
			node = parseIdentifierNode(&tokens);
			if (!node) return NULL;
			ret = createExprNode(ET_IDENTIFIER, node);
			if (!ret) {
				deleteIdentifierNode(node);
				return NULL;
			}
		}
	}
	/* Binary operations */
	else if (peekToken(&tokens, TT_SUMOF)
			|| peekToken(&tokens, TT_DIFFOF)
			|| peekToken(&tokens, TT_PRODUKTOF)
			|| peekToken(&tokens, TT_QUOSHUNTOF)
			|| peekToken(&tokens, TT_MODOF)
			|| peekToken(&tokens, TT_BIGGROF)
			|| peekToken(&tokens, TT_SMALLROF)
			|| peekToken(&tokens, TT_BOTHOF)
			|| peekToken(&tokens, TT_EITHEROF)
			|| peekToken(&tokens, TT_WONOF)
			|| peekToken(&tokens, TT_BOTHSAEM)
			|| peekToken(&tokens, TT_DIFFRINT)) {
		ExprNodeList *args = NULL;
		ExprNode *arg = NULL;
		OpExprNode *op = NULL;
		OpType type;
		if (acceptToken(&tokens, TT_SUMOF)) {
			type = OP_ADD;
#ifdef DEBUG
			debug("ET_OP (OP_ADD)");
#endif
		}
		else if(acceptToken(&tokens, TT_DIFFOF)) {
			type = OP_SUB;
#ifdef DEBUG
			debug("ET_OP (OP_SUB)");
#endif
		}
		else if(acceptToken(&tokens, TT_PRODUKTOF)) {
			type = OP_MULT;
#ifdef DEBUG
			debug("ET_OP (OP_MULT)");
#endif
		}
		else if(acceptToken(&tokens, TT_QUOSHUNTOF)) {
			type = OP_DIV;
#ifdef DEBUG
			debug("ET_OP (OP_DIV)");
#endif
		}
		else if(acceptToken(&tokens, TT_MODOF)) {
			type = OP_MOD;
#ifdef DEBUG
			debug("ET_OP (OP_MOD)");
#endif
		}
		else if(acceptToken(&tokens, TT_BIGGROF)) {
			type = OP_MAX;
#ifdef DEBUG
			debug("ET_OP (OP_MAX)");
#endif
		}
		else if(acceptToken(&tokens, TT_SMALLROF)) {
			type = OP_MIN;
#ifdef DEBUG
			debug("ET_OP (OP_MIN)");
#endif
		}
		else if(acceptToken(&tokens, TT_BOTHOF)) {
			type = OP_AND;
#ifdef DEBUG
			debug("ET_OP (OP_AND)");
#endif
		}
		else if(acceptToken(&tokens, TT_EITHEROF)) {
			type = OP_OR;
#ifdef DEBUG
			debug("ET_OP (OP_OR)");
#endif
		}
		else if(acceptToken(&tokens, TT_WONOF)) {
			type = OP_XOR;
#ifdef DEBUG
			debug("ET_OP (OP_XOR)");
#endif
		}
		else if(acceptToken(&tokens, TT_BOTHSAEM)) {
			type = OP_EQ;
#ifdef DEBUG
			debug("ET_OP (OP_EQ)");
#endif
		}
		else if(acceptToken(&tokens, TT_DIFFRINT)) {
			type = OP_NEQ;
#ifdef DEBUG
			debug("ET_OP (OP_NEQ)");
#endif
		}
		else {
			error("invalid binary operator", tokens);
			return NULL;
		}
		args = createExprNodeList();
		if (!args) return NULL;
		arg = parseExprNode(&tokens, functab);
		if (!arg) {
			deleteExprNodeList(args);
			return NULL;
		}
		if (!addExprNode(args, arg)) {
			deleteExprNode(arg);
			deleteExprNodeList(args);
			return NULL;
		}
		acceptToken(&tokens, TT_AN);
		arg = parseExprNode(&tokens, functab);
		if (!arg) {
			deleteExprNodeList(args);
			return NULL;
		}
		if (!addExprNode(args, arg)) {
			deleteExprNode(arg);
			deleteExprNodeList(args);
			return NULL;
		}
		op = createOpExprNode(type, args);
		if (!op) {
			deleteExprNodeList(args);
			return NULL;
		}
		ret = createExprNode(ET_OP, op);
		if (!ret) {
			deleteOpExprNode(op);
			return NULL;
		}
	}
	/* N-ary operations ending in MKAY */
	else if (peekToken(&tokens, TT_ALLOF)
			|| peekToken(&tokens, TT_ANYOF)) {
		ExprNodeList *args = NULL;
		OpExprNode *op = NULL;
		OpType type;
		if (acceptToken(&tokens, TT_ALLOF)) {
			type = OP_AND;
#ifdef DEBUG
			debug("ET_OP (OP_AND)");
#endif
		}
		else if(acceptToken(&tokens, TT_ANYOF)) {
			type = OP_OR;
#ifdef DEBUG
			debug("ET_OP (OP_OR)");
#endif
		}
		else {
			error("invalid n-ary operator", tokens);
			return NULL;
		}
		if (!(args = createExprNodeList())) return NULL;
		while (1) {
			ExprNode *arg = NULL;
			if (!(arg = parseExprNode(&tokens, functab))) {
				deleteExprNodeList(args);
				return NULL;
			}
			if (!addExprNode(args, arg)) {
				deleteExprNode(arg);
				deleteExprNodeList(args);
				return NULL;
			}
			if (acceptToken(&tokens, TT_MKAY)) break;
			acceptToken(&tokens, TT_AN);
		}
		if (!(op = createOpExprNode(type, args))) {
			deleteExprNodeList(args);
			return NULL;
		}
		if (!(ret = createExprNode(ET_OP, op))) {
			deleteOpExprNode(op);
			return NULL;
		}
	}
	/* N-ary operations not ending in MKAY */
	else if (peekToken(&tokens, TT_SMOOSH)) {
		ExprNodeList *args = NULL;
		OpExprNode *op = NULL;
		OpType type;
		acceptToken(&tokens, TT_SMOOSH);
		type = OP_CAT;
#ifdef DEBUG
		debug("ET_OP (OP_CAT)");
#endif
		if (!(args = createExprNodeList())) return NULL;
		while (1) {
			ExprNode *arg = NULL;
			if (!(arg = parseExprNode(&tokens, functab))) {
				deleteExprNodeList(args);
				return NULL;
			}
			if (!addExprNode(args, arg)) {
				deleteExprNode(arg);
				deleteExprNodeList(args);
				return NULL;
			}
			if (acceptToken(&tokens, TT_MKAY)
					|| peekToken(&tokens, TT_NEWLINE)) break;
			acceptToken(&tokens, TT_AN);
		}
		if (!(op = createOpExprNode(type, args))) {
			deleteExprNodeList(args);
			return NULL;
		}
		if (!(ret = createExprNode(ET_OP, op))) {
			deleteOpExprNode(op);
			return NULL;
		}
	}
	/* Logical NOT */
	else if (acceptToken(&tokens, TT_NOT)) {
		ExprNodeList *args = NULL;
		ExprNode *arg = NULL;
		OpExprNode *op = NULL;
#ifdef DEBUG
		debug("ET_OP (OP_NOT)");
#endif
		if (!(args = createExprNodeList())) return NULL;
		if (!(arg = parseExprNode(&tokens, functab))) {
			deleteExprNodeList(args);
			return NULL;
		}
		if (!addExprNode(args, arg)) {
			deleteExprNode(arg);
			deleteExprNodeList(args);
			return NULL;
		}
		if (!(op = createOpExprNode(OP_NOT, args))) {
			deleteExprNodeList(args);
			return NULL;
		}
		if (!(ret = createExprNode(ET_OP, op))) {
			deleteOpExprNode(op);
			return NULL;
		}
	}
	else error("expected expression", tokens);
#ifdef DEBUG
	shiftin();
#endif
	*tokenp = tokens;
	return ret;
}

/** Parses a set of tokens into a StmtNode structure.  Parsing begins at the
  * token pointed to by \a tokenp.
  *
  * \pre \a tokenp points to a position in an array of tokens created by
  *     tokenizeLexemes(Lexeme **).
  *
  * \post \a tokenp will point to the next \b unparsed token in the array.
  *
  * \return A pointer to the generated StmtNode structure.
  *
  * \retval NULL An error occurred during parsing.
  *
  * \see parseConstantNode(Token ***)
  * \see parseTypeNode(Token ***)
  * \see parseIdentifierNode(Token ***)
  * \see parseExprNode(Token ***, FunctionTable *)
  * \see parseBlockNode(Token ***, FunctionTable *)
  * \see parseMainNode(Token **, FunctionTable *) */
StmtNode *parseStmtNode(Token ***tokenp,        /**< [in,out] A pointer to the position of the next token to parse. */
                        FunctionTable *functab) /**< [in,out] A pointer to the table of defined functions. */
{
	Token **tokens = *tokenp;
	StmtNode *ret = NULL;
	ExprNode *expr = NULL;
#ifdef DEBUG
	shiftout();
#endif
	/* Casting */
	if (nextToken(&tokens, TT_ISNOWA)) {
		IdentifierNode *target = NULL;
		TypeNode *newtype = NULL;
		CastStmtNode *stmt = NULL;
#ifdef DEBUG
		debug("ST_CAST");
#endif
		if (!(target = parseIdentifierNode(&tokens))) return NULL;
		if (!acceptToken(&tokens, TT_ISNOWA)) {
			error("expected IS NOW A", tokens);
			deleteIdentifierNode(target);
			return NULL;
		}
		if (!(newtype = parseTypeNode(&tokens))) {
			deleteIdentifierNode(target);
			return NULL;
		}
		if (!acceptToken(&tokens, TT_NEWLINE)) {
			error("expected end of expression", tokens);
			deleteIdentifierNode(target);
			deleteTypeNode(newtype);
			return NULL;
		}
		if (!(stmt = createCastStmtNode(target, newtype))) {
			deleteIdentifierNode(target);
			deleteTypeNode(newtype);
			return NULL;
		}
		if (!(ret = createStmtNode(ST_CAST, stmt))) {
			deleteCastStmtNode(stmt);
			return NULL;
		}
	}
	/* Print */
	else if (acceptToken(&tokens, TT_VISIBLE)) {
		ExprNodeList *args = NULL;
		int nonl = 0;
		PrintStmtNode *stmt = NULL;
#ifdef DEBUG
		debug("ST_PRINT");
#endif
		if (!(args = createExprNodeList())) return NULL;
		do {
			ExprNode *arg = NULL;
			if (!(arg = parseExprNode(&tokens, functab))) {
				deleteExprNodeList(args);
				return NULL;
			}
			if (!addExprNode(args, arg)) {
				deleteExprNodeList(args);
				deleteExprNode(arg);
				return NULL;
			}
			acceptToken(&tokens, TT_AN);
		} while (!peekToken(&tokens, TT_NEWLINE) && !peekToken(&tokens, TT_BANG));
		if (acceptToken(&tokens, TT_BANG)) nonl = 1;
		if (!acceptToken(&tokens, TT_NEWLINE)) {
			error("expected end of expression", tokens);
			deleteExprNodeList(args);
			return NULL;
		}
		if (!(stmt = createPrintStmtNode(args, nonl))) {
			deleteExprNodeList(args);
			return NULL;
		}
		if (!(ret = createStmtNode(ST_PRINT, stmt))) {
			deletePrintStmtNode(stmt);
			return NULL;
		}
	}
	/* Input */
	else if (acceptToken(&tokens, TT_GIMMEH)) {
		IdentifierNode *target = NULL;
		InputStmtNode *stmt = NULL;
#ifdef DEBUG
		debug("ST_INPUT");
#endif
		target = parseIdentifierNode(&tokens);
		if (!target) return NULL;
		if (!acceptToken(&tokens, TT_NEWLINE)) {
			error("expected end of expression", tokens);
			deleteIdentifierNode(target);
			return NULL;
		}
		stmt = createInputStmtNode(target);
		if (!stmt) {
			deleteIdentifierNode(target);
			return NULL;
		}
		ret = createStmtNode(ST_INPUT, stmt);
		if (!ret) {
			deleteInputStmtNode(stmt);
			return NULL;
		}
	}
	/* Assignment */
	else if (nextToken(&tokens, TT_R)) {
		IdentifierNode *target = NULL;
		ExprNode *expr = NULL;
		AssignmentStmtNode *stmt = NULL;
#ifdef DEBUG
		debug("ST_ASSIGNMENT");
#endif
		target = parseIdentifierNode(&tokens);
		if (!target) return NULL;
		if (!acceptToken(&tokens, TT_R)) {
			error("expected R", tokens);
			deleteIdentifierNode(target);
			return NULL;
		}
		expr = parseExprNode(&tokens, functab);
		if (!expr) {
			deleteIdentifierNode(target);
			return NULL;
		}
		if (!acceptToken(&tokens, TT_NEWLINE)) {
			error("expected end of statement", tokens);
			deleteIdentifierNode(target);
			deleteExprNode(expr);
			return NULL;
		}
		stmt = createAssignmentStmtNode(target, expr);
		if (!stmt) {
			deleteIdentifierNode(target);
			deleteExprNode(expr);
			return NULL;
		}
		ret = createStmtNode(ST_ASSIGNMENT, stmt);
		if (!ret) {
			deleteAssignmentStmtNode(stmt);
			return NULL;
		}
	}
	/* Variable declaration */
	else if (nextToken(&tokens, TT_HASA)) {
		IdentifierNode *scope = NULL;
		IdentifierNode *target = NULL;
		ExprNode *expr = NULL;
		DeclarationStmtNode *stmt = NULL;
#ifdef DEBUG
		debug("ST_DECLARATION");
#endif
		scope = parseIdentifierNode(&tokens);
		if (!scope) return NULL;
		if (!acceptToken(&tokens, TT_HASA)) {
			error("expected HAS A", tokens);
			deleteIdentifierNode(scope);
			return NULL;
		}
		target = parseIdentifierNode(&tokens);
		if (!target) {
			deleteIdentifierNode(scope);
			return NULL;
		}
		if (acceptToken(&tokens, TT_ITZ)) {
			expr = parseExprNode(&tokens, functab);
			if (!expr) {
				deleteIdentifierNode(scope);
				deleteIdentifierNode(target);
				return NULL;
			}
		}
		if (!acceptToken(&tokens, TT_NEWLINE)) {
			error("expected end of statement", tokens);
			deleteIdentifierNode(scope);
			deleteIdentifierNode(target);
			if (expr) deleteExprNode(expr);
			return NULL;
		}
		stmt = createDeclarationStmtNode(scope, target, expr);
		if (!stmt) {
			deleteIdentifierNode(scope);
			deleteIdentifierNode(target);
			if (expr) deleteExprNode(expr);
			return NULL;
		}
		ret = createStmtNode(ST_DECLARATION, stmt); 
		if (!ret) {
			deleteDeclarationStmtNode(stmt);
			return NULL;
		}
	}
	/* If/then/else */
	else if (acceptToken(&tokens, TT_ORLY)) {
		BlockNode *yes = NULL;
		ExprNodeList *guards = NULL;
		BlockNodeList *blocks = NULL;
		BlockNode *no = NULL;
		IfThenElseStmtNode *stmt = NULL;
#ifdef DEBUG
		debug("ST_CONDITIONA");
#endif
		if (!acceptToken(&tokens, TT_NEWLINE)) {
			error("expected end of expression", tokens);
			return NULL;
		}
		if (!acceptToken(&tokens, TT_YARLY)) {
			error("expected YA RLY", tokens);
			return NULL;
		}
		if (!acceptToken(&tokens, TT_NEWLINE)) {
			error("expected end of expression", tokens);
			return NULL;
		}
		yes = parseBlockNode(&tokens, functab);
		if (!yes) return NULL;
		guards = createExprNodeList();
		if (!guards) {
			deleteBlockNode(yes);
			return NULL;
		}
		blocks = createBlockNodeList();
		if (!blocks) {
			deleteBlockNode(yes);
			deleteExprNodeList(guards);
			return NULL;
		}
		while (acceptToken(&tokens, TT_MEBBE)) {
			ExprNode *guard = NULL;
			BlockNode *block = NULL;
			guard = parseExprNode(&tokens, functab);
			if (!guard) {
				deleteBlockNode(yes);
				deleteExprNodeList(guards);
				deleteBlockNodeList(blocks);
				return NULL;
			}
			if (!addExprNode(guards, guard)) {
				deleteBlockNode(yes);
				deleteExprNodeList(guards);
				deleteBlockNodeList(blocks);
				deleteExprNode(guard);
				return NULL;
			}
			if (!acceptToken(&tokens, TT_NEWLINE)) {
				error("expected end of expression", tokens);
				deleteBlockNode(yes);
				deleteExprNodeList(guards);
				deleteBlockNodeList(blocks);
				return NULL;
			}
			block = parseBlockNode(&tokens, functab);
			if (!block) {
				deleteBlockNode(yes);
				deleteExprNodeList(guards);
				deleteBlockNodeList(blocks);
				return NULL;
			}
			if (!addBlockNode(blocks, block)) {
				deleteBlockNode(yes);
				deleteExprNodeList(guards);
				deleteBlockNodeList(blocks);
				deleteBlockNode(block);
				return NULL;
			}
		}
		if (acceptToken(&tokens, TT_NOWAI)) {
			if (!acceptToken(&tokens, TT_NEWLINE)) {
				error("expected end of expression", tokens);
				deleteBlockNode(yes);
				deleteExprNodeList(guards);
				deleteBlockNodeList(blocks);
				return NULL;
			}
			no = parseBlockNode(&tokens, functab);
			if (!no) {
				deleteBlockNode(yes);
				deleteExprNodeList(guards);
				deleteBlockNodeList(blocks);
				return NULL;
			}
		}
		if (!acceptToken(&tokens, TT_OIC)) {
			error("expected OIC", tokens);
			deleteBlockNode(yes);
			deleteExprNodeList(guards);
			deleteBlockNodeList(blocks);
			if (no) deleteBlockNode(no);
			return NULL;
		}
		if (!acceptToken(&tokens, TT_NEWLINE)) {
			error("expected end of expression", tokens);
			deleteBlockNode(yes);
			deleteExprNodeList(guards);
			deleteBlockNodeList(blocks);
			if (no) deleteBlockNode(no);
			return NULL;
		}
		stmt = createIfThenElseStmtNode(yes, no, guards, blocks);
		if (!stmt) {
			deleteBlockNode(yes);
			deleteExprNodeList(guards);
			deleteBlockNodeList(blocks);
			if (no) deleteBlockNode(no);
			return NULL;
		}
		ret = createStmtNode(ST_IFTHENELSE, stmt);
		if (!ret) {
			deleteIfThenElseStmtNode(stmt);
			return NULL;
		}
	}
	/* Switch */
	else if (acceptToken(&tokens, TT_WTF)) {
		ExprNodeList *guards = NULL;
		BlockNodeList *blocks = NULL;
		BlockNode *def = NULL;
		SwitchStmtNode *stmt = NULL;
#ifdef DEBUG
		debug("ST_SWITCH");
#endif
		if (!acceptToken(&tokens, TT_NEWLINE)) {
			error("expected end of expression", tokens);
			return NULL;
		}
		guards = createExprNodeList();
		if (!guards) return NULL;
		blocks = createBlockNodeList();
		if (!blocks) {
			deleteExprNodeList(guards);
			return NULL;
		}
		do {
			ConstantNode *c = NULL;
			ExprNode *guard = NULL;
			BlockNode *block = NULL;
			unsigned int n = 0;
			if (!acceptToken(&tokens, TT_OMG)) {
				error("expected OMG", tokens);
				deleteExprNodeList(guards);
				deleteBlockNodeList(blocks);
				return NULL;
			}
			/** \note The 1.2 specification only allows constant
			  *       values for OMG guards thus this function
			  *       explicitly checks for them. */
			c = parseConstantNode(&tokens);
			if (!c) {
				deleteExprNodeList(guards);
				deleteBlockNodeList(blocks);
				return NULL;
			}
			/* Check for string interpolation */
			if (c->type == CT_STRING && strstr(c->data.s, ":{")) {
				error("cannot use an interpolated string as an OMG literal", tokens);
				return NULL;
			}
			/* Check if constant is unique */
			for (n = 0; n < guards->num; n++) {
				ConstantNode *test = guards->exprs[n]->expr;
				if (c->type == test->type) {
					if (((c->type == CT_BOOLEAN || c->type == CT_INTEGER)
							&& c->data.i == test->data.i)
							|| (c->type == CT_FLOAT
							&& c->data.f == test->data.f)
							|| (c->type == CT_STRING
							&& !strcmp(c->data.s, test->data.s))) {
								deleteExprNodeList(guards);
								deleteBlockNodeList(blocks);
								deleteConstantNode(c);
								error("duplicate OMG literal", tokens);
								return NULL;
					}
				}
			}
			guard = createExprNode(ET_CONSTANT, c);
			if (!guard) {
				deleteExprNodeList(guards);
				deleteBlockNodeList(blocks);
				deleteConstantNode(c);
				return NULL;
			}
			if (!addExprNode(guards, guard)) {
				deleteExprNodeList(guards);
				deleteBlockNodeList(blocks);
				deleteExprNode(guard);
				return NULL;
			}
			if (!acceptToken(&tokens, TT_NEWLINE)) {
				error("expected end of expression", tokens);
				deleteExprNodeList(guards);
				deleteBlockNodeList(blocks);
				return NULL;
			}
			block = parseBlockNode(&tokens, functab);
			if (!block) {
				deleteExprNodeList(guards);
				deleteBlockNodeList(blocks);
			}
			if (!addBlockNode(blocks, block)) {
				deleteExprNodeList(guards);
				deleteBlockNodeList(blocks);
				deleteBlockNode(block);
				return NULL;
			}
		} while (peekToken(&tokens, TT_OMG));
		if (acceptToken(&tokens, TT_OMGWTF)) {
			if (!acceptToken(&tokens, TT_NEWLINE)) {
				error("expected end of expression", tokens);
				deleteExprNodeList(guards);
				deleteBlockNodeList(blocks);
				return NULL;
			}
			def = parseBlockNode(&tokens, functab);
			if (!def) {
				deleteExprNodeList(guards);
				deleteBlockNodeList(blocks);
				return NULL;
			}
		}
		if (!acceptToken(&tokens, TT_OIC)) {
			error("expected OIC", tokens);
			deleteExprNodeList(guards);
			deleteBlockNodeList(blocks);
			if (def) deleteBlockNode(def);
			return NULL;
		}
		if (!acceptToken(&tokens, TT_NEWLINE)) {
			error("expected end of expression", tokens);
			deleteExprNodeList(guards);
			deleteBlockNodeList(blocks);
			if (def) deleteBlockNode(def);
			return NULL;
		}
		stmt = createSwitchStmtNode(guards, blocks, def);
		if (!stmt) {
			deleteExprNodeList(guards);
			deleteBlockNodeList(blocks);
			if (def) deleteBlockNode(def);
			return NULL;
		}
		ret = createStmtNode(ST_SWITCH, stmt);
		if (!ret) {
			deleteSwitchStmtNode(stmt);
		}
	}
	/* Break */
	else if (acceptToken(&tokens, TT_GTFO)) {
#ifdef DEBUG
		debug("ST_BREAK");
#endif
		if (!acceptToken(&tokens, TT_NEWLINE)) {
			error("expected end of expression", tokens);
			return NULL;
		}
		ret = createStmtNode(ST_BREAK, NULL);
		if (!ret) return NULL;
	}
	/* Return */
	else if (acceptToken(&tokens, TT_FOUNDYR)) {
		ExprNode *value = NULL;
		ReturnStmtNode *stmt = NULL;
#ifdef DEBUG
		debug("ST_RETURN");
#endif
		value = parseExprNode(&tokens, functab);
		if (!value) return NULL;
		if (!acceptToken(&tokens, TT_NEWLINE)) {
			error("expected end of expression", tokens);
			deleteExprNode(value);
			return NULL;
		}
		stmt = createReturnStmtNode(value);
		if (!stmt) {
			deleteExprNode(value);
			return NULL;
		}
		ret = createStmtNode(ST_RETURN, stmt);
		if (!ret) {
			deleteReturnStmtNode(stmt);
			return NULL;
		}
	}
	/* Loop */
	else if (acceptToken(&tokens, TT_IMINYR)) {
		IdentifierNode *name1 = NULL;
		IdentifierNode *var = NULL;
		ExprNode *update = NULL;
		ExprNode *guard = NULL;
		BlockNode *body = NULL;
		FuncDefStmtNode *def = NULL;
		IdentifierNode *name2 = NULL;
		LoopStmtNode *stmt = NULL;
#ifdef DEBUG
		debug("ST_LOOP");
#endif
		name1 = parseIdentifierNode(&tokens);
		if (!name1) return NULL;
		if (peekToken(&tokens, TT_UPPIN) || peekToken(&tokens, TT_NERFIN)) {
			OpType type;
			IdentifierNode *varcopy = NULL;
			ExprNode *arg1 = NULL;
			ConstantNode *one = NULL;
			ExprNode *arg2 = NULL;
			ExprNodeList *args = NULL;
			OpExprNode *op = NULL;
			if (acceptToken(&tokens, TT_UPPIN)) {
				type = OP_ADD;
#ifdef DEBUG
			shiftout();
			debug("ET_OP (OP_ADD)");
#endif
			}
			else if (acceptToken(&tokens, TT_NERFIN)) {
				type = OP_SUB;
#ifdef DEBUG
			shiftout();
			debug("ET_OP (OP_SUB)");
#endif
			}
			else {
				error("expected UPPIN or NERFIN", tokens);
				return NULL;
			}
			if (!acceptToken(&tokens, TT_YR)) {
				error("expected YR", tokens);
				deleteIdentifierNode(name1);
				return NULL;
			}
			var = parseIdentifierNode(&tokens);
			if (!var) {
				deleteIdentifierNode(name1);
				return NULL;
			}
#ifdef DEBUG
			shiftout();
			debug("ET_CONSTANT (CT_INTEGER)");
			shiftin();
			shiftin();
#endif
			varcopy = createIdentifierNode(var->image, var->fname, var->line);
			if (!varcopy) {
				deleteIdentifierNode(name1);
				deleteIdentifierNode(var);
				return NULL;
			}
			arg1 = createExprNode(ET_IDENTIFIER, varcopy);
			if (!arg1) {
				deleteIdentifierNode(name1);
				deleteIdentifierNode(var);
				deleteIdentifierNode(varcopy);
				return NULL;
			}
			one = createIntegerConstantNode(1);
			if (!one) {
				deleteIdentifierNode(name1);
				deleteIdentifierNode(var);
				deleteExprNode(arg1);
				return NULL;
			}
			arg2 = createExprNode(ET_CONSTANT, one);
			if (!arg2) {
				deleteIdentifierNode(name1);
				deleteIdentifierNode(var);
				deleteExprNode(arg1);
				deleteConstantNode(one);
				return NULL;
			}
			args = createExprNodeList();
			if (!args) {
				deleteIdentifierNode(name1);
				deleteIdentifierNode(var);
				deleteExprNode(arg1);
				deleteExprNode(arg2);
				return NULL;
			}
			if (!addExprNode(args, arg1)) {
				deleteIdentifierNode(name1);
				deleteIdentifierNode(var);
				deleteExprNode(arg1);
				deleteExprNode(arg2);
				deleteExprNodeList(args);
				return NULL;
			}
			if (!addExprNode(args, arg2)) {
				deleteIdentifierNode(name1);
				deleteIdentifierNode(var);
				deleteExprNode(arg2);
				deleteExprNodeList(args);
				return NULL;
			}
			op = createOpExprNode(type, args);
			if (!op) {
				deleteIdentifierNode(name1);
				deleteIdentifierNode(var);
				deleteExprNodeList(args);
				return NULL;
			}
			update = createExprNode(ET_OP, op);
			if (!update) {
				deleteIdentifierNode(name1);
				deleteIdentifierNode(var);
				deleteOpExprNode(op);
				return NULL;
			}
		}
		else if ((def = lookupFuncDefStmtNode(functab, (*tokens)->image))) {
			tokens++;
			if (!acceptToken(&tokens, TT_YR)) {
				error("expected YR", tokens);
				deleteIdentifierNode(name1);
				return NULL;
			}
			/* Check for unary function */
			if (def->args->num == 1) {
				IdentifierNode *varcopy = NULL;
				ExprNode *arg = NULL;
				ExprNodeList *args = NULL;
				FuncCallExprNode *func = NULL;
#ifdef DEBUG
				shiftout();
				debug("ET_FUNCCALL");
#endif
				var = parseIdentifierNode(&tokens);
				if (!var) {
					deleteIdentifierNode(name1);
					return NULL;
				}
#ifdef DEBUG
				shiftin();
#endif
				varcopy = createIdentifierNode(var->image, var->fname, var->line);
				if (!varcopy) {
					deleteIdentifierNode(name1);
					deleteIdentifierNode(var);
					return NULL;
				}
				arg = createExprNode(ET_IDENTIFIER, varcopy);
				if (!arg) {
					deleteIdentifierNode(name1);
					deleteIdentifierNode(var);
					deleteIdentifierNode(varcopy);
					return NULL;
				}
				args = createExprNodeList();
				if (!args) {
					deleteIdentifierNode(name1);
					deleteIdentifierNode(var);
					deleteExprNode(arg);
					return NULL;
				}
				if (!addExprNode(args, arg)) {
					deleteIdentifierNode(name1);
					deleteIdentifierNode(var);
					deleteExprNode(arg);
					deleteExprNodeList(args);
					return NULL;
				}
				func = createFuncCallExprNode(def, args);
				if (!func) {
					deleteIdentifierNode(name1);
					deleteIdentifierNode(var);
					deleteExprNodeList(args);
					return NULL;
				}
				update = createExprNode(ET_FUNCCALL, func);
				if (!update) {
					deleteFuncCallExprNode(func);
					return NULL;
				}
			}
			else {
				error("expected unary function", tokens);
				deleteIdentifierNode(name1);
				return NULL;
			}
		}
		if (update) {
			if (acceptToken(&tokens, TT_WILE)) {
				guard = parseExprNode(&tokens, functab);
				if (!guard) {
					deleteIdentifierNode(name1);
					deleteExprNode(update);
					return NULL;
				}
			}
			else if (acceptToken(&tokens, TT_TIL)) {
				ExprNode *arg = NULL;
				ExprNodeList *args = NULL;
				OpExprNode *op = NULL;
#ifdef DEBUG
				shiftout();
				debug("ET_OP (OP_NOT)");
#endif
				arg = parseExprNode(&tokens, functab);
				if (!arg) {
					deleteIdentifierNode(name1);
					deleteExprNode(update);
					return NULL;
				}
				args = createExprNodeList();
				if (!args) {
					deleteIdentifierNode(name1);
					deleteExprNode(update);
					deleteExprNode(arg);
					return NULL;
				}
				if(!addExprNode(args, arg)) {
					deleteIdentifierNode(name1);
					deleteExprNode(update);
					deleteExprNode(arg);
					deleteExprNodeList(args);
					return NULL;
				}
#ifdef DEBUG
				shiftin();
#endif
				op = createOpExprNode(OP_NOT, args);
				if (!op) {
					deleteIdentifierNode(name1);
					deleteExprNode(update);
					deleteExprNodeList(args);
					return NULL;
				}
				guard = createExprNode(ET_OP, op);
				if (!guard) {
					deleteIdentifierNode(name1);
					deleteExprNode(update);
					deleteOpExprNode(op);
					return NULL;
				}
			}
		}
		if (!acceptToken(&tokens, TT_NEWLINE)) {
			error("expected end of expression", tokens);
			deleteIdentifierNode(name1);
			if (update) deleteExprNode(update);
			if (guard) deleteExprNode(guard);
			return NULL;
		}
		body = parseBlockNode(&tokens, functab);
		if (!body) {
			deleteIdentifierNode(name1);
			if (update) deleteExprNode(update);
			if (guard) deleteExprNode(guard);
			return NULL;
		}
		if (!acceptToken(&tokens, TT_IMOUTTAYR)) {
			error("expected IM OUTTA YR", tokens);
			deleteIdentifierNode(name1);
			if (update) deleteExprNode(update);
			if (guard) deleteExprNode(guard);
			deleteBlockNode(body);
			return NULL;
		}
		name2 = parseIdentifierNode(&tokens);
		if (!name2) {
			deleteIdentifierNode(name1);
			if (update) deleteExprNode(update);
			if (guard) deleteExprNode(guard);
			deleteBlockNode(body);
			return NULL;
		}
		if (strcmp(name1->image, name2->image)) {
			error("expected matching loop name", tokens);
			deleteIdentifierNode(name1);
			if (update) deleteExprNode(update);
			if (guard) deleteExprNode(guard);
			deleteBlockNode(body);
			deleteIdentifierNode(name2);
			return NULL;
		}
		deleteIdentifierNode(name2);
		if (!acceptToken(&tokens, TT_NEWLINE)) {
			error("expected end of expression", tokens);
			deleteIdentifierNode(name1);
			if (update) deleteExprNode(update);
			if (guard) deleteExprNode(guard);
			deleteBlockNode(body);
			return NULL;
		}
		stmt = createLoopStmtNode(name1, var, guard, update, body);
		if (!stmt) {
			deleteIdentifierNode(name1);
			if (update) deleteExprNode(update);
			if (guard) deleteExprNode(guard);
			deleteBlockNode(body);
			return NULL;
		}
		ret = createStmtNode(ST_LOOP, stmt);
		if (!ret) {
			deleteLoopStmtNode(stmt);
			return NULL;
		}
	}
	/* Function definition */
	else if (acceptToken(&tokens, TT_HOWDUZ)) {
		IdentifierNode *scope = NULL;
		IdentifierNode *name = NULL;
		BlockNode *body;
		FuncDefStmtNode *proto = NULL;
#ifdef DEBUG
		debug("ST_FUNCDEF");
#endif
		/* Skip over the scope as it has already been parsed */
		scope = parseIdentifierNode(&tokens);
		if (!scope) return NULL;
		deleteIdentifierNode(scope);
		name = parseIdentifierNode(&tokens);
		if (!name) return NULL;
		/* Lookup function prototype to update it after parsing body */
		proto = lookupFuncDefStmtNode(functab, name->image);
		deleteIdentifierNode(name);
		if (!proto) {
			error("function prototype not found", tokens);
			return NULL;
		}
		/* Skip over the arguments as they have already been parsed */
		if (acceptToken(&tokens, TT_YR)) {
			IdentifierNode *arg = parseIdentifierNode(&tokens);
			if (!arg) return NULL;
			deleteIdentifierNode(arg);
			while (acceptToken(&tokens, TT_ANYR)) {
				arg = parseIdentifierNode(&tokens);
				if (!arg) return NULL;
				deleteIdentifierNode(arg);
			}
		}
		if (!acceptToken(&tokens, TT_NEWLINE)) {
			error("expected end of statement", tokens);
			return NULL;
		}
		body = parseBlockNode(&tokens, functab);
		if (!body) return NULL;
		if (!acceptToken(&tokens, TT_IFUSAYSO)) {
			error("expected IF YOU SAY SO", tokens);
			deleteBlockNode(body);
			return NULL;
		}
		if (!acceptToken(&tokens, TT_NEWLINE)) {
			error("expected end of statement", tokens);
			deleteBlockNode(body);
			return NULL;
		}
		/* Update the function body */
		proto->body = body;
		ret = createStmtNode(ST_FUNCDEF, proto);
		if (!ret) {
			deleteStmtNode(ret);
		}
	}
	/* Expression Evaluation */
	else if ((expr = parseExprNode(&tokens, functab))) {
#ifdef DEBUG
		debug("ST_EXPR");
#endif
		if (!acceptToken(&tokens, TT_NEWLINE)) {
			deleteExprNode(expr);
			error("expected end of expression", tokens);
			return NULL;
		}
		ret = createStmtNode(ST_EXPR, expr);
	}
	else error("expected statement", tokens);
#ifdef DEBUG
	shiftin();
#endif
	*tokenp = tokens;
	return ret;
}

/** Parses a set of tokens into a BlockNode structure.  Parsing begins at the
  * token pointed to by \a tokenp.
  *
  * \pre \a tokenp points to a position in an array of tokens created by
  *     tokenizeLexemes(Lexeme **).
  *
  * \post \a tokenp will point to the next \b unparsed token in the array.
  *
  * \return A pointer to the generated BlockNode structure.
  *
  * \retval NULL An error occurred during parsing.
  *
  * \see parseConstantNode(Token ***)
  * \see parseTypeNode(Token ***)
  * \see parseIdentifierNode(Token ***)
  * \see parseExprNode(Token ***, FunctionTable *)
  * \see parseStmtNode(Token ***, FunctionTable *)
  * \see parseMainNode(Token **, FunctionTable *) */
BlockNode *parseBlockNode(Token ***tokenp,        /**< [in,out] A pointer to the position of the next token to parse. */
                          FunctionTable *functab) /**< [in,out] A pointer to the table of defined functions. */
{
	Token **tokens = *tokenp;
	StmtNodeList *stmts = NULL;
	BlockNode *block = NULL;
#ifdef DEBUG
	shiftout();
	debug(">ET_BLOCK");
#endif
	stmts = createStmtNodeList();
	if (!stmts) return NULL;
	while (!peekToken(&tokens, TT_EOF)
			&& !peekToken(&tokens, TT_KTHXBYE) && !peekToken(&tokens, TT_OIC)
			&& !peekToken(&tokens, TT_YARLY) && !peekToken(&tokens, TT_NOWAI)
			&& !peekToken(&tokens, TT_MEBBE) && !peekToken(&tokens, TT_OMG)
			&& !peekToken(&tokens, TT_OMGWTF) && !peekToken(&tokens, TT_IMOUTTAYR)
			&& !peekToken(&tokens, TT_IFUSAYSO)) {
		StmtNode *s = parseStmtNode(&tokens, functab);
		if (!s) {
			deleteStmtNodeList(stmts);
			return NULL;
		}
		addStmtNode(stmts, s);
	}
#ifdef DEBUG
	debug("<ET_BLOCK");
	shiftin();
#endif
	*tokenp = tokens;
	block = createBlockNode(stmts);
	if (!block) {
		deleteStmtNodeList(stmts);
		return NULL;
	}
	return block;
}

/** Parses \a tokens into a MainNode structure, an intermediary form to be
  * passed along to a later stage of processing such as, for example, an
  * interpreter.
  *
  * \pre \a tokens was created by tokenizeLexemes(Lexeme **).
  * \pre \a functab was created by setupFunctionTable(Token **).
  *
  * \return A pointer to the generated MainNode structure.
  *
  * \retval NULL An error occurred during parsing.
  *
  * \see parseConstantNode(Token ***)
  * \see parseTypeNode(Token ***)
  * \see parseIdentifierNode(Token ***)
  * \see parseExprNode(Token ***, FunctionTable *)
  * \see parseStmtNode(Token ***, FunctionTable *)
  * \see parseBlockNode(Token ***, FunctionTable *) */
MainNode *parseMainNode(Token **tokens,         /**< [in] A pointer to an array of tokens to parse. */
                        FunctionTable *functab) /**< [in,out] A pointer to a table of function prototypes. */
{
	if (acceptToken(&tokens, TT_HAI)) {
		BlockNode *block = NULL;
		MainNode *_main = NULL; /* main is reserved for the main function */
#ifdef DEBUG
		fprintf(stderr, "ET_MAINBLOCK\n");
#endif
		tokens++; /* Accept any version */
		if (!acceptToken(&tokens, TT_NEWLINE)) {
			error("expected end of statement", tokens);
			return NULL;
		}
		block = parseBlockNode(&tokens, functab);
		if (!block) return NULL;
		if (!acceptToken(&tokens, TT_KTHXBYE)) {
			error("expected KTHXBYE", tokens);
			deleteBlockNode(block);
			return NULL;
		}
		_main = createMainNode(block, functab);
		if (!_main) {
			deleteBlockNode(block);
			return NULL;
		}
		return _main;
	}
	else {
		error("expected HAI", tokens);
		return NULL;
	}
}

/** Sets up a FunctionTable structure filled with the prototypes of all declared
  * functions.  This structure is then passed as an argument to parseMainNode(Token **, FunctionTable *)
  * so that functions may be called before their definition as well as recursively.
  *
  * \pre \a tokens was created by tokenizeLexemes(Lexeme **).
  *
  * \return A pointer to the generated FunctionTable structure.
  *
  * \retval NULL An error occurred during setup. */
FunctionTable *setupFunctionTable(Token **tokens) /**< [in] A pointer to an array of tokens to scan for function prototypes. */
{
	FunctionTable *p = createFunctionTable();
#ifdef DEBUG
	fprintf(stderr, "Setting up function table...\n");
#endif
	while (*tokens) {
		if (acceptToken(&tokens, TT_HOWDUZ)) {
			IdentifierNode *scope = NULL;
			IdentifierNode *name = NULL;
			IdentifierNodeList *args = NULL;
			FuncDefStmtNode *stmt = NULL;
			scope = parseIdentifierNode(&tokens);
			if (!scope) {
				deleteFunctionTable(p);
				return NULL;
			}
			name = parseIdentifierNode(&tokens);
			if (!name) {
				deleteFunctionTable(p);
				deleteIdentifierNode(scope);
				return NULL;
			}
			args = createIdentifierNodeList();
			if (!args) {
				deleteFunctionTable(p);
				deleteIdentifierNode(scope);
			}
			if (acceptToken(&tokens, TT_YR)) {
				IdentifierNode *arg = parseIdentifierNode(&tokens);
				if (!arg) {
					deleteFunctionTable(p);
					deleteIdentifierNode(scope);
					deleteIdentifierNodeList(args);
					return NULL;
				}
				if (!addIdentifierNode(args, arg)) {
					deleteFunctionTable(p);
					deleteIdentifierNode(scope);
					deleteIdentifierNodeList(args);
					deleteIdentifierNode(arg);
					return NULL;
				}
				while (acceptToken(&tokens, TT_ANYR)) {
					arg = parseIdentifierNode(&tokens);
					if (!arg) {
						deleteFunctionTable(p);
						deleteIdentifierNode(scope);
						deleteIdentifierNodeList(args);
						return NULL;
					}
					if (!addIdentifierNode(args, arg)) {
						deleteFunctionTable(p);
						deleteIdentifierNode(scope);
						deleteIdentifierNodeList(args);
						deleteIdentifierNode(arg);
						return NULL;
					}
				}
			}
			if (!acceptToken(&tokens, TT_NEWLINE)) {
				error("expected end of statement", tokens);
				deleteFunctionTable(p);
				deleteIdentifierNode(scope);
				deleteIdentifierNodeList(args);
				return NULL;
			}
			stmt = createFuncDefStmtNode(scope, name, args, NULL);
			if (!stmt) {
				deleteFunctionTable(p);
				deleteIdentifierNode(scope);
				deleteIdentifierNodeList(args);
				return NULL;
			}
			if (!addFuncDefStmtNode(p, stmt)) {
				deleteFunctionTable(p);
				deleteFuncDefStmtNode(stmt);
				return NULL;
			}
		}
		else tokens++;
	}
#ifdef DEBUG
	fprintf(stderr, "Done setting up function table...\n");
#endif
	return p;
}
