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

void loadLibrary(ScopeObject *scope, IdentifierNode *target)
{
	char *name = NULL;
	int status;
	ScopeObject *lib = NULL;
	IdentifierNode *id = NULL;
	ValueObject *val = NULL;
	if (target == NULL) return;

	name = resolveIdentifierName(target, scope);
	if (!name) goto loadLibraryAbort;

	if (!strcmp(name, "STDIO")) {
		lib = createScopeObject(scope);
		if (!lib) goto loadLibraryAbort;

		loadBinding(lib, "FOPENIN", "filename mode", &fopenWrapper);
		loadBinding(lib, "FREADIN", "file length", &freadWrapper);
		loadBinding(lib, "FWRITIN", "file data", &fwriteWrapper);
		loadBinding(lib, "FCLOSIN", "file", &fcloseWrapper);

		id = createIdentifierNode(IT_DIRECT, (void *)copyString("STDIO"), NULL, NULL, 0);
		if (!id) goto loadLibraryAbort;

		if (!createScopeValue(scope, scope, id)) goto loadLibraryAbort;

		val = createArrayValueObject(lib);
		if (!val) goto loadLibraryAbort;
		lib = NULL;

		if (!updateScopeValue(scope, scope, id, val)) goto loadLibraryAbort;
	}

	if (name) free(name);
	return;

loadLibraryAbort: /* In case something goes wrong... */

	/* Clean up any allocated structures */
	if (name) free(name);
	if (lib) deleteScopeObject(lib);
	if (id) deleteIdentifierNode(id);
	if (val) deleteValueObject(val);
	return;
}

void loadBinding(ScopeObject *scope, char *name, const char *args, struct returnobject *(*binding)(struct scopeobject *))
{
	IdentifierNode *id = NULL;
	StmtNodeList *stmts = NULL;
	BindingStmtNode *stmt = NULL;
	StmtNode *wrapper = NULL;
	int status;
	BlockNode *body = NULL;
	IdentifierNodeList *ids = NULL;
	IdentifierNode *arg = NULL;
	if (name == NULL || binding == NULL) return;

	id = createIdentifierNode(IT_DIRECT, (void *)copyString(name), NULL, NULL, 0);
	if (!id) goto loadBindingAbort;

	stmts = createStmtNodeList();
	if (!stmts) goto loadBindingAbort;

	stmt = createBindingStmtNode(binding);
	if (!stmt) goto loadBindingAbort;

	wrapper = createStmtNode(ST_BINDING, stmt);
	if (!wrapper) goto loadBindingAbort;
	stmt = NULL;

	status = addStmtNode(stmts, wrapper);
	if (!status) goto loadBindingAbort;
	wrapper = NULL;

	body = createBlockNode(stmts);
	if (!body) goto loadBindingAbort;
	stmts = NULL;

	ids = createIdentifierNodeList();
	if (!ids) goto loadBindingAbort;

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

		arg = createIdentifierNode(IT_DIRECT, (void *)temp, NULL, NULL, 0);
		if (!arg) goto loadBindingAbort;

		status = addIdentifierNode(ids, arg);
		if (!status) goto loadBindingAbort;
	}

	FuncDefStmtNode *interface = createFuncDefStmtNode(NULL, id, ids, body);
	if (!interface) goto loadBindingAbort;

	ValueObject *val = createFunctionValueObject(interface);
	if (!val) goto loadBindingAbort;

	createScopeValue(scope, scope, id);
	updateScopeValue(scope, scope, id, val);

	return;

loadBindingAbort: /* In case something goes wrong... */

	if (id) deleteIdentifierNode(id);
	if (val) deleteValueObject(val);
	else if (interface) deleteFuncDefStmtNode(interface);
	else {
		if (arg) deleteIdentifierNode(arg);
		if (ids) deleteIdentifierNodeList(ids);
		if (body) deleteBlockNode(body);
		if (stmts) deleteStmtNodeList(stmts);
		if (wrapper) deleteStmtNode(wrapper);
		if (stmt) deleteBindingStmtNode(stmt);
	}
	return;
}
