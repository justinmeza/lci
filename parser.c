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
  *
  * \return A pointer to a MainNode structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteMainNode(MainNode *) */
MainNode *createMainNode(BlockNode *block) /**< [in] A pointer to the block of code to execute first. */
{
	MainNode *p = malloc(sizeof(MainNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->block = block;
	return p;
}

/** Deletes a MainNode structure.
  *
  * \pre \a node was created by createMainNode(BlockNode *).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createMainNode(BlockNode *) */
void deleteMainNode(MainNode *node) /**< [in,out] A pointer to the MainNode structure to be deleted. */
{
	if (!node) return;
	deleteBlockNode(node->block);
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
  * \retval 0 realloc was unable to allocate memory.
  * \retval 1 \a node was added to \a list. */
int addBlockNode(BlockNodeList *list,  /**< [in,out] A pointer to the BlockNodeList structure to add \a node to. */
                 BlockNode *node)     /**< [in] A pointer to the BlockNode structure to add to \a list. */
{
	unsigned int newsize = list->num + 1;
	void *mem = realloc(list->blocks, sizeof(BlockNode *) * newsize);
	if (!mem) {
		perror("realloc");
		return 0;
	}
	list->blocks = mem;
	list->blocks[list->num] = node;
	list->num = newsize;
	return 1;
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
  * \retval 0 realloc was unable to allocate memory.
  * \retval 1 \a node was added to \a list. */
int addIdentifierNode(IdentifierNodeList *list,   /**< [in,out] A pointer to the IdentifierNodeList structure to add \a node to. */
                      IdentifierNode *node) /**< [in] A pointer to the IdentifierNode structure to add to \a list. */
{
	unsigned int newsize = list->num + 1;
	void *mem = realloc(list->ids, sizeof(IdentifierNode *) * newsize);
	if (!mem) {
		perror("realloc");
		return 0;
	}
	list->ids = mem;
	list->ids[list->num] = node;
	list->num = newsize;
	return 1;
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
		case ST_DEALLOCATION: {
			DeallocationStmtNode *stmt = (DeallocationStmtNode *)node->stmt;
			deleteDeallocationStmtNode(stmt);
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
  * \retval 0 malloc was unable to allocate memory.
  * \retval 1 \a node was added to \a list. */
int addStmtNode(StmtNodeList *list, /**< [in,out] A pointer to the StmtNodeList structure to add \a node to. */
                StmtNode *node)     /**< [in] A pointer to the StmtNode structure to add to \a list. */
{
	unsigned int newsize = list->num + 1;
	void *mem = realloc(list->stmts, sizeof(StmtNode *) * newsize);
	if (!mem) {
		perror("realloc");
		return 0;
	}
	list->stmts = mem;
	list->stmts[list->num] = node;
	list->num = newsize;
	return 1;
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
                                               ExprNode *expr,         /**< [in] An optional pointer to the expression to initialize \a target to. */
                                               TypeNode *type)         /**< [in] An optional pointer to the type to initialize \a target to. */
{
	DeclarationStmtNode *p = malloc(sizeof(DeclarationStmtNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->scope = scope;
	p->target = target;
	p->expr = expr;
	p->type = type;
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
	deleteTypeNode(node->type);
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

/** Creates a DeallocationStmtNode structure.
  *
  * \pre \a target was created by createIdentifierNode(char *, const char *, unsigned int).
  *
  * \return A pointer to a DeallocationStmtNode structure with the desired
  *         properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteDeallocationStmtNode(DeallocationStmtNode *) */
DeallocationStmtNode *createDeallocationStmtNode(IdentifierNode *target) /**< [in] A pointer to the name of the variable. */
{
	DeallocationStmtNode *p = malloc(sizeof(DeallocationStmtNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->target = target;
	return p;
}

/** Deletes a DeallocationStmtNode structure.
  *
  * \pre \a node was created by createDeallocationStmtNode(IdentifierNode *).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createDeallocationStmtNode(IdentifierNode *) */
void deleteDeallocationStmtNode(DeallocationStmtNode *node) /**< [in,out] A pointer to the DeallocationStmtNode structure to be deleted. */
{
	if (!node) return;
	deleteIdentifierNode(node->target);
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
  *        createFloatConstantNode(float), or createStringConstantNode(char *)
  *      - ET_IDENTIFIER: createIdentifierNode(char *, const char *, unsigned int)
  *      - ET_FUNCCALL: createFuncCallExprNode(IdentifierNode *, IdentifierNode *, ExprNodeList *)
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
  * \retval 0 realloc was unable to allocate memory.
  * \retval 1 \a node was added to \a list. */
int addExprNode(ExprNodeList *list, /**< [in,out] A pointer to the ExprNodeList structure to add \a node to. */
                ExprNode *node)     /**< [in] A pointer to the ExprNode structure to add to \a list. */
{
	unsigned int newsize = list->num + 1;
	void *mem = realloc(list->exprs, sizeof(ExprNode *) * newsize);
	if (!mem) {
		perror("realloc");
		return 0;
	}
	list->exprs = mem;
	list->exprs[list->num] = node;
	list->num = newsize;
	return 1;
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
  * \pre \a scope was created by createIdentifierNode(char *, const char *, unsigned int).
  * \pre \a name was created by createIdentifierNode(char *, const char *, unsigned int).
  * \pre \a args was created by createIdentifierNodeList(void) and contains
  *      items added by addIdentifierNode(IdentifierNodeList *, IdentifierNode *).
  *
  * \return A pointer to a FuncCallExprNode structure with the desired
  *         properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteFuncCallExprNode(FuncCallExprNode *) */
FuncCallExprNode *createFuncCallExprNode(IdentifierNode *scope, /**< [in] A pointer to the scope the function is defined in. */
                                         IdentifierNode *name,  /**< [in] A pointer to the name of the function. */
                                         ExprNodeList *args)    /**< [in] A pointer to a list of ExprNode structure arguments supplied to the function definition. */
{
	FuncCallExprNode *p = malloc(sizeof(FuncCallExprNode));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->scope = scope;
	p->name = name;
	p->args = args;
	return p;
}

/** Deletes a FuncCallExprNode structure.
  *
  * \pre \a node was created by createFuncCallExprNode(IdentifierNode *, IdentifierNode *, ExprNodeList *).
  *
  * \post The memory at \a node and any of its associated members will be
  *       freed.
  *
  * \see createFuncCallExprNode(IdentifierNode *, IdentifierNode *, ExprNodeList *) */
void deleteFuncCallExprNode(FuncCallExprNode *node) /**< [in,out] A pointer to the FuncCallExprNode structure to be deleted. */
{
	if (!node) return;
	deleteIdentifierNode(node->scope);
	deleteIdentifierNode(node->name);
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
  * \see parseExprNode(Token ***)
  * \see parseStmtNode(Token ***)
  * \see parseBlockNode(Token ***)
  * \see parseMainNode(Token **) */
ConstantNode *parseConstantNode(Token ***tokenp)
{
	ConstantNode *ret = NULL;
	char *data = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	shiftout();
#endif

	/* Boolean */
	if (peekToken(&tokens, TT_BOOLEAN)) {
#ifdef DEBUG
		debug("CT_BOOLEAN");
#endif
		/* Create the ConstantNode structure */
		ret = createBooleanConstantNode((*tokens)->data.i);
		if (!ret) goto parseConstantNodeAbort;

		/* This should succeed; it was checked for above */
		status = acceptToken(&tokens, TT_BOOLEAN);
		if (!status) {
			error("expected boolean", tokens);
			goto parseConstantNodeAbort;
		}
	}
	/* Integer */
	else if (peekToken(&tokens, TT_INTEGER)) {
#ifdef DEBUG
		debug("CT_INTEGER");
#endif
		/* Create the ConstantNode structure */
		ret = createIntegerConstantNode((*tokens)->data.i);
		if (!ret) goto parseConstantNodeAbort;

		/* This should succeed; it was checked for above */
		status = acceptToken(&tokens, TT_INTEGER);
		if (!status) {
			error("expected integer", tokens);
			goto parseConstantNodeAbort;
		}
	}
	/* Float */
	else if (peekToken(&tokens, TT_FLOAT)) {
#ifdef DEBUG
		debug("CT_FLOAT");
#endif
		/* Create the ConstantNode structure */
		ret = createFloatConstantNode((*tokens)->data.f);
		if (!ret) goto parseConstantNodeAbort;

		/* This should succeed; it was checked for above */
		status = acceptToken(&tokens, TT_FLOAT);
		if (!status) {
			error("expected float", tokens);
			goto parseConstantNodeAbort;
		}
	}
	/* String */
	else if (peekToken(&tokens, TT_STRING)) {
		unsigned int len = strlen((*tokens)->image);
		data = malloc(sizeof(char) * (len - 1));
		if (!data) {
			perror("malloc");
			goto parseConstantNodeAbort;
		}
		strncpy(data, (*tokens)->image + 1, len - 2);
		data[len - 2] = '\0';
#ifdef DEBUG
		debug("CT_STRING");
#endif
		/* Create the ConstantNode structure */
		ret = createStringConstantNode(data);
		if (!ret) goto parseConstantNodeAbort;
		data = NULL;

		/* This should succeed; it was checked for above */
		status = acceptToken(&tokens, TT_STRING);
		if (!status) {
			error("expected string", tokens);
			goto parseConstantNodeAbort;
		}
	}
	else {
		error("expected constant value", tokens);
	}

#ifdef DEBUG
	shiftin();
#endif

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parseConstantNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (data) free(data);
	if (ret) deleteConstantNode(ret);

	return NULL;
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
  * \see parseExprNode(Token ***)
  * \see parseStmtNode(Token ***)
  * \see parseBlockNode(Token ***)
  * \see parseMainNode(Token **) */
TypeNode *parseTypeNode(Token ***tokenp) /**< [in,out] A pointer to the position of the next token to parse. */
{
	TypeNode *ret = NULL;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	shiftout();
#endif

	/* Nil */
	if (acceptToken(&tokens, TT_NOOB)) {
#ifdef DEBUG
		debug("CT_NIL");
#endif
		ret = createTypeNode(CT_NIL);
		if (!ret) goto parseTypeNodeAbort;
	}
	/* Boolean */
	else if (acceptToken(&tokens, TT_TROOF)) {
#ifdef DEBUG
		debug("CT_BOOLEAN");
#endif
		ret = createTypeNode(CT_BOOLEAN);
		if (!ret) goto parseTypeNodeAbort;
	}
	/* Integer */
	else if (acceptToken(&tokens, TT_NUMBR)) {
#ifdef DEBUG
		debug("CT_INTEGER");
#endif
		ret = createTypeNode(CT_INTEGER);
		if (!ret) goto parseTypeNodeAbort;
	}
	/* Float */
	else if (acceptToken(&tokens, TT_NUMBAR)) {
#ifdef DEBUG
		debug("CT_FLOAT");
#endif
		ret = createTypeNode(CT_FLOAT);
		if (!ret) goto parseTypeNodeAbort;
	}
	/* String */
	else if (acceptToken(&tokens, TT_YARN)) {
#ifdef DEBUG
		debug("CT_STRING");
#endif
		ret = createTypeNode(CT_STRING);
		if (!ret) goto parseTypeNodeAbort;
	}
	else {
		error("expected type", tokens);
	}

#ifdef DEBUG
	shiftin();
#endif

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parseTypeNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (ret) deleteTypeNode(ret);

	return NULL;
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
  * \see parseExprNode(Token ***)
  * \see parseStmtNode(Token ***)
  * \see parseBlockNode(Token ***)
  * \see parseMainNode(Token **) */
IdentifierNode *parseIdentifierNode(Token ***tokenp) /**< [in,out] A pointer to the position of the next token to parse. */
{
	IdentifierNode *ret = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	shiftout();
#endif

	/* Make sure there is an identifier to parse */
	if (peekToken(&tokens, TT_IDENTIFIER)) {
#ifdef DEBUG
		debug("IDENTIFIER");
#endif
		/* Create the new IdentifierNode structure */
		ret = createIdentifierNode((*tokens)->image, (*tokens)->fname, (*tokens)->line);
		if (!ret) goto parseIdentifierNodeAbort;

		/* This should succeed; it was checked for above */
		status = acceptToken(&tokens, TT_IDENTIFIER);
		if (!status) {
			error("expected identifier", tokens);
			goto parseIdentifierNodeAbort;
		}
	}
	else {
		error("expected identifier", tokens);
	}

#ifdef DEBUG
	shiftin();
#endif

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parseIdentifierNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (ret) deleteIdentifierNode(ret);

	return NULL;
}

/** Parses a set of tokens into an ExprNode structure containing a CastExprNode
  * structure.  Parsing begins at the token pointed to by \a tokenp.
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
  * \see parseConstantExprNode(Token ***)
  * \see parseIdentifierExprNode(Token ***)
  * \see parseFuncCallExprNode(Token ***)
  * \see parseOpExprNode(Token ***) */
ExprNode *parseCastExprNode(Token ***tokenp)
{
	ExprNode *target = NULL;
	TypeNode *newtype = NULL;
	CastExprNode *expr = NULL;
	ExprNode *ret = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	debug("ET_CAST");
#endif

	/* Parse the cast token */
	status = acceptToken(&tokens, TT_MAEK);
	if (!status) {
		error("expected MAEK", tokens);
		goto parseCastExprNodeAbort;
	}

	/* Parse the expression to cast */
	target = parseExprNode(&tokens);
	if (!target) goto parseCastExprNodeAbort;

	/* Optionally parse the cast-to token */
	acceptToken(&tokens, TT_A);

	/* Parse the type to cast to */
	newtype = parseTypeNode(&tokens);
	if (!newtype) goto parseCastExprNodeAbort;

	/* Create the new CastExprNode structure */
	expr = createCastExprNode(target, newtype);
	if (!expr) goto parseCastExprNodeAbort;

	/* Create the new ExprNode structure */
	ret = createExprNode(ET_CAST, expr);
	if (!ret) goto parseCastExprNodeAbort;

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parseCastExprNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (ret) deleteExprNode(ret);
	else if (expr) deleteCastExprNode(expr);
	else {
		if (newtype) deleteTypeNode(newtype);
		if (target) deleteExprNode(target);
	}

	return NULL;
}

/** Parses a set of tokens into an ExprNode structure containing a ConstantExprNode
  * structure.  Parsing begins at the token pointed to by \a tokenp.
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
  * \see parseCastExprNode(Token ***)
  * \see parseIdentifierExprNode(Token ***)
  * \see parseFuncCallExprNode(Token ***)
  * \see parseOpExprNode(Token ***) */
ExprNode *parseConstantExprNode(Token ***tokenp)
{
	ConstantNode *node = NULL;
	ExprNode *ret = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	debug("ET_CONSTANT");
#endif

	/* Parse the constant */
	node = parseConstantNode(&tokens);
	if (!node) goto parseConstantExprNodeAbort;

	/* Create the new ExprNode structure */
	ret = createExprNode(ET_CONSTANT, node);
	if (!ret) goto parseConstantExprNodeAbort;

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parseConstantExprNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (ret) deleteExprNode(ret);
	else {
		if (node) deleteConstantNode(node);
	}

	return NULL;
}

/** Parses a set of tokens into an ExprNode structure containing an
  * IdentifierExprNode structure.  Parsing begins at the token pointed to by
  * \a tokenp.
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
  * \see parseCastExprNode(Token ***)
  * \see parseConstantExprNode(Token ***)
  * \see parseFuncCallExprNode(Token ***)
  * \see parseOpExprNode(Token ***) */
ExprNode *parseIdentifierExprNode(Token ***tokenp)
{
	IdentifierNode *node = NULL;
	ExprNode *ret = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	debug("ET_IDENTIFIER");
#endif

	/* Parse the identifier node */
	node = parseIdentifierNode(&tokens);
	if (!node) goto parseIdentifierExprNodeAbort;

	/* Create the new ExprNode structure */
	ret = createExprNode(ET_IDENTIFIER, node);
	if (!ret) goto parseIdentifierExprNodeAbort;

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parseIdentifierExprNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (ret) deleteExprNode(ret);
	else {
		if (node) deleteIdentifierNode(node);
	}

	return NULL;
}

/** Parses a set of tokens into an ExprNode structure containing a
  * FuncCallExprNode structure.  Parsing begins at the token pointed to by
  * \a tokenp.
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
  * \see parseCastExprNode(Token ***)
  * \see parseConstantExprNode(Token ***)
  * \see parseIdentifierExprNode(Token ***)
  * \see parseOpExprNode(Token ***) */
ExprNode *parseFuncCallExprNode(Token ***tokenp)
{
	IdentifierNode *scope = NULL;
	IdentifierNode *name = NULL;
	ExprNodeList *args = NULL;
	ExprNode *arg = NULL;
	FuncCallExprNode *expr = NULL;
	ExprNode *ret = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	debug("ET_FUNCCALL");
#endif

	/* Parse the scope name */
	scope = parseIdentifierNode(&tokens);
	if (!scope) goto parseFuncCallExprNodeAbort;

	/* Parse the function call token */
	status = acceptToken(&tokens, TT_IZ);
	if (!status) {
		error("expected IZ", tokens);
		goto parseFuncCallExprNodeAbort;
	}

	/* Parse the function name */
	name = parseIdentifierNode(&tokens);
	if (!name) goto parseFuncCallExprNodeAbort;

	/* Create an argument list */
	args = createExprNodeList();
	if (!args) goto parseFuncCallExprNodeAbort;

	/* Parse the first argument token */
	if (acceptToken(&tokens, TT_YR)) {
		/* Parse the first argument */
		arg = parseExprNode(&tokens);
		if (!arg) goto parseFuncCallExprNodeAbort;

		/* Add the first argument to the argument list */
		status = addExprNode(args, arg);
		if (!status) goto parseFuncCallExprNodeAbort;
		arg = NULL;

		/* Parse additional arguments */
		while (acceptToken(&tokens, TT_ANYR)) {
			/* Parse the argument */
			arg = parseExprNode(&tokens);
			if (!arg) goto parseFuncCallExprNodeAbort;

			/* Add the argument to the argument list */
			status = addExprNode(args, arg);
			if (!status) goto parseFuncCallExprNodeAbort;
			arg = NULL;
		}
	}

	/* Parse the end-of-argument token */
	status = acceptToken(&tokens, TT_MKAY);
	if (!status) {
		error("expected MKAY", tokens);
		goto parseFuncCallExprNodeAbort;
	}

	/* Create the new FuncCallExprNode structure */
	expr = createFuncCallExprNode(scope, name, args);
	if (!expr) goto parseFuncCallExprNodeAbort;

	/* Create the new ExprNode structure */
	ret = createExprNode(ET_FUNCCALL, expr);
	if (!ret) goto parseFuncCallExprNodeAbort;

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parseFuncCallExprNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (ret) deleteExprNode(ret);
	else if (expr) deleteFuncCallExprNode(expr);
	else {
		if (args) deleteExprNodeList(args);
		if (arg) deleteExprNode(arg);
		if (name) deleteIdentifierNode(name);
		if (scope) deleteIdentifierNode(scope);
	}

	return NULL;
}

/** Parses a set of tokens into an ExprNode structure containing an OpExprNode
  * structure.  Parsing begins at the token pointed to by
  * \a tokenp.
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
  * \see parseCastExprNode(Token ***)
  * \see parseConstantExprNode(Token ***)
  * \see parseFuncCallExprNode(Token ***)
  * \see parseOpExprNode(Token ***) */
ExprNode *parseOpExprNode(Token ***tokenp)
{
	enum ArityType {
		AR_UNARY,
		AR_BINARY,
		AR_NARY,
	};
	enum ArityType arity;
	ExprNodeList *args = NULL;
	ExprNode *arg = NULL;
	OpExprNode *expr = NULL;
	OpType type;
	ExprNode *ret = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

	/* Unary operations */
	if (acceptToken(&tokens, TT_NOT)) {
		type = OP_NOT;
		arity = AR_UNARY;
#ifdef DEBUG
		debug("ET_OP (OP_NOT)");
#endif
	}
	/* Binary operations */
	else if (acceptToken(&tokens, TT_SUMOF)) {
		type = OP_ADD;
		arity = AR_BINARY;
#ifdef DEBUG
		debug("ET_OP (OP_ADD)");
#endif
	}
	else if(acceptToken(&tokens, TT_DIFFOF)) {
		type = OP_SUB;
		arity = AR_BINARY;
#ifdef DEBUG
		debug("ET_OP (OP_SUB)");
#endif
	}
	else if(acceptToken(&tokens, TT_PRODUKTOF)) {
		type = OP_MULT;
		arity = AR_BINARY;
#ifdef DEBUG
		debug("ET_OP (OP_MULT)");
#endif
	}
	else if(acceptToken(&tokens, TT_QUOSHUNTOF)) {
		type = OP_DIV;
		arity = AR_BINARY;
#ifdef DEBUG
		debug("ET_OP (OP_DIV)");
#endif
	}
	else if(acceptToken(&tokens, TT_MODOF)) {
		type = OP_MOD;
		arity = AR_BINARY;
#ifdef DEBUG
		debug("ET_OP (OP_MOD)");
#endif
	}
	else if(acceptToken(&tokens, TT_BIGGROF)) {
		type = OP_MAX;
		arity = AR_BINARY;
#ifdef DEBUG
		debug("ET_OP (OP_MAX)");
#endif
	}
	else if(acceptToken(&tokens, TT_SMALLROF)) {
		type = OP_MIN;
		arity = AR_BINARY;
#ifdef DEBUG
		debug("ET_OP (OP_MIN)");
#endif
	}
	else if(acceptToken(&tokens, TT_BOTHOF)) {
		type = OP_AND;
		arity = AR_BINARY;
#ifdef DEBUG
		debug("ET_OP (OP_AND)");
#endif
	}
	else if(acceptToken(&tokens, TT_EITHEROF)) {
		type = OP_OR;
		arity = AR_BINARY;
#ifdef DEBUG
		debug("ET_OP (OP_OR)");
#endif
	}
	else if(acceptToken(&tokens, TT_WONOF)) {
		type = OP_XOR;
		arity = AR_BINARY;
#ifdef DEBUG
		debug("ET_OP (OP_XOR)");
#endif
	}
	else if(acceptToken(&tokens, TT_BOTHSAEM)) {
		type = OP_EQ;
		arity = AR_BINARY;
#ifdef DEBUG
		debug("ET_OP (OP_EQ)");
#endif
	}
	else if(acceptToken(&tokens, TT_DIFFRINT)) {
		type = OP_NEQ;
		arity = AR_BINARY;
#ifdef DEBUG
		debug("ET_OP (OP_NEQ)");
#endif
	}
	/* N-ary operators */
	else if (acceptToken(&tokens, TT_ALLOF)) {
		type = OP_AND;
		arity = AR_NARY;
#ifdef DEBUG
		debug("ET_OP (OP_AND)");
#endif
	}
	else if(acceptToken(&tokens, TT_ANYOF)) {
		type = OP_OR;
		arity = AR_NARY;
#ifdef DEBUG
		debug("ET_OP (OP_OR)");
#endif
	}
	else if(acceptToken(&tokens, TT_SMOOSH)) {
		type = OP_CAT;
		arity = AR_NARY;
#ifdef DEBUG
		debug("ET_OP (OP_CAT)");
#endif
	}
	else {
		error("invalid operator", tokens);
		return NULL;
	}

	/* Create the argument list */
	args = createExprNodeList();
	if (!args) goto parseOpExprNodeAbort;

	/* Parse the operation arguments */
	if (arity == AR_UNARY) {
		/* Parse the argument */
		arg = parseExprNode(&tokens);
		if (!arg) goto parseOpExprNodeAbort;

		/* Add the argument to the argument list */
		status = addExprNode(args, arg);
		if (!status) goto parseOpExprNodeAbort;
		arg = NULL;
	}
	else if (arity == AR_BINARY) {
		/* Parse the first argument */
		arg = parseExprNode(&tokens);
		if (!arg) goto parseOpExprNodeAbort;

		/* Add the first argument to the argument list */
		status = addExprNode(args, arg);
		if (!status) goto parseOpExprNodeAbort;
		arg = NULL;

		/* Optionally parse the argument-separator token */
		acceptToken(&tokens, TT_AN);

		/* Parse the second argument */
		arg = parseExprNode(&tokens);
		if (!arg) goto parseOpExprNodeAbort;

		/* Add the second argument to the argument list */
		status = addExprNode(args, arg);
		if (!status) goto parseOpExprNodeAbort;
		arg = NULL;
	}
	else if (arity == AR_NARY) {
		/* Parse as many arguments as possible */
		while (1) {
			/* Parse an argument */
			arg = parseExprNode(&tokens);
			if (!arg) goto parseOpExprNodeAbort;

			/* Add the argument to the argument list */
			status = addExprNode(args, arg);
			if (!status) goto parseOpExprNodeAbort;
			arg = NULL;
			
			/* Stop if the end-of-arguments token is present */
			if (acceptToken(&tokens, TT_MKAY)) break;

			/* Optionally parse the argument-separator token */
			acceptToken(&tokens, TT_AN);
		}
	}

	/* Create the new OpExprNode structure */
	expr = createOpExprNode(type, args);
	if (!expr) goto parseOpExprNodeAbort;

	/* Create the new ExprNode structure */
	ret = createExprNode(ET_OP, expr);
	if (!ret) goto parseOpExprNodeAbort;

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parseOpExprNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (ret) deleteExprNode(ret);
	else if (expr) deleteOpExprNode(expr);
	else {
		if (arg) deleteExprNode(arg);
		if (args) deleteExprNodeList(args);
	}

	return NULL;
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
  * \see parseStmtNode(Token ***)
  * \see parseBlockNode(Token ***)
  * \see parseMainNode(Token **) */
ExprNode *parseExprNode(Token ***tokenp) /**< [in,out] A pointer to the position of the next token to parse. */
{
	Token **tokens = *tokenp;
	ExprNode *ret = NULL;

#ifdef DEBUG
	shiftout();
#endif

	/* Cast */
	if (peekToken(&tokens, TT_MAEK)) {
		ret = parseCastExprNode(tokenp);
	}
	/* Constant value */
	else if (peekToken(&tokens, TT_BOOLEAN)
			|| peekToken(&tokens, TT_INTEGER)
			|| peekToken(&tokens, TT_FLOAT)
			|| peekToken(&tokens, TT_STRING))
		ret = parseConstantExprNode(tokenp);
	/* Function call (must come before identifier) */
	else if (nextToken(&tokens, TT_IZ)) {
		ret = parseFuncCallExprNode(tokenp);
	}
	/* Identifier */
	else if (peekToken(&tokens, TT_IDENTIFIER)) {
		ret = parseIdentifierExprNode(tokenp);
	}
	/* Operation */
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
			|| peekToken(&tokens, TT_DIFFRINT)
			|| peekToken(&tokens, TT_ANYOF)
			|| peekToken(&tokens, TT_ALLOF)
			|| peekToken(&tokens, TT_SMOOSH)
			|| peekToken(&tokens, TT_NOT)) {
		ret = parseOpExprNode(tokenp);
	}
	/* Implicit variable */
	else if (acceptToken(&tokens, TT_IT)) {
#ifdef DEBUG
		debug("ET_IMPVAR");
#endif

		/* Create the new ExprNode structure */
		ret = createExprNode(ET_IMPVAR, NULL);
		if (!ret) return NULL;

		/* Since we're successful, update the token stream */
		*tokenp = tokens;
	}
	else {
		error("expected expression", tokens);
	}

#ifdef DEBUG
	shiftin();
#endif

	return ret;
}

/** Parses a set of tokens into a StmtNode structure containing a CastStmtNode
  * structure.  Parsing begins at the token pointed to by \a tokenp.
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
  * \see parsePrintStmtNode(Token ***)
  * \see parseInputStmtNode(Token ***)
  * \see parseAssignmentStmtNode(Token ***)
  * \see parseDeclarationStmtNode(Token ***)
  * \see parseIfThenElseStmtNode(Token ***)
  * \see parseSwitchStmtNode(Token ***)
  * \see parseBreakStmtNode(Token ***)
  * \see parseReturnStmtNode(Token ***)
  * \see parseLoopStmtNode(Token ***)
  * \see parseDeallocationStmtNode(Token ***)
  * \see parseFuncDefStmtNode(Token ***) */
StmtNode *parseCastStmtNode(Token ***tokenp)
{
	IdentifierNode *target = NULL;
	TypeNode *newtype = NULL;
	CastStmtNode *stmt = NULL;
	StmtNode *ret = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	debug("ST_CAST");
#endif

	/* Parse the target of the cast statement */
	target = parseIdentifierNode(&tokens);
	if (!target) goto parseCastStmtNodeAbort;

	/* Remove the cast keywords from the token stream */
	status = acceptToken(&tokens, TT_ISNOWA);
	if (!status) {
		error("expected IS NOW A", tokens);
		goto parseCastStmtNodeAbort;
	}

	/* Parse the type to cast to */
	newtype = parseTypeNode(&tokens);
	if (!newtype) goto parseCastStmtNodeAbort;

	/* Make sure the statement ends with a newline */
	status = acceptToken(&tokens, TT_NEWLINE);
	if (!status) {
		error("expected end of expression", tokens);
		goto parseCastStmtNodeAbort;
	}

	/* Create the new CastStmtNode structure */
	stmt = createCastStmtNode(target, newtype);
	if (!stmt) goto parseCastStmtNodeAbort;

	/* Create the new StmtNode structure */
	ret = createStmtNode(ST_CAST, stmt);
	if (!ret) goto parseCastStmtNodeAbort;

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parseCastStmtNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (ret) deleteStmtNode(ret);
	else if (stmt) deleteCastStmtNode(stmt);
	else {
		if (newtype) deleteTypeNode(newtype);
		if (target) deleteIdentifierNode(target);
	}

	return NULL;
}

/** Parses a set of tokens into a StmtNode structure containing a PrintStmtNode
  * structure.  Parsing begins at the token pointed to by \a tokenp.
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
  * \see parseCastStmtNode(Token ***)
  * \see parseInputStmtNode(Token ***)
  * \see parseAssignmentStmtNode(Token ***)
  * \see parseDeclarationStmtNode(Token ***)
  * \see parseIfThenElseStmtNode(Token ***)
  * \see parseSwitchStmtNode(Token ***)
  * \see parseBreakStmtNode(Token ***)
  * \see parseReturnStmtNode(Token ***)
  * \see parseLoopStmtNode(Token ***)
  * \see parseDeallocationStmtNode(Token ***)
  * \see parseFuncDefStmtNode(Token ***) */
StmtNode *parsePrintStmtNode(Token ***tokenp)
{
	ExprNode *arg = NULL;
	ExprNodeList *args = NULL;
	int nonl = 0;
	PrintStmtNode *stmt = NULL;
	StmtNode *ret = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	debug("ST_PRINT");
#endif

	/* Remove the print keyword from the token stream */
	status = acceptToken(&tokens, TT_VISIBLE);
	if (!status) {
		error("expected VISIBLE", tokens);
		goto parsePrintStmtNodeAbort;
	}

	/* Parse the arguments to the print statement */
	args = createExprNodeList();
	if (!args) goto parsePrintStmtNodeAbort;
	do {
		/* Parse an argument; it should be an expression */
		arg = parseExprNode(&tokens);
		if (!arg) goto parsePrintStmtNodeAbort;

		/* Add it to the list of arguments */
		status = addExprNode(args, arg);
		if (!status) goto parsePrintStmtNodeAbort;
		arg = NULL;

		/* Arguments can optionally be separated by an AN keyword */
		acceptToken(&tokens, TT_AN);
	} while (!peekToken(&tokens, TT_NEWLINE)
			&& !peekToken(&tokens, TT_BANG));

	/* Check for the no-newline token */
	if(acceptToken(&tokens, TT_BANG)) nonl = 1;

	/* Make sure the statement ends with a newline */
	status = acceptToken(&tokens, TT_NEWLINE);
	if (!status) {
		error("expected end of expression", tokens);
		goto parsePrintStmtNodeAbort;
	}

	/* Create the new PrintStmtNode structure */
	stmt = createPrintStmtNode(args, nonl);
	if (!stmt) goto parsePrintStmtNodeAbort;

	/* Create the new StmtNode structure */
	ret = createStmtNode(ST_PRINT, stmt);
	if (!ret) goto parsePrintStmtNodeAbort;

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parsePrintStmtNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (ret) deleteStmtNode(ret);
	else if (stmt) deletePrintStmtNode(stmt);
	else {
		if (arg) deleteExprNode(arg);
		if (args) deleteExprNodeList(args);
	}

	return NULL;
}

/** Parses a set of tokens into a StmtNode structure containing an InputStmtNode
  * structure.  Parsing begins at the token pointed to by \a tokenp.
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
  * \see parseCastStmtNode(Token ***)
  * \see parsePrintStmtNode(Token ***)
  * \see parseAssignmentStmtNode(Token ***)
  * \see parseDeclarationStmtNode(Token ***)
  * \see parseIfThenElseStmtNode(Token ***)
  * \see parseSwitchStmtNode(Token ***)
  * \see parseBreakStmtNode(Token ***)
  * \see parseReturnStmtNode(Token ***)
  * \see parseLoopStmtNode(Token ***)
  * \see parseDeallocationStmtNode(Token ***)
  * \see parseFuncDefStmtNode(Token ***) */
StmtNode *parseInputStmtNode(Token ***tokenp)
{
	IdentifierNode *target = NULL;
	InputStmtNode *stmt = NULL;
	StmtNode *ret = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	debug("ST_INPUT");
#endif

	/* Remove the input keyword from the token stream */
	status = acceptToken(&tokens, TT_GIMMEH);
	if (!status) {
		error("expected GIMMEH", tokens);
		goto parseInputStmtNodeAbort;
	}

	/* Parse the identifier to store the input into */
	target = parseIdentifierNode(&tokens);
	if (!target) goto parseInputStmtNodeAbort;

	/* Make sure the statement ends with a newline */
	status = acceptToken(&tokens, TT_NEWLINE);
	if (!status) {
		error("expected end of expression", tokens);
		goto parseInputStmtNodeAbort;
	}

	/* Create the new InputStmtNode structure */
	stmt = createInputStmtNode(target);
	if (!stmt) goto parseInputStmtNodeAbort;

	/* Create the new StmtNode structure */
	ret = createStmtNode(ST_INPUT, stmt);
	if (!ret) goto parseInputStmtNodeAbort;

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parseInputStmtNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (ret) deleteStmtNode(ret);
	else if (stmt) deleteInputStmtNode(stmt);
	else {
		if (target) deleteIdentifierNode(target);
	}

	return NULL;
}

/** Parses a set of tokens into a StmtNode structure containing an
  * AssignmentStmtNode structure.  Parsing begins at the token pointed to by
  * \a tokenp.
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
  * \see parseCastStmtNode(Token ***)
  * \see parsePrintStmtNode(Token ***)
  * \see parseInputStmtNode(Token ***)
  * \see parseDeclarationStmtNode(Token ***)
  * \see parseIfThenElseStmtNode(Token ***)
  * \see parseSwitchStmtNode(Token ***)
  * \see parseBreakStmtNode(Token ***)
  * \see parseReturnStmtNode(Token ***)
  * \see parseLoopStmtNode(Token ***)
  * \see parseDeallocationStmtNode(Token ***)
  * \see parseFuncDefStmtNode(Token ***) */
StmtNode *parseAssignmentStmtNode(Token ***tokenp)
{
	IdentifierNode *target = NULL;
	ExprNode *expr = NULL;
	AssignmentStmtNode *stmt = NULL;
	StmtNode *ret = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	debug("ST_ASSIGNMENT");
#endif

	/* Parse the target of the assignment */
	target = parseIdentifierNode(&tokens);
	if (!target) goto parseAssignmentStmtNodeAbort;

	/* Remove the assignment keyword from the token stream */
	if (!acceptToken(&tokens, TT_R)) {
		error("expected R", tokens);
		goto parseAssignmentStmtNodeAbort;
	}

	/* Parse the expression to assign */
	expr = parseExprNode(&tokens);
	if (!expr) goto parseAssignmentStmtNodeAbort;

	/* Make sure the statement ends with a newline */
	status = acceptToken(&tokens, TT_NEWLINE);
	if (!status) {
		error("expected end of statement", tokens);
		goto parseAssignmentStmtNodeAbort;
	}

	/* Create the new AssignmentStmtNode structure */
	stmt = createAssignmentStmtNode(target, expr);
	if (!stmt) goto parseAssignmentStmtNodeAbort;

	/* Create the new StmtNode structure */
	ret = createStmtNode(ST_ASSIGNMENT, stmt);
	if (!ret) goto parseAssignmentStmtNodeAbort;

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parseAssignmentStmtNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (ret) deleteStmtNode(ret);
	else if (stmt) deleteAssignmentStmtNode(stmt);
	else {
		if (expr) deleteExprNode(expr);
		if (target) deleteIdentifierNode(target);
	}

	return NULL;
}

/** Parses a set of tokens into a StmtNode structure containing a
  * DeclarationStmtNode structure.  Parsing begins at the token pointed to by
  * \a tokenp.
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
  * \see parseCastStmtNode(Token ***)
  * \see parsePrintStmtNode(Token ***)
  * \see parseInputStmtNode(Token ***)
  * \see parseAssignmentStmtNode(Token ***)
  * \see parseIfThenElseStmtNode(Token ***)
  * \see parseSwitchStmtNode(Token ***)
  * \see parseBreakStmtNode(Token ***)
  * \see parseReturnStmtNode(Token ***)
  * \see parseLoopStmtNode(Token ***)
  * \see parseDeallocationStmtNode(Token ***)
  * \see parseFuncDefStmtNode(Token ***) */
StmtNode *parseDeclarationStmtNode(Token ***tokenp)
{
	IdentifierNode *scope = NULL;
	IdentifierNode *target = NULL;
	ExprNode *expr = NULL;
	TypeNode *type = NULL;
	DeclarationStmtNode *stmt = NULL;
	StmtNode *ret = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	debug("ST_DECLARATION");
#endif

	/* Parse the scope to declare the variable in */
	scope = parseIdentifierNode(&tokens);
	if (!scope) goto parseDeclarationStmtNodeAbort;

	/* Remove the declaration keywords from the token stream */
	if (!acceptToken(&tokens, TT_HASA)) {
		error("expected HAS A", tokens);
		goto parseDeclarationStmtNodeAbort;
	}

	/* Parse the identifier to declare */
	target = parseIdentifierNode(&tokens);
	if (!target) goto parseDeclarationStmtNodeAbort;

	/* Check for an optional initialization */
	if (acceptToken(&tokens, TT_ITZ)) {
		/* Parse the expression to initialize to */
		expr = parseExprNode(&tokens);
		if (!expr) goto parseDeclarationStmtNodeAbort;
	}
	/* Check for an optional type initialization */
	else if (acceptToken(&tokens, TT_ITZA)) {
		/* Parse the type to initialize to */
		type = parseTypeNode(&tokens);
		if (!type) goto parseDeclarationStmtNodeAbort;
	}

	/* Make sure the statement ends with a newline */
	status = acceptToken(&tokens, TT_NEWLINE);
	if (!status) {
		error("expected end of statement", tokens);
		goto parseDeclarationStmtNodeAbort;
	}

	/* Create the new DeclarationStmtNode structure */
	stmt = createDeclarationStmtNode(scope, target, expr, type);
	if (!stmt) goto parseDeclarationStmtNodeAbort;

	/* Create the new StmtNode structure */
	ret = createStmtNode(ST_DECLARATION, stmt); 
	if (!ret) goto parseDeclarationStmtNodeAbort;

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parseDeclarationStmtNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (ret) deleteStmtNode(ret);
	else if (stmt) deleteDeclarationStmtNode(stmt);
	else {
		if (type) deleteTypeNode(type);
		if (expr) deleteExprNode(expr);
		if (target) deleteIdentifierNode(target);
		if (scope) deleteIdentifierNode(scope);
	}

	return NULL;
}

/** Parses a set of tokens into a StmtNode structure containing an
  * IfThenElseStmtNode structure.  Parsing begins at the token pointed to by
  * \a tokenp.
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
  * \see parseCastStmtNode(Token ***)
  * \see parsePrintStmtNode(Token ***)
  * \see parseInputStmtNode(Token ***)
  * \see parseAssignmentStmtNode(Token ***)
  * \see parseDeclarationStmtNode(Token ***)
  * \see parseSwitchStmtNode(Token ***)
  * \see parseBreakStmtNode(Token ***)
  * \see parseReturnStmtNode(Token ***)
  * \see parseLoopStmtNode(Token ***)
  * \see parseDeallocationStmtNode(Token ***)
  * \see parseFuncDefStmtNode(Token ***) */
StmtNode *parseIfThenElseStmtNode(Token ***tokenp)
{
	BlockNode *yes = NULL;
	ExprNodeList *guards = NULL;
	BlockNodeList *blocks = NULL;
	ExprNode *guard = NULL;
	BlockNode *block = NULL;
	BlockNode *no = NULL;
	IfThenElseStmtNode *stmt = NULL;
	StmtNode *ret = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	debug("ST_CONDITIONAL");
#endif

	/* Remove the if keyword from the token stream */
	status = acceptToken(&tokens, TT_ORLY);
	if (!status) {
		error("expected O RLY?", tokens);
		goto parseIfThenElseStmtNodeAbort;
	}

	/* The if keyword must appear on its own line */
	if (!acceptToken(&tokens, TT_NEWLINE)) {
		error("expected end of expression", tokens);
		goto parseIfThenElseStmtNodeAbort;
	}

	/* Parse the true branch keyword */
	status = acceptToken(&tokens, TT_YARLY);
	if (!status) {
		error("expected YA RLY", tokens);
		goto parseIfThenElseStmtNodeAbort;
	}

	/* The true branch keyword must appear on its own line */
	status = acceptToken(&tokens, TT_NEWLINE);
	if (!status) {
		error("expected end of expression", tokens);
		goto parseIfThenElseStmtNodeAbort;
	}

	/* Parse the true part of the branch */
	yes = parseBlockNode(&tokens);
	if (!yes) goto parseIfThenElseStmtNodeAbort;

	/* Set up lists of guards and blocks */
	guards = createExprNodeList();
	if (!guards) goto parseIfThenElseStmtNodeAbort;
	blocks = createBlockNodeList();
	if (!blocks) goto parseIfThenElseStmtNodeAbort;

	/* Parse the else-if keyword */
	while (acceptToken(&tokens, TT_MEBBE)) {
		/* Parse an else-if guard */
		guard = parseExprNode(&tokens);
		if (!guard) goto parseIfThenElseStmtNodeAbort;

		/* Add the else-if guard to the guard list */
		status = addExprNode(guards, guard);
		if (!status) goto parseIfThenElseStmtNodeAbort;
		guard = NULL;

		/* The else-if keyword and guard must be on their own line */
		status = acceptToken(&tokens, TT_NEWLINE);
		if (!status) {
			error("expected end of expression", tokens);
			goto parseIfThenElseStmtNodeAbort;
		}

		/* Parse the else-if block */
		block = parseBlockNode(&tokens);
		if (!block) goto parseIfThenElseStmtNodeAbort;

		/* Add the else-if block to the block list */
		status = addBlockNode(blocks, block);
		if (!status) goto parseIfThenElseStmtNodeAbort;
		block = NULL;
	}

	/* Parse the else keyword */
	if (acceptToken(&tokens, TT_NOWAI)) {
		/* The else keyword must appear on its own line */
		status = acceptToken(&tokens, TT_NEWLINE);
		if (!status) {
			error("expected end of expression", tokens);
			goto parseIfThenElseStmtNodeAbort;
		}

		/* Parse the else block */
		no = parseBlockNode(&tokens);
		if (!no) goto parseIfThenElseStmtNodeAbort;
	}

	/* Parse the end-if-block keyword */
	status = acceptToken(&tokens, TT_OIC);
	if (!status) {
		error("expected OIC", tokens);
		goto parseIfThenElseStmtNodeAbort;
	}

	/* The end-if-block keyword must appear on its own line */
	status = acceptToken(&tokens, TT_NEWLINE);
	if (!status) {
		error("expected end of expression", tokens);
		goto parseIfThenElseStmtNodeAbort;
	}

	/* Create the new IfThenElseStmtNode structure */
	stmt = createIfThenElseStmtNode(yes, no, guards, blocks);
	if (!stmt) goto parseIfThenElseStmtNodeAbort;

	/* Create the new StmtNode structure */
	ret = createStmtNode(ST_IFTHENELSE, stmt);
	if (!ret) goto parseIfThenElseStmtNodeAbort;

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parseIfThenElseStmtNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (ret) deleteStmtNode(ret);
	else if (stmt) deleteIfThenElseStmtNode(stmt);
	else {
		if (no) deleteBlockNode(no);
		if (blocks) deleteBlockNodeList(blocks);
		if (guards) deleteExprNodeList(guards);
		if (block) deleteBlockNode(block);
		if (guard) deleteExprNode(guard);
		if (yes) deleteBlockNode(yes);
	}

	return NULL;
}

/** Parses a set of tokens into a StmtNode structure containing a SwitchStmtNode
  * structure.  Parsing begins at the token pointed to by \a tokenp.
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
  * \see parseCastStmtNode(Token ***)
  * \see parsePrintStmtNode(Token ***)
  * \see parseInputStmtNode(Token ***)
  * \see parseAssignmentStmtNode(Token ***)
  * \see parseDeclarationStmtNode(Token ***)
  * \see parseIfThenElseStmtNode(Token ***)
  * \see parseBreakStmtNode(Token ***)
  * \see parseReturnStmtNode(Token ***)
  * \see parseLoopStmtNode(Token ***)
  * \see parseDeallocationStmtNode(Token ***)
  * \see parseFuncDefStmtNode(Token ***) */
StmtNode *parseSwitchStmtNode(Token ***tokenp)
{
	ExprNodeList *guards = NULL;
	BlockNodeList *blocks = NULL;
	ConstantNode *c = NULL;
	ExprNode *guard = NULL;
	BlockNode *block = NULL;
	BlockNode *def = NULL;
	SwitchStmtNode *stmt = NULL;
	StmtNode *ret = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	debug("ST_SWITCH");
#endif

	/* Remove the switch keyword from the token stream */
	status = acceptToken(&tokens, TT_WTF);
	if (!status) {
		error("expected WTF?", tokens);
		goto parseSwitchStmtNodeAbort;
	}

	/* The switch keyword must appear on its own line */
	status = acceptToken(&tokens, TT_NEWLINE);
	if (!status) {
		error("expected end of expression", tokens);
		goto parseSwitchStmtNodeAbort;
	}

	/* Set up lists of guards and blocks */
	guards = createExprNodeList();
	if (!guards) goto parseSwitchStmtNodeAbort;
	blocks = createBlockNodeList();
	if (!blocks) goto parseSwitchStmtNodeAbort;

	/* Parse at least one case */
	do {
		unsigned int n = 0;

		/* Parse the case keyword */
		status = acceptToken(&tokens, TT_OMG);
		if (!status) {
			error("expected OMG", tokens);
			goto parseSwitchStmtNodeAbort;
		}

		/* Parse a constant for the case */
		/** \note The 1.2 specification only allows constant
		 *       values for OMG guards thus this function
		 *       explicitly checks for them. */
		c = parseConstantNode(&tokens);
		if (!c) goto parseSwitchStmtNodeAbort;

		/* String interpolation is not allowed */
		if (c->type == CT_STRING && strstr(c->data.s, ":{")) {
			error("cannot use an interpolated string as an OMG literal", tokens);
			goto parseSwitchStmtNodeAbort;
		}

		/* Make sure the constant is unique to this switch statement */
		for (n = 0; n < guards->num; n++) {
			ConstantNode *test = guards->exprs[n]->expr;
			if (c->type != test->type) continue;
			/* Check for equivalent types and values */
			if (((c->type == CT_BOOLEAN || c->type == CT_INTEGER)
					&& c->data.i == test->data.i)
					|| (c->type == CT_FLOAT
					&& c->data.f == test->data.f)
					|| (c->type == CT_STRING
					&& !strcmp(c->data.s, test->data.s))) {
				error("OMG literal must be unique", tokens);
				goto parseSwitchStmtNodeAbort;
			}
		}

		/* Package the constant in an expression */
		guard = createExprNode(ET_CONSTANT, c);
		if (!guard) goto parseSwitchStmtNodeAbort;

		/* Add the guard to the list of guards */
		status = addExprNode(guards, guard);
		if (!status) goto parseSwitchStmtNodeAbort;
		guard = NULL;

		/* Make sure the guard appears on its own line */
		status = acceptToken(&tokens, TT_NEWLINE);
		if (!status) {
			error("expected end of expression", tokens);
			goto parseSwitchStmtNodeAbort;
		}

		/* Parse the block associated with the guard */
		block = parseBlockNode(&tokens);
		if (!block) goto parseSwitchStmtNodeAbort;

		/* Add the block to the list of blocks */
		status = addBlockNode(blocks, block);
		if (!status) goto parseSwitchStmtNodeAbort;
		block = NULL;
	} while (peekToken(&tokens, TT_OMG));

	/* Check for the default case */
	if (acceptToken(&tokens, TT_OMGWTF)) {
		/* Make sure the default case keyword appears on its own line */
		status = acceptToken(&tokens, TT_NEWLINE);
		if (!status) {
			error("expected end of expression", tokens);
			goto parseSwitchStmtNodeAbort;
		}

		/* Parse the default case block */
		def = parseBlockNode(&tokens);
		if (!def) goto parseSwitchStmtNodeAbort;
	}

	/* Parse the end-switch keyword */
	status = acceptToken(&tokens, TT_OIC);
	if (!status) {
		error("expected OIC", tokens);
		goto parseSwitchStmtNodeAbort;
	}

	/* Make sure the end-switch keyword appears on its own line */
	status = acceptToken(&tokens, TT_NEWLINE);
	if (!status) {
		error("expected end of expression", tokens);
		goto parseSwitchStmtNodeAbort;
	}

	/* Create the new SwitchStmtNode structure */
	stmt = createSwitchStmtNode(guards, blocks, def);
	if (!stmt) goto parseSwitchStmtNodeAbort;

	/* Create the new StmtNode structure */
	ret = createStmtNode(ST_SWITCH, stmt);
	if (!ret) goto parseSwitchStmtNodeAbort;

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parseSwitchStmtNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (ret) deleteStmtNode(ret);
	else if (stmt) deleteSwitchStmtNode(stmt);
	else {
		if (def) deleteBlockNode(def);
		if (block) deleteBlockNode(block);
		if (guard) deleteExprNode(guard);
		if (c) deleteConstantNode(c);
		if (blocks) deleteBlockNodeList(blocks);
		if (guards) deleteExprNodeList(guards);
	}

	return NULL;
}

/** Parses a set of tokens into a StmtNode structure representing a break
  * statement.  Parsing begins at the token pointed to by \a tokenp.
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
  * \see parseCastStmtNode(Token ***)
  * \see parsePrintStmtNode(Token ***)
  * \see parseInputStmtNode(Token ***)
  * \see parseAssignmentStmtNode(Token ***)
  * \see parseDeclarationStmtNode(Token ***)
  * \see parseIfThenElseStmtNode(Token ***)
  * \see parseSwitchStmtNode(Token ***)
  * \see parseReturnStmtNode(Token ***)
  * \see parseLoopStmtNode(Token ***)
  * \see parseDeallocationStmtNode(Token ***)
  * \see parseFuncDefStmtNode(Token ***)
  * \see parseExprNode(Token ***) */
StmtNode *parseBreakStmtNode(Token ***tokenp)
{
	StmtNode *ret = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	debug("ST_BREAK");
#endif

	/* Remove the break keyword from the token stream */
	status = acceptToken(&tokens, TT_GTFO);
	if (!status) {
		error("expected GTFO", tokens);
		goto parseBreakStmtNodeAbort;
	}

	/* The break keyword must appear on its own line */
	status = acceptToken(&tokens, TT_NEWLINE);
	if (!status) {
		error("expected end of expression", tokens);
		goto parseBreakStmtNodeAbort;
	}

	/* Create the new StmtNode structure */
	ret = createStmtNode(ST_BREAK, NULL);
	if (!ret) goto parseBreakStmtNodeAbort;

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parseBreakStmtNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (ret) deleteStmtNode(ret);
	return NULL;
}

/** Parses a set of tokens into a StmtNode structure containing a ReturnStmtNode
  * structure.  Parsing begins at the token pointed to by \a tokenp.
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
  * \see parseCastStmtNode(Token ***)
  * \see parsePrintStmtNode(Token ***)
  * \see parseInputStmtNode(Token ***)
  * \see parseAssignmentStmtNode(Token ***)
  * \see parseDeclarationStmtNode(Token ***)
  * \see parseIfThenElseStmtNode(Token ***)
  * \see parseSwitchStmtNode(Token ***)
  * \see parseBreakStmtNode(Token ***)
  * \see parseLoopStmtNode(Token ***)
  * \see parseDeallocationStmtNode(Token ***)
  * \see parseFuncDefStmtNode(Token ***) */
StmtNode *parseReturnStmtNode(Token ***tokenp)
{
	ExprNode *value = NULL;
	ReturnStmtNode *stmt = NULL;
	StmtNode *ret = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	debug("ST_RETURN");
#endif

	/* Remove the return keyword from the token stream */
	status = acceptToken(&tokens, TT_FOUNDYR);
	if (!status) {
		error("expected FOUND YR", tokens);
		goto parseReturnStmtNodeAbort;
	}

	/* Parse the return value */
	value = parseExprNode(&tokens);
	if (!value) goto parseReturnStmtNodeAbort;

	/* The return statement must reside on its own line */
	status = acceptToken(&tokens, TT_NEWLINE);
	if (!status) {
		error("expected end of expression", tokens);
		goto parseReturnStmtNodeAbort;
	}

	/* Create the new ReturnStmtNode structure */
	stmt = createReturnStmtNode(value);
	if (!stmt) goto parseReturnStmtNodeAbort;

	/* Create the new StmtNode structure */
	ret = createStmtNode(ST_RETURN, stmt);
	if (!ret) goto parseReturnStmtNodeAbort;

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parseReturnStmtNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (ret) deleteStmtNode(ret);
	else if (stmt) deleteReturnStmtNode(stmt);
	else {
		if (value) deleteExprNode(value);
	}
	return NULL;
}

/** Parses a set of tokens into a StmtNode structure containing a LoopStmtNode
  * structure.  Parsing begins at the token pointed to by \a tokenp.
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
  * \see parseCastStmtNode(Token ***)
  * \see parsePrintStmtNode(Token ***)
  * \see parseInputStmtNode(Token ***)
  * \see parseAssignmentStmtNode(Token ***)
  * \see parseDeclarationStmtNode(Token ***)
  * \see parseIfThenElseStmtNode(Token ***)
  * \see parseSwitchStmtNode(Token ***)
  * \see parseBreakStmtNode(Token ***)
  * \see parseReturnStmtNode(Token ***)
  * \see parseDeallocationStmtNode(Token ***)
  * \see parseFuncDefStmtNode(Token ***) */
StmtNode *parseLoopStmtNode(Token ***tokenp)
{
	IdentifierNode *name1 = NULL;
	IdentifierNode *var = NULL;
	ExprNode *update = NULL;
	ExprNode *guard = NULL;
	BlockNode *body = NULL;
	FuncDefStmtNode *def = NULL;
	IdentifierNode *name2 = NULL;
	LoopStmtNode *stmt = NULL;
	ExprNodeList *args = NULL;

	/* For increment and decrement loops */
	IdentifierNode *varcopy = NULL;
	ExprNode *arg1 = NULL;
	ConstantNode *one = NULL;
	ExprNode *arg2 = NULL;
	OpExprNode *op = NULL;

	/* For function loops */
	IdentifierNode *scope = NULL;
	IdentifierNode *name = NULL;
	FuncCallExprNode *node = NULL;
	ExprNode *arg = NULL;

	/* For loop predicates */
	ExprNode *predarg = NULL;
	ExprNodeList *predargs = NULL;
	OpExprNode *predop = NULL;

	StmtNode *ret = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	debug("ST_LOOP");
#endif

	/* Remove the loop-start keyword from the token stream */
	status = acceptToken(&tokens, TT_IMINYR);
	if (!status) {
		error("expected IM IN YR", tokens);
		goto parseLoopStmtNodeAbort;
	}

	/* Parse the loop name */
	name1 = parseIdentifierNode(&tokens);
	if (!name1) goto parseLoopStmtNodeAbort;

	/* Check if an increment or decrement loop */
	if (peekToken(&tokens, TT_UPPIN) || peekToken(&tokens, TT_NERFIN)) {
		OpType type;

		/* Parse the increment token or decrement token */
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
			goto parseLoopStmtNodeAbort;
		}

		/* Parse the required syntax */
		if (!acceptToken(&tokens, TT_YR)) {
			error("expected YR", tokens);
			goto parseLoopStmtNodeAbort;
		}

		/* Parse the variable to operate on */
		var = parseIdentifierNode(&tokens);
		if (!var) goto parseLoopStmtNodeAbort;
#ifdef DEBUG
		shiftout();
		debug("ET_CONSTANT (CT_INTEGER)");
		shiftin();
		shiftin();
#endif
		/* Make a copy of the variable for use as a function argument */
		varcopy = createIdentifierNode(var->image, var->fname, var->line);
		if (!varcopy) goto parseLoopStmtNodeAbort;

		/* Package the variable into an identifier expression */
		arg1 = createExprNode(ET_IDENTIFIER, varcopy);
		if (!arg1) goto parseLoopStmtNodeAbort;

		/* Create a constant integer "1" */
		one = createIntegerConstantNode(1);
		if (!one) goto parseLoopStmtNodeAbort;

		/* Package the constant into an expression */
		arg2 = createExprNode(ET_CONSTANT, one);
		if (!arg2) goto parseLoopStmtNodeAbort;

		/* Create a list of arguments */
		args = createExprNodeList();
		if (!args) goto parseLoopStmtNodeAbort;

		/* Add the packaged arguments to the argument list */
		status = addExprNode(args, arg1);
		if (!status) goto parseLoopStmtNodeAbort;
		arg1 = NULL;
		status = addExprNode(args, arg2);
		if (!status) goto parseLoopStmtNodeAbort;
		arg2 = NULL;

		/* Package the arguments and operation type in an expression */
		op = createOpExprNode(type, args);
		if (!op) goto parseLoopStmtNodeAbort;

		/* Create the update expression */
		update = createExprNode(ET_OP, op);
		if (!update) goto parseLoopStmtNodeAbort;
	}
	/* Check if a function loop */
	else if (nextToken(&tokens, TT_IZ)) {
		IdentifierNode *temp = NULL;
#ifdef DEBUG
		debug("ET_FUNCCALL");
#endif
		/* Parse the function scope */
		scope = parseIdentifierNode(&tokens);
		if (!scope) goto parseLoopStmtNodeAbort;

		/* Parse the function indicator */
		status = acceptToken(&tokens, TT_IZ);
		if (!status) {
			error("expected IZ", tokens);
			goto parseLoopStmtNodeAbort;
		}

		/* Parse the function name */
		name = parseIdentifierNode(&tokens);
		if (!name) goto parseLoopStmtNodeAbort;

		/* Create a list of arguments */
		args = createExprNodeList();
		if (!args) goto parseLoopStmtNodeAbort;

		/* Check for unary function */
		status = acceptToken(&tokens, TT_YR);
		if (!status) {
			error("expected unary function", tokens);
			goto parseLoopStmtNodeAbort;
		}

		/* Parse the unary function's single argument */
		arg = parseExprNode(&tokens);
		if (!arg) goto parseLoopStmtNodeAbort;

		/* Make sure the argument is an identifier */
		if (arg->type != ET_IDENTIFIER) {
			error("expected identifier", tokens);
			goto parseLoopStmtNodeAbort;
		}

		/* Add the argument to the argument list */
		status = addExprNode(args, arg);
		if (!status) goto parseLoopStmtNodeAbort;
		/* (Save a pointer to its expression for use, below) */
		temp = (IdentifierNode *)(arg->expr);
		arg = NULL;

		/* Copy the identifier to make it the loop variable */
		var = createIdentifierNode(temp->image, temp->fname, temp->line);
		if (!var) goto parseLoopStmtNodeAbort;

		/* Check for unary function */
		status = acceptToken(&tokens, TT_MKAY);
		if (!status) {
			error("expected MKAY", tokens);
			goto parseLoopStmtNodeAbort;
		}

		/* Create a function call expression */
		node = createFuncCallExprNode(scope, name, args);
		if (!node) goto parseLoopStmtNodeAbort;

		/* Package the function call in an expression */
		update = createExprNode(ET_FUNCCALL, node);
		if (!update) goto parseLoopStmtNodeAbort;
	}

	/* If there is an update, parse any predicates */
	if (update) {
		/* Check for a while token */
		if (acceptToken(&tokens, TT_WILE)) {
			/* Parse the while predicate */
			guard = parseExprNode(&tokens);
			if (!guard) goto parseLoopStmtNodeAbort;
		}
		/* Check for a until token */
		else if (acceptToken(&tokens, TT_TIL)) {
#ifdef DEBUG
			shiftout();
			debug("ET_OP (OP_NOT)");
#endif
			/* Parse the until predicate */
			predarg = parseExprNode(&tokens);
			if (!predarg) goto parseLoopStmtNodeAbort;

			/* Create a list for predicate arguments */
			predargs = createExprNodeList();
			if (!predargs) goto parseLoopStmtNodeAbort;

			/* Add the until predicate to the list */
			status = addExprNode(predargs, predarg);
			if (!status) goto parseLoopStmtNodeAbort;
			predarg = NULL;
#ifdef DEBUG
			shiftin();
#endif
			/* Create a NOT operation with the predicate */
			predop = createOpExprNode(OP_NOT, predargs);
			if (!predop) goto parseLoopStmtNodeAbort;

			/* Package the NOT operation in an expression */
			guard = createExprNode(ET_OP, predop);
			if (!guard) goto parseLoopStmtNodeAbort;
		}
	}

	/* All of the above should be on its own line */
	status = acceptToken(&tokens, TT_NEWLINE);
	if (!status) {
		error("expected end of expression", tokens);
		goto parseLoopStmtNodeAbort;
	}

	/* Parse the body of the loop */
	body = parseBlockNode(&tokens);
	if (!body) goto parseLoopStmtNodeAbort;

	/* Parse the end-loop keywords */
	status = acceptToken(&tokens, TT_IMOUTTAYR);
	if (!status) {
		error("expected IM OUTTA YR", tokens);
		goto parseLoopStmtNodeAbort;
	}

	/* Parse the end-of-loop name */
	name2 = parseIdentifierNode(&tokens);
	if (!name2) goto parseLoopStmtNodeAbort;

	/* Make sure the beginning-of-loop and end-of-loop names match */
	if (strcmp(name1->image, name2->image)) {
		error("expected matching loop name", tokens);
		goto parseLoopStmtNodeAbort;
	}

	/* We no longer need the end-of-loop name */
	deleteIdentifierNode(name2);

	/* The end-of-loop structure should appear on its own line */
	status = acceptToken(&tokens, TT_NEWLINE);
	if (!status) {
		error("expected end of expression", tokens);
		goto parseLoopStmtNodeAbort;
	}

	/* Create the new LoopStmtNode structure */
	stmt = createLoopStmtNode(name1, var, guard, update, body);
	if (!stmt) goto parseLoopStmtNodeAbort;

	/* Create the new StmtNode structure */
	ret = createStmtNode(ST_LOOP, stmt);
	if (!ret) goto parseLoopStmtNodeAbort;

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parseLoopStmtNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (ret) deleteStmtNode(ret);
	else if (stmt) deleteLoopStmtNode(stmt);
	else {
		if (name2) deleteIdentifierNode(name2);
		if (def) deleteFuncDefStmtNode(def);
		if (body) deleteBlockNode(body);
		if (guard) deleteExprNode(guard);
		if (update) deleteExprNode(update);
		if (var) deleteIdentifierNode(var);
		if (name1) deleteIdentifierNode(name1);

		/* For increment and decrement loops */
		if (op) deleteOpExprNode(op);
		if (args) deleteExprNodeList(args);
		if (arg2) deleteExprNode(arg2);
		if (one) deleteConstantNode(one);
		if (arg1) deleteExprNode(arg1);
		if (varcopy) deleteIdentifierNode(varcopy);

		/* For function loops */
		if (arg) deleteExprNode(arg);
		if (node) deleteFuncCallExprNode(node);
		if (name) deleteIdentifierNode(name);
		if (scope) deleteIdentifierNode(scope);

		/* For loop predicates */
		if (predarg) deleteExprNode(predarg);
		if (predargs) deleteExprNodeList(predargs);
		if (predop) deleteOpExprNode(predop);
	}
	return NULL;
}

/** Parses a set of tokens into a StmtNode structure containing a
  * DeallocationStmtNode structure.  Parsing begins at the token pointed to by
  * \a tokenp.
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
  * \see parseCastStmtNode(Token ***)
  * \see parsePrintStmtNode(Token ***)
  * \see parseInputStmtNode(Token ***)
  * \see parseAssignmentStmtNode(Token ***)
  * \see parseDeclarationStmtNode(Token ***)
  * \see parseIfThenElseStmtNode(Token ***)
  * \see parseSwitchStmtNode(Token ***)
  * \see parseBreakStmtNode(Token ***)
  * \see parseReturnStmtNode(Token ***)
  * \see parseLoopStmtNode(Token ***)
  * \see parseFuncDefStmtNode(Token ***) */
StmtNode *parseDeallocationStmtNode(Token ***tokenp)
{
	IdentifierNode *target = NULL;
	DeallocationStmtNode *stmt = NULL;
	StmtNode *ret = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	debug("ST_DEALLOCATION");
#endif

	/* Parse the variable to deallocate */
	target = parseIdentifierNode(&tokens);
	if (!target) goto parseDeallocationStmtNodeAbort;

	/* Parse the deallocation token */
	status = acceptToken(&tokens, TT_RNOOB);
	if (!status) {
		error("expected R NOOB", tokens);
		goto parseDeallocationStmtNodeAbort;
	}

	/* The deallocation statement must reside on its own line */
	status = acceptToken(&tokens, TT_NEWLINE);
	if (!status) {
		error("expected end of statement", tokens);
		goto parseDeallocationStmtNodeAbort;
	}

	/* Create the new DeallocationStmtNode structure */
	stmt = createDeallocationStmtNode(target);
	if (!stmt) goto parseDeallocationStmtNodeAbort;

	/* Create the new StmtNode structure */
	ret = createStmtNode(ST_DEALLOCATION, stmt);
	if (!ret) goto parseDeallocationStmtNodeAbort;

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parseDeallocationStmtNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (ret) deleteStmtNode(ret);
	else if (stmt) deleteDeallocationStmtNode(stmt);
	else {
		if (target) deleteIdentifierNode(target);
	}
}

/** Parses a set of tokens into a StmtNode structure containing a
  * FuncDefStmtNode structure.  Parsing begins at the token pointed to by
  * \a tokenp.
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
  * \see parseCastStmtNode(Token ***)
  * \see parsePrintStmtNode(Token ***)
  * \see parseInputStmtNode(Token ***)
  * \see parseAssignmentStmtNode(Token ***)
  * \see parseDeclarationStmtNode(Token ***)
  * \see parseIfThenElseStmtNode(Token ***)
  * \see parseSwitchStmtNode(Token ***)
  * \see parseBreakStmtNode(Token ***)
  * \see parseReturnStmtNode(Token ***)
  * \see parseLoopStmtNode(Token ***)
  * \see parseDeallocationStmtNode(Token ***) */
StmtNode *parseFuncDefStmtNode(Token ***tokenp)
{
	IdentifierNode *scope = NULL;
	IdentifierNode *name = NULL;
	IdentifierNodeList *args = NULL;
	IdentifierNode *arg = NULL;
	BlockNode *body = NULL;
	FuncDefStmtNode *stmt = NULL;
	StmtNode *ret = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	debug("ST_FUNCDEF");
#endif

	/* Parse the function definition token */
	status = acceptToken(&tokens, TT_HOWIZ);
	if (!status) {
		error("expected HOW IZ", tokens);
		goto parseFuncDefStmtNodeAbort;
	}

	/* Parse the scope to define the function in */
	scope = parseIdentifierNode(&tokens);
	if (!scope) goto parseFuncDefStmtNodeAbort;

	/* Parse the name of the function */
	name = parseIdentifierNode(&tokens);
	if (!name) goto parseFuncDefStmtNodeAbort;

	/* Create a list of arguments */
	args = createIdentifierNodeList();
	if (!args) goto parseFuncDefStmtNodeAbort;

	/* Parse the first argument indicator */
	if (acceptToken(&tokens, TT_YR)) {
		/* Parse the first argument name */
		arg = parseIdentifierNode(&tokens);
		if (!arg) goto parseFuncDefStmtNodeAbort;

		/* Add the first argument to the arguments list */
		status = addIdentifierNode(args, arg);
		if (!status) goto parseFuncDefStmtNodeAbort;
		arg = NULL;

		/* Continue parsing argument indicators */
		while (acceptToken(&tokens, TT_ANYR)) {
			/* Parse the argument */
			arg = parseIdentifierNode(&tokens);
			if (!arg) goto parseFuncDefStmtNodeAbort;

			/* Add the argument to the arguments list */
			status = addIdentifierNode(args, arg);
			if (!status) goto parseFuncDefStmtNodeAbort;
			arg = NULL;
		}
	}

	/* The function declaration should appear on its own line */
	status = acceptToken(&tokens, TT_NEWLINE);
	if (!status) {
		error("expected end of statement", tokens);
		goto parseFuncDefStmtNodeAbort;
	}

	/* Parse the body of the function */
	body = parseBlockNode(&tokens);
	if (!body) goto parseFuncDefStmtNodeAbort;

	/* Parse the end-function token */
	status = acceptToken(&tokens, TT_IFUSAYSO);
	if (!status) {
		error("expected IF YOU SAY SO", tokens);
		goto parseFuncDefStmtNodeAbort;
	}

	/* The end-function token should appear on its own line */
	status = acceptToken(&tokens, TT_NEWLINE);
	if (!status) {
		error("expected end of statement", tokens);
		goto parseFuncDefStmtNodeAbort;
	}

	/* Create the new FuncDefStmtNode structure */
	stmt = createFuncDefStmtNode(scope, name, args, body);
	if (!stmt) goto parseFuncDefStmtNodeAbort;

	/* Create the new StmtNode structure */
	ret = createStmtNode(ST_FUNCDEF, stmt);
	if (!ret) goto parseFuncDefStmtNodeAbort;

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return ret;

parseFuncDefStmtNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (ret) deleteStmtNode(ret);
	else if (stmt) deleteFuncDefStmtNode(stmt);
	else {
		if (body) deleteBlockNode(body);
		if (args) deleteIdentifierNodeList(args);
		if (arg) deleteIdentifierNode(arg);
		if (name) deleteIdentifierNode(name);
		if (scope) deleteIdentifierNode(scope);
	}
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
  * \see parseExprNode(Token ***)
  * \see parseBlockNode(Token ***)
  * \see parseMainNode(Token **) */
StmtNode *parseStmtNode(Token ***tokenp) /**< [in,out] A pointer to the position of the next token to parse. */
{
	StmtNode *ret = NULL;
	ExprNode *expr = NULL;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	shiftout();
#endif

	/* Casting */
	if (nextToken(&tokens, TT_ISNOWA)) {
		ret = parseCastStmtNode(tokenp);
	}
	/* Print */
	else if (peekToken(&tokens, TT_VISIBLE)) {
		ret = parsePrintStmtNode(tokenp);
	}
	/* Input */
	else if (peekToken(&tokens, TT_GIMMEH)) {
		ret = parseInputStmtNode(tokenp);
	}
	/* Assignment */
	else if (nextToken(&tokens, TT_R)) {
		ret = parseAssignmentStmtNode(tokenp);
	}
	/* Variable declaration */
	else if (nextToken(&tokens, TT_HASA)) {
		ret = parseDeclarationStmtNode(tokenp);
	}
	/* If/then/else */
	else if (peekToken(&tokens, TT_ORLY)) {
		ret = parseIfThenElseStmtNode(tokenp);
	}
	/* Switch */
	else if (peekToken(&tokens, TT_WTF)) {
		ret = parseSwitchStmtNode(tokenp);
	}
	/* Break */
	else if (peekToken(&tokens, TT_GTFO)) {
		ret = parseBreakStmtNode(tokenp);
	}
	/* Return */
	else if (peekToken(&tokens, TT_FOUNDYR)) {
		ret = parseReturnStmtNode(tokenp);
	}
	/* Loop */
	else if (peekToken(&tokens, TT_IMINYR)) {
		ret = parseLoopStmtNode(tokenp);
	}
	/* Deallocation */
	else if (nextToken(&tokens, TT_RNOOB)) {
		ret = parseDeallocationStmtNode(tokenp);
	}
	/* Function definition */
	else if (peekToken(&tokens, TT_HOWIZ)) {
		ret = parseFuncDefStmtNode(tokenp);
	}
	/* Expression evaluation */
	else if ((expr = parseExprNode(&tokens))) {
		int status;

#ifdef DEBUG
		debug("ST_EXPR");
#endif

		/* The expression should appear on its own line */
		status = acceptToken(&tokens, TT_NEWLINE);
		if (!status) {
			error("expected end of expression", tokens);
			deleteExprNode(expr);
			return NULL;
		}

		ret = createStmtNode(ST_EXPR, expr);
		if (!ret) {
			deleteExprNode(expr);
			return NULL;
		}

		/* Since we're successful, update the token stream */
		*tokenp = tokens;
	}
	else {
		error("expected statement", tokens);
	}

#ifdef DEBUG
	shiftin();
#endif

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
  * \see parseExprNode(Token ***)
  * \see parseStmtNode(Token ***)
  * \see parseMainNode(Token **) */
BlockNode *parseBlockNode(Token ***tokenp) /**< [in,out] A pointer to the position of the next token to parse. */
{
	StmtNodeList *stmts = NULL;
	StmtNode *stmt = NULL;
	BlockNode *block = NULL;
	int status;

	/* Work from a copy of the token stream in case something goes wrong */
	Token **tokens = *tokenp;

#ifdef DEBUG
	shiftout();
	debug(">ET_BLOCK");
#endif

	/* Create a list of statements */
	stmts = createStmtNodeList();
	if (!stmts) goto parseBlockNodeAbort;
	while (!peekToken(&tokens, TT_EOF)
			&& !peekToken(&tokens, TT_KTHXBYE)
			&& !peekToken(&tokens, TT_OIC)
			&& !peekToken(&tokens, TT_YARLY)
			&& !peekToken(&tokens, TT_NOWAI)
			&& !peekToken(&tokens, TT_MEBBE)
			&& !peekToken(&tokens, TT_OMG)
			&& !peekToken(&tokens, TT_OMGWTF)
			&& !peekToken(&tokens, TT_IMOUTTAYR)
			&& !peekToken(&tokens, TT_IFUSAYSO)) {
		/* Parse the next statement */
		stmt = parseStmtNode(&tokens);
		if (!stmt) goto parseBlockNodeAbort;

		/* Add the statement to the list */
		status = addStmtNode(stmts, stmt);
		if (!status) goto parseBlockNodeAbort;
		stmt = NULL;
	}

#ifdef DEBUG
	debug("<ET_BLOCK");
	shiftin();
#endif

	/* Create the BlockNode structure */
	block = createBlockNode(stmts);
	if (!block) goto parseBlockNodeAbort;

	/* Since we're successful, update the token stream */
	*tokenp = tokens;

	return block;

parseBlockNodeAbort: /* Exception handling */

	/* Clean up any allocated structures */
	if (block) deleteBlockNode(block);
	else {
		if (stmt) deleteStmtNode(stmt);
		if (stmts) deleteStmtNodeList(stmts);
	}
	return NULL;
}

/** Parses \a tokens into a MainNode structure, an intermediary form to be
  * passed along to a later stage of processing such as, for example, an
  * interpreter.
  *
  * \pre \a tokens was created by tokenizeLexemes(Lexeme **).
  *
  * \return A pointer to the generated MainNode structure.
  *
  * \retval NULL An error occurred during parsing.
  *
  * \see parseConstantNode(Token ***)
  * \see parseTypeNode(Token ***)
  * \see parseIdentifierNode(Token ***)
  * \see parseExprNode(Token ***)
  * \see parseStmtNode(Token ***)
  * \see parseBlockNode(Token ***) */
MainNode *parseMainNode(Token **tokens) /**< [in] A pointer to an array of tokens to parse. */
{
	BlockNode *block = NULL;
	MainNode *_main = NULL;
	int status;

	/* All programs must start with the HAI token */
	status = acceptToken(&tokens, TT_HAI);
	if (!status) {
		error("expected HAI", tokens);
		goto parseMainNodeAbort;
	}

	/* Accept any version */
	tokens++;

#ifdef DEBUG
	debug("ET_MAINBLOCK");
#endif

	/* Make sure the header line ends with a newline */
	status = acceptToken(&tokens, TT_NEWLINE);
	if (!status) {
		error("expected end of statement", tokens);
		goto parseMainNodeAbort;
	}

	/* Parse the main block of code */
	block = parseBlockNode(&tokens);
	if (!block) goto parseMainNodeAbort;

	/* Make sure the program ends with KTHXBYE */
	status = acceptToken(&tokens, TT_KTHXBYE);
	if (!status) {
		error("expected KTHXBYE", tokens);
		goto parseMainNodeAbort;
	}

	/* Create the MainBlockNode structure */
	_main = createMainNode(block);
	if (!_main) goto parseMainNodeAbort;

	return _main;

parseMainNodeAbort: /* In case something goes wrong... */

	/* Clean up any allocated structures */
	if (_main) deleteMainNode(_main);
	else if (block) deleteBlockNode(block);

	return NULL;
}
