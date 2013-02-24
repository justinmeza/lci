#include "binding.h"

struct returnobject;
struct scopeobject;

struct returnobject *fopenWrapper(struct scopeobject *scope)
{
	IdentifierNode *id1 = createIdentifierNode(IT_DIRECT, (void *)copyString("filename"), NULL, NULL, 0);
	ValueObject *val1 = getScopeValueLocal(scope, scope, id1);
	IdentifierNode *id2 = createIdentifierNode(IT_DIRECT, (void *)copyString("mode"), NULL, NULL, 0);
	ValueObject *val2 = getScopeValueLocal(scope, scope, id2);
	char *filename = getString(val1);
	char *mode = getString(val2);

	FILE *f = fopen(filename, mode);
	ValueObject *ret = createBlobValueObject(f);
	return createReturnObject(RT_RETURN, ret);
}

struct returnobject *freadWrapper(struct scopeobject *scope)
{
	IdentifierNode *id1 = createIdentifierNode(IT_DIRECT, (void *)copyString("file"), NULL, NULL, 0);
	ValueObject *val1 = getScopeValueLocal(scope, scope, id1);
	IdentifierNode *id2 = createIdentifierNode(IT_DIRECT, (void *)copyString("length"), NULL, NULL, 0);
	ValueObject *val2 = getScopeValueLocal(scope, scope, id2);
	FILE *file = (FILE *)getBlob(val1);
	int length = getInteger(val2);

	char *buf = malloc((sizeof(char) * length) + 1);
	fread(buf, 1, length, file);
	buf[length] = '\0';

	ValueObject *ret = createStringValueObject(buf);
	return createReturnObject(RT_RETURN, ret);
}

struct returnobject *fcloseWrapper(struct scopeobject *scope)
{
	IdentifierNode *id1 = createIdentifierNode(IT_DIRECT, (void *)copyString("file"), NULL, NULL, 0);
	ValueObject *val1 = getScopeValueLocal(scope, scope, id1);
	FILE *file = (FILE *)getBlob(val1);

	fclose(file);
	return createReturnObject(RT_DEFAULT, NULL);
}

void loadBinding(ScopeObject *scope)
{
	/* TODO: add error checking and handling */

	/* stdio */
	ScopeObject *stdio = createScopeObject(scope);
	IdentifierNode* name_stdio = createIdentifierNode(IT_DIRECT, (void *)copyString("STDIO"), NULL, NULL, 0);
	createScopeValue(scope, scope, name_stdio);
	ValueObject *val_stdio = createArrayValueObject(stdio);
	updateScopeValue(scope, scope, name_stdio, val_stdio);

	/* fopen */
	{
		IdentifierNode* name = createIdentifierNode(IT_DIRECT, (void *)copyString("FOPENIN"), NULL, NULL, 0);
		IdentifierNodeList *args = createIdentifierNodeList();
		IdentifierNode *filename = createIdentifierNode(IT_DIRECT, (void *)copyString("filename"), NULL, NULL, 0);
		IdentifierNode *mode = createIdentifierNode(IT_DIRECT, (void *)copyString("mode"), NULL, NULL, 0);
		addIdentifierNode(args, filename);
		addIdentifierNode(args, mode);
		StmtNodeList *stmts = createStmtNodeList();
		BindingStmtNode *binding = createBindingStmtNode(&fopenWrapper);
		StmtNode *wrapper = createStmtNode(ST_BINDING, binding);
		addStmtNode(stmts, wrapper);
		BlockNode *body = createBlockNode(stmts);
		/* TODO: should this first parameter be NULL? */
		FuncDefStmtNode *interface = createFuncDefStmtNode(NULL, name, args, body);
		ValueObject *val = createFunctionValueObject(interface);
		createScopeValue(stdio, stdio, name);
		updateScopeValue(stdio, stdio, name, val);
	}

	/* fread */
	{
		IdentifierNode* name = createIdentifierNode(IT_DIRECT, (void *)copyString("FREADIN"), NULL, NULL, 0);
		IdentifierNodeList *args = createIdentifierNodeList();
		IdentifierNode *filename = createIdentifierNode(IT_DIRECT, (void *)copyString("file"), NULL, NULL, 0);
		IdentifierNode *mode = createIdentifierNode(IT_DIRECT, (void *)copyString("length"), NULL, NULL, 0);
		addIdentifierNode(args, filename);
		addIdentifierNode(args, mode);
		StmtNodeList *stmts = createStmtNodeList();
		BindingStmtNode *binding = createBindingStmtNode(&freadWrapper);
		StmtNode *wrapper = createStmtNode(ST_BINDING, binding);
		addStmtNode(stmts, wrapper);
		BlockNode *body = createBlockNode(stmts);
		/* TODO: should this first parameter be NULL? */
		FuncDefStmtNode *interface = createFuncDefStmtNode(NULL, name, args, body);
		ValueObject *val = createFunctionValueObject(interface);
		createScopeValue(stdio, stdio, name);
		updateScopeValue(stdio, stdio, name, val);
	}

	/* fclose */
	{
		IdentifierNode* name = createIdentifierNode(IT_DIRECT, (void *)copyString("FCLOSIN"), NULL, NULL, 0);
		IdentifierNodeList *args = createIdentifierNodeList();
		IdentifierNode *filename = createIdentifierNode(IT_DIRECT, (void *)copyString("file"), NULL, NULL, 0);
		addIdentifierNode(args, filename);
		StmtNodeList *stmts = createStmtNodeList();
		BindingStmtNode *binding = createBindingStmtNode(&fcloseWrapper);
		StmtNode *wrapper = createStmtNode(ST_BINDING, binding);
		addStmtNode(stmts, wrapper);
		BlockNode *body = createBlockNode(stmts);
		FuncDefStmtNode *interface = createFuncDefStmtNode(NULL, name, args, body);
		ValueObject *val = createFunctionValueObject(interface);
		createScopeValue(stdio, stdio, name);
		updateScopeValue(stdio, stdio, name, val);
	}

	return;

loadBindingAbort: /* Exception handling */

	return;
}
