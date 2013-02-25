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

struct returnobject *fwriteWrapper(struct scopeobject *scope)
{
	ValueObject *arg1 = getArg(scope, "file");
	ValueObject *arg2 = getArg(scope, "data");
	FILE *file = (FILE *)getBlob(arg1);
	char *data = getString(arg2);

	fwrite(data, 1, strlen(data), file);

	return createReturnObject(RT_DEFAULT, NULL);
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
	loadBinding(lib, "FWRITIN", "file data", &fwriteWrapper);
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

loadBindingAbort: /* Exception handling */

	return;
}
