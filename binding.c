#include "binding.h"

struct returnobject;
struct scopeobject;

ValueObject *getArg(struct scopeobject *scope, char *name)
{
	IdentifierNode *id = createIdentifierNode(IT_DIRECT, (void *)copyString(name), NULL, NULL, 0);
	ValueObject *val = getScopeValueLocal(scope, scope, id);
	deleteIdentifierNode(id);
	return val;
}

struct returnobject *fopenWrapper(struct scopeobject *scope)
{
	ValueObject *arg1 = getArg(scope, "filename");
	ValueObject *arg2 = getArg(scope, "mode");
	char *filename = getString(arg1);
	char *mode = getString(arg2);

	FILE *f = fopen(filename, mode);

	ValueObject *ret = createBlobValueObject(f);
	return createReturnObject(RT_RETURN, ret);
}

struct returnobject *freadWrapper(struct scopeobject *scope)
{
	ValueObject *arg1 = getArg(scope, "file");
	ValueObject *arg2 = getArg(scope, "length");
	FILE *file = (FILE *)getBlob(arg1);
	int length = getInteger(arg2);

	char *buf = malloc((sizeof(char) * length) + 1);
	fread(buf, 1, length, file);
	buf[length] = '\0';

	ValueObject *ret = createStringValueObject(buf);
	return createReturnObject(RT_RETURN, ret);
}

struct returnobject *fcloseWrapper(struct scopeobject *scope)
{
	ValueObject *arg1 = getArg(scope, "file");
	FILE *file = (FILE *)getBlob(arg1);

	fclose(file);
	deleteValueObject(arg1);

	return createReturnObject(RT_DEFAULT, NULL);
}

void loadLibrary(ScopeObject *scope, IdentifierNode *name)
{
	/* TODO: make this select which library to load */

	/* stdio */
	ScopeObject *lib = createScopeObject(scope);
	IdentifierNode* id = createIdentifierNode(IT_DIRECT, (void *)copyString("STDIO"), NULL, NULL, 0);
	createScopeValue(scope, scope, id);
	ValueObject *val = createArrayValueObject(lib);
	updateScopeValue(scope, scope, id, val);
	loadBinding(lib, "FOPENIN", "filename mode", &fopenWrapper);
	loadBinding(lib, "FREADIN", "file length", &freadWrapper);
	loadBinding(lib, "FCLOSIN", "file", &fcloseWrapper);
}

void loadBinding(ScopeObject *scope, char *name, const char *args, struct returnobject *(*binding)(struct scopeobject *))
{
	/* TODO: add error checking and handling */

	IdentifierNode* id = createIdentifierNode(IT_DIRECT, (void *)copyString(name), NULL, NULL, 0);
	StmtNodeList *stmts = createStmtNodeList();
	BindingStmtNode *stmt = createBindingStmtNode(binding);
	StmtNode *wrapper = createStmtNode(ST_BINDING, stmt);
	addStmtNode(stmts, wrapper);
	BlockNode *body = createBlockNode(stmts);

	IdentifierNodeList *ids = createIdentifierNodeList();
	const char *start = args;
	while (start != NULL) {
		char *end = strchr(start, ' ');
		char *temp = NULL;
		unsigned int len = 0;
		if (end != NULL) len = (end - start);
		else len = strlen(start);
		temp = malloc(sizeof(char) * (len + 1));
		strncpy(temp, start, len);
		temp[len] = '\0';
		if (end != NULL) start = (end + 1);
		else start = NULL;
		IdentifierNode *arg = createIdentifierNode(IT_DIRECT, (void *)temp, NULL, NULL, 0);
		addIdentifierNode(ids, arg);
	}

	FuncDefStmtNode *interface = createFuncDefStmtNode(NULL, id, ids, body);
	ValueObject *val = createFunctionValueObject(interface);
	createScopeValue(scope, scope, id);
	updateScopeValue(scope, scope, id, val);
	return;

	/* fopen */
	/*
	{
		IdentifierNode* name = createIdentifierNode(IT_DIRECT, (void *)copyString("FOPENIN"), NULL, NULL, 0);
		StmtNodeList *stmts = createStmtNodeList();
		BindingStmtNode *binding = createBindingStmtNode(&fopenWrapper);
		StmtNode *wrapper = createStmtNode(ST_BINDING, binding);
		addStmtNode(stmts, wrapper);
		BlockNode *body = createBlockNode(stmts);

		FuncDefStmtNode *interface = createFuncDefStmtNode(NULL, name, args, body);
		ValueObject *val = createFunctionValueObject(interface);
		createScopeValue(stdio, stdio, name);
		updateScopeValue(stdio, stdio, name, val);
	}
	*/

	/* fread */
	/*
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
		FuncDefStmtNode *interface = createFuncDefStmtNode(NULL, name, args, body);
		ValueObject *val = createFunctionValueObject(interface);
		createScopeValue(stdio, stdio, name);
		updateScopeValue(stdio, stdio, name, val);
	}
	*/

	/* fclose */
	/*
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
	*/

	return;

loadBindingAbort: /* Exception handling */

	return;
}
