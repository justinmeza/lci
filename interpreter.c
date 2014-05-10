#include "interpreter.h"

/**
 * Creates a new string by copying the contents of another string.
 *
 * \param [in] data The string to copy.
 *
 * \return A new string whose contents is a copy of \a data.
 *
 * \retval NULL Memory allocation failed.
 */
char *copyString(char *data)
{
	char *p = malloc(sizeof(char) * (strlen(data) + 1));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	strcpy(p, data);
	return p;
}

/**
 * Checks if a string follows the format of a decimal number.
 *
 * \param [in] data The string to check the format of.
 *
 * \retval 0 The string is not a decimal number.
 *
 * \retval 1 The string is a decimal number.
 */
unsigned int isDecString(const char *data)
{
	size_t n;
	size_t len = strlen(data);

	/* Check for an empty string */
	if (len == 0) return 0;

	/* Check for non-digit, non-hyphen, and non-period characters */
	for (n = 0; n < len; n++) {
		if (!isdigit(data[n])
				&& data[n] != '.'
				&& data[n] != '-')
			return 0;
	}

	return 1;
}

/**
 * Checks if a string follows the format of a hexadecimal number.
 *
 * \param [in] data The characters to check the format of.
 *
 * \retval 0 The string is not a hexadecimal number.
 *
 * \retval 1 The string is a hexadecimal number.
 */
unsigned int isHexString(const char *data)
{
	size_t n;
	size_t len = strlen(data);

	/* Check for empty string */
	if (len == 0) return 0;

	/* Check for non-digit and non-A-through-F characters */
	for (n = 0; n < len; n++) {
		if (!isdigit(data[n])
				&& data[n] != 'A'
				&& data[n] != 'B'
				&& data[n] != 'C'
				&& data[n] != 'D'
				&& data[n] != 'E'
				&& data[n] != 'F'
				&& data[n] != 'a'
				&& data[n] != 'b'
				&& data[n] != 'c'
				&& data[n] != 'd'
				&& data[n] != 'e'
				&& data[n] != 'f')
			return 0;
	}

	return 1;
}

/**
 * Evaluates an identifier to produce its name as a string.
 *
 * \param [in] id The identifier to evaluate.
 *
 * \param [in] scope The scope to evaluate \a id under.
 *
 * \return A new string containing the evaluated name of the identifier.
 *
 * \retval NULL Memory allocation failed.
 */
char *resolveIdentifierName(IdentifierNode *id,
                            ScopeObject *scope)
{
	ValueObject *val = NULL;
	ValueObject *str = NULL;
	char *ret = NULL;

	if (!id) goto resolveIdentifierNameAbort;

	if (id->type == IT_DIRECT) {
		/* Just return a copy of the character array */
		const char *temp = (char *)(id->id);
		ret = malloc(sizeof(char) * (strlen(temp) + 1));
		strcpy(ret, temp);
	}
	else if (id->type == IT_INDIRECT) {
		ExprNode *expr = (ExprNode *)(id->id);

		/* Interpret the identifier expression */
		val = interpretExprNode(expr, scope);
		if (!val) goto resolveIdentifierNameAbort;

		/* Then cast it to a string */
		str = castStringExplicit(val, scope);
		if (!str) goto resolveIdentifierNameAbort;
		deleteValueObject(val);

		/* Copy the evaluated string */
		ret = copyString(getString(str));
		if (!ret) goto resolveIdentifierNameAbort;
		deleteValueObject(str);
	}
	else {
		char *name = resolveIdentifierName(id, scope);
		error(IN_INVALID_IDENTIFIER_TYPE, id->fname, id->line, name);
		free(name);
	}

	return ret;

resolveIdentifierNameAbort: /* Exception handline */

	/* Clean up any allocated structures */
	if (ret) free(ret);
	if (str) deleteValueObject(str);
	if (val) deleteValueObject(val);

	return NULL;
}

/**
 * Creates a nil-type value.
 *
 * \return A new nil-type value.
 *
 * \retval NULL Memory allocation failed.
 */
ValueObject *createNilValueObject(void)
{
	ValueObject *p = malloc(sizeof(ValueObject));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->type = VT_NIL;
	p->semaphore = 1;
	return p;
}

/**
 * Creates a boolean-type value.
 *
 * \param [in] data The boolean data to store.
 *
 * \return A boolean-type value equalling 0 if \a data equals 0 and 1 otherwise.
 *
 * \retval NULL Memory allocation failed.
 */
ValueObject *createBooleanValueObject(int data)
{
	ValueObject *p = malloc(sizeof(ValueObject));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->type = VT_BOOLEAN;
	p->data.i = (data != 0);
	p->semaphore = 1;
	return p;
}

/**
 * Creates a integer-type value.
 *
 * \param [in] data The integer data to store.
 *
 * \return An integer-type value equalling \a data.
 *
 * \retval NULL Memory allocation failed.
 */
ValueObject *createIntegerValueObject(long long data)
{
	ValueObject *p = malloc(sizeof(ValueObject));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->type = VT_INTEGER;
	p->data.i = data;
	p->semaphore = 1;
	return p;
}

/**
 * Creates a floating-point-type value.
 *
 * \param [in] data The floating-point data to store.
 *
 * \return A floating-point-type value equalling \a data.
 *
 * \retval NULL Memory allocation failed.
 */
ValueObject *createFloatValueObject(float data)
{
	ValueObject *p = malloc(sizeof(ValueObject));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->type = VT_FLOAT;
	p->data.f = data;
	p->semaphore = 1;
	return p;
}

/**
 * Creates a string-type value.
 *
 * \param [in] data The string data to store.
 *
 * \note \a data is stored as-is; no copy of it is made.
 *
 * \return A string-type value equalling \a data.
 *
 * \retval NULL Memory allocation failed.
 */
ValueObject *createStringValueObject(char *data)
{
	ValueObject *p = malloc(sizeof(ValueObject));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->type = VT_STRING;
	p->data.s = data;
	p->semaphore = 1;
	return p;
}

/**
 * Creates a function-type value.
 *
 * \param [in] def The function definition to store.
 *
 * \return A function-type value containing \a data.
 *
 * \retval NULL Memory allocation failed.
 */
ValueObject *createFunctionValueObject(FuncDefStmtNode *def)
{
	ValueObject *p = malloc(sizeof(ValueObject));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->type = VT_FUNC;
	p->data.fn = def;
	p->semaphore = 1;
	return p;
}

/**
 * Creates an array-type value.
 *
 * \param [in] parent The optional parent scope to use.
 *
 * \note \a parent may be NULL, in which case this array is treated as the root.
 *
 * \return An empty array-type value with parent \a parent.
 *
 * \retval NULL Memory allocation failed.
 */
ValueObject *createArrayValueObject(ScopeObject *parent)
{
	ValueObject *p = malloc(sizeof(ValueObject));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->type = VT_ARRAY;
	p->data.a = createScopeObject(parent);
	if (!p->data.a) {
		free(p);
		return NULL;
	}
	p->semaphore = 1;
	return p;
}

/**
 * Creates a blob-type value.
 *
 * \param [in] data The binary blob data to store.
 *
 * \note \a data is stored as-is; no copy of it is made.
 *
 * \return A string-type value equalling \a data.
 *
 * \retval NULL Memory allocation failed.
 */
ValueObject *createBlobValueObject(void *data)
{
	ValueObject *p = malloc(sizeof(ValueObject));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->type = VT_BLOB;
	p->data.b = data;
	p->semaphore = 1;
	return p;
}

/**
 * Copies a value.
 *
 * Instead of actually performing a copy of memory, this function increments a
 * semaphore in \a value and returns \a value again.  The semaphore gets
 * decremented when \a value gets deleted.  This way, an immutable copy of a
 * value may be made without actually copying its blocks of memory; this reduces
 * the overhead associated with copying a value--a fairly common
 * operation--while still preserving its usability.
 *
 * \param [in,out] value The value to copy.
 *
 * \return A value with the same type and contents as \a value.
 *
 * \retval NULL The type of \a value is unrecognized.
 */
ValueObject *copyValueObject(ValueObject *value)
{
	V(value);
	return value;
}

/**
 * Deletes a value.
 *
 * This function decrements a semaphore in \a value and deletes \a value if the
 * semaphore reaches 0 (no copies of this value are need anymore).  The
 * semaphore gets incremented when either the value is created or it gets
 * copied.  This way, an immutable copy of the value may be made without
 * actually copying its memory.
 *
 * \param [in,out] value The value to delete.
 *
 * \post The memory at \a value and any of its members will be freed (although
 * see note for full details).
 */
void deleteValueObject(ValueObject *value)
{
	if (!value) return;
	P(value);
	if (!value->semaphore) {
		if (value->type == VT_STRING)
			free(value->data.s);
		/* FuncDefStmtNode structures get freed with the parse tree */
		else if (value->type == VT_ARRAY)
			deleteScopeObject(value->data.a);
		free(value);
	}
}

/**
 * Creates a scope.
 *
 * Scopes are used to map identifiers to values.  Scopes are organized
 * hierarchically.
 *
 * \param [in] parent The optional parent scope to use.
 *
 * \return An empty scope with parent \a parent.
 *
 * \retval NULL Memory allocation failed.
 */
ScopeObject *createScopeObject(ScopeObject *parent)
{
	ScopeObject *p = malloc(sizeof(ScopeObject));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->impvar = createNilValueObject();
	if (!p->impvar) {
		free(p);
		return NULL;
	}
	p->numvals = 0;
	p->names = NULL;
	p->values = NULL;
	p->parent = parent;
	if (parent) p->caller = parent->caller;
	else p->caller = NULL;
	return p;
}

/**
 * Creates a scope with a specific caller.
 *
 * \param [in] parent The optional parent scope to use.
 *
 * \param [in] caller The caller scope to use.
 *
 * \return An empty scope with parent \a parent and caller \a caller.
 *
 * \retval NULL Memory allocation failed.
 */
ScopeObject *createScopeObjectCaller(ScopeObject *parent,
                                     ScopeObject *caller)
{
	ScopeObject *p = createScopeObject(parent);
	if (!p) return NULL;
	if (caller) p->caller = caller;
	return p;
}

/**
 * Deletes a scope.
 *
 * \param [in,out] scope The scope to delete.
 *
 * \post The memory at \a scope and any of its members will be freed.
 */
void deleteScopeObject(ScopeObject *scope)
{
	unsigned int n;
	if (!scope) return;
	for (n = 0; n < scope->numvals; n++) {
		free(scope->names[n]);
		deleteValueObject(scope->values[n]);
	}
	free(scope->names);
	free(scope->values);
	deleteValueObject(scope->impvar);
	free(scope);
}

/**
 * Creates a new, nil-type value in a scope.
 *
 * \param [in] src The scope to evaluate \a target under.
 *
 * \param [in,out] dest The scope to create the new value in.
 *
 * \param [in] target The name of the value to create.
 *
 * \return The newly-created value.
 *
 * \retval NULL Memory allocation failed.
 */
ValueObject *createScopeValue(ScopeObject *src,
                              ScopeObject *dest,
                              IdentifierNode *target)
{
	ScopeObject *parent = dest;
	IdentifierNode *child = target;
	int status;
	unsigned int newnumvals;
	void *mem1 = NULL;
	void *mem2 = NULL;
	char *name = NULL;

	/* Traverse the target to the terminal child and parent */
	status = resolveTerminalSlot(src, dest, target, &parent, &child);
	if (!status) goto createScopeValueAbort;

	/* Store the new number of values */
	newnumvals = dest->numvals + 1;

	/* Look up the identifier name */
	name = resolveIdentifierName(target, src);
	if (!name) goto createScopeValueAbort;

	/* Add value to local scope */
	mem1 = realloc(dest->names, sizeof(IdentifierNode *) * newnumvals);
	if (!mem1) {
		perror("realloc");
		goto createScopeValueAbort;
	}
	mem2 = realloc(dest->values, sizeof(ValueObject *) * newnumvals);
	if (!mem2) {
		perror("realloc");
		goto createScopeValueAbort;
	}

	dest->names = mem1;
	dest->values = mem2;
	dest->names[dest->numvals] = name;
	dest->values[dest->numvals] = createNilValueObject();
	if (!dest->values[dest->numvals]) goto createScopeValueAbort;
	dest->numvals = newnumvals;

	return dest->values[dest->numvals - 1];

createScopeValueAbort: /* In case something goes wrong... */

	/* Clean up any allocated structures */
	if (name) free(name);
	if (mem1) free(mem1);
	if (mem2) free(mem2);

	return NULL;
}

/**
 * Updates a value in a scope.
 *
 * \param [in] src The scope to evaluate \a target under.
 *
 * \param [in,out] dest The scope to update the value in.
 *
 * \param [in] target The name of the value to create.
 *
 * \param [in] value The new value to assign.
 *
 * \return The updated value (will be the same as \a val).
 *
 * \retval NULL Either \a target could not be evaluated in \a src or \a target
 * could not be found in \a dest.
 */
ValueObject *updateScopeValue(ScopeObject *src,
                              ScopeObject *dest,
                              IdentifierNode *target,
                              ValueObject *value)
{
	ScopeObject *parent = dest;
	IdentifierNode *child = target;
	int status;
	char *name = NULL;

	/* Traverse the target to the terminal child and parent */
	status = resolveTerminalSlot(src, dest, target, &parent, &child);
	if (!status) goto updateScopeValueAbort;

	/* Look up the identifier name */
	name = resolveIdentifierName(child, src);
	if (!name) goto updateScopeValueAbort;

	/* Traverse upwards through scopes */
	do {
		unsigned int n;
		/* Check for existing value in current scope */
		for (n = 0; n < parent->numvals; n++) {
			if (!strcmp(parent->names[n], name)) {
				free(name);
				/* Wipe out the old value */
				deleteValueObject(parent->values[n]);
				/* Assign the new value */
				if (value) {
					parent->values[n] = value;
				}
				else {
					parent->values[n] = createNilValueObject();
				}
				return parent->values[n];
			}
		}
	} while ((parent = parent->parent));

	{
		char *name = resolveIdentifierName(target, src);
		error(IN_UNABLE_TO_STORE_VARIABLE, target->fname, target->line, name);
		free(name);
	}

updateScopeValueAbort: /* In case something goes wrong... */

	/* Clean up any allocated structures */
	if (name) free(name);

	return NULL;
}

/**
 * Gets a stored value in a scope.
 *
 * \param [in] src The scope to evaluate \a target under.
 *
 * \param [in,out] dest The scope to update the value in.
 *
 * \param [in] target The name of the value to get.
 *
 * \return The value in \a dest, named by evaluating \a target under \a src.
 *
 * \retval NULL Either \a target could not be evaluated in \a src or \a target
 * could not be found in \a dest.
 */
ValueObject *getScopeValue(ScopeObject *src,
                           ScopeObject *dest,
                           IdentifierNode *target)
{
	ScopeObject *parent = dest;
	IdentifierNode *child = target;
	char *name = NULL;
	int status;

	/* Traverse the target to the terminal child and parent */
	status = resolveTerminalSlot(src, dest, target, &parent, &child);
	if (!status) goto getScopeValueAbort;

	/* Look up the identifier name */
	name = resolveIdentifierName(child, src);
	if (!name) goto getScopeValueAbort;

	/* Traverse upwards through scopes */
	do {
		unsigned int n;
		/* Check for value in current scope */
		for (n = 0; n < parent->numvals; n++) {
			if (!strcmp(parent->names[n], name)) {
				free(name);
				return parent->values[n];
			}
		}
	} while ((parent = parent->parent));

	{
		char *name = resolveIdentifierName(child, src);
		error(IN_VARIABLE_DOES_NOT_EXIST, child->fname, child->line, name);
		free(name);
	}

getScopeValueAbort: /* In case something goes wrong... */

	/* Clean up any allocated structures */
	if (name) free(name);

	return NULL;
}

/**
 * Gets a scope without accessing any arrays.
 *
 * \param [in] src The scope to evaluate \a target under.
 *
 * \param [in,out] dest The scope to update the value in.
 *
 * \param [in] target The name of the value containing the scope to get.
 *
 * \return The scope contained in the value in \a dest, named by evaluating \a
 * target under \a src, without accessing any arrays.
 *
 * \retval NULL Either \a target could not be evaluated in \a src or \a target
 * could not be found in \a dest.
 */
/** \todo Add this definition to interpreter.h */
ScopeObject *getScopeObjectLocal(ScopeObject *src,
                                 ScopeObject *dest,
                                 IdentifierNode *target)
{
	ScopeObject *current = dest;
	char *name = NULL;

	/* Look up the identifier name */
	name = resolveIdentifierName(target, src);
	if (!name) goto getScopeObjectLocalAbort;

	/* Check for calling object reference variable */
	if (!strcmp(name, "ME")) {
		/* Traverse upwards through callers */
		for (current = dest;
				current->caller;
				current = current->caller);
		free(name);
		return current;
	}

	/* Traverse upwards through scopes */
	do {
		unsigned int n;
		/* Check for value in current scope */
		for (n = 0; n < current->numvals; n++) {
			if (!strcmp(current->names[n], name)) {
				free(name);
				return getArray(current->values[n]);
			}
		}
	} while ((current = current->parent));

	{
		char *name = resolveIdentifierName(target, src);
		error(IN_VARIABLE_DOES_NOT_EXIST, target->fname, target->line, name);
		free(name);
	}

getScopeObjectLocalAbort: /* In case something goes wrong... */

	/* Clean up any allocated structures */
	if (name) free(name);

	return NULL;
}

/**
 * Gets a value from a scope without accessing its ancestors.
 *
 * \param [in] src The scope to evaluate \a target under.
 *
 * \param [in,out] dest The scope to update the value in.
 *
 * \param [in] target The name of the value to get.
 *
 * \return The value in \a dest, named by evaluating \a target under \a src,
 * without accessing any ancestors of \a dest.
 *
 * \retval NULL Either \a target could not be evaluated in \a src or \a target
 * could not be found in \a dest.
 */
ValueObject *getScopeValueLocal(ScopeObject *src,
                                ScopeObject *dest,
                                IdentifierNode *target)
{
	unsigned int n;
	char *name = NULL;
	ScopeObject *scope = NULL;

	/* Access any slots */
	while (target->slot) {
		/*
		 * Look up the target in the dest scope, using the src scope
		 * for resolving variables in indirect identifiers
		 */
		scope = getScopeObjectLocal(src, dest, target);
		if (!scope) return 0;
		dest = scope;

		target = target->slot;
	}

	/* Look up the identifier name */
	name = resolveIdentifierName(target, src);
	if (!name) goto getScopeValueLocalAbort;

	/* Check for value in current scope */
	for (n = 0; n < dest->numvals; n++) {
		if (!strcmp(dest->names[n], name)) {
			free(name);
			return dest->values[n];
		}
	}

getScopeValueLocalAbort: /* In case something goes wrong... */

	/* Clean up any allocated structures */
	if (name) free(name);

	return NULL;
}

/**
 * Gets a scope from within another scope.
 *
 * \param [in] src The scope to evaluate \a target under.
 *
 * \param [in,out] dest The scope to update the value in.
 *
 * \param [in] target The name of the scope to get.
 *
 * \return The value in \a dest, named by evaluating \a target under \a src,
 * without accessing any ancestors of \a dest.
 *
 * \retval NULL Either \a target could not be evaluated in \a src or \a target
 * could not be found in \a dest.
 */
ScopeObject *getScopeObject(ScopeObject *src,
                            ScopeObject *dest,
                            IdentifierNode *target)
{
	ValueObject *val = NULL;
	char *name = NULL;
	int status;
	int isI;
	int isME;
	ScopeObject *scope;
	
	/* Look up the identifier name */
	name = resolveIdentifierName(target, src);
	if (!name) goto getScopeObjectAbort;

	/* Check for targets with special meanings */
	isI = strcmp(name, "I");
	isME = strcmp(name, "ME");
	free(name);
	name = NULL;

	if (!isI) {
		/* The function scope variable */
		return src;
	}
	else if (!isME) {
		/* The calling object scope variable */
		scope = getScopeObjectLocal(src, dest, target);
		if (!scope) goto getScopeObjectAbort;
		return scope;
	}

	/* Access any slots */
	while (target->slot) {
		/*
		 * Look up the target in the dest scope, using the src scope
		 * for resolving variables in indirect identifiers
		 */
		scope = getScopeObjectLocal(src, dest, target);
		if (!scope) goto getScopeObjectAbort;
		dest = scope;

		target = target->slot;
	}

	val = getScopeValue(src, dest, target);
	if (!val) goto getScopeObjectAbort;
	if (val->type != VT_ARRAY) {
		char *name = resolveIdentifierName(target, src);
		error(IN_VARIABLE_NOT_AN_ARRAY, target->fname, target->line, name);
		free(name);
		goto getScopeObjectAbort;
	}

	return getArray(val);

getScopeObjectAbort: /* In case something goes wrong... */

	/* Clean up any allocated structures */
	if (name) free(name);

	return NULL;
}

/**
 * Deletes a value from a scope.
 *
 * \param [in] src The scope to evaluate \a target under.
 *
 * \param [in,out] dest The scope to update the value in.
 *
 * \param [in] target The name of the value to delete.
 */
void deleteScopeValue(ScopeObject *src,
                      ScopeObject *dest,
                      IdentifierNode *target)
{
	ScopeObject *current = NULL;
	char *name = NULL;
	void *mem1 = NULL;
	void *mem2 = NULL;
	ScopeObject *scope = NULL;

	/* Access any slots */
	while (target->slot) {
		/*
		 * Look up the target in the dest scope, using the src scope
		 * for resolving variables in indirect identifiers
		 */
		scope = getScopeObjectLocal(src, dest, target);
		if (!scope) goto deleteScopeValueAbort;
		dest = scope;
		target = target->slot;
	}
	current = dest;

	/* Look up the identifier name */
	name = resolveIdentifierName(target, src);
	if (!name) goto deleteScopeValueAbort;

	/* Traverse upwards through scopes */
	do {
		unsigned int n;
		/* Check for existing value in current scope */
		for (n = 0; n < current->numvals; n++) {
			if (!strcmp(current->names[n], name)) {
				unsigned int i;
				unsigned int newnumvals = dest->numvals - 1;
				free(name);
				/* Wipe out the name and value */
				free(current->names[n]);
				deleteValueObject(current->values[n]);
				/* Reorder the tables */
				for (i = n; i < current->numvals - 1; i++) {
					current->names[i] = current->names[i + 1];
					current->values[i] = current->values[i + 1];
				}
				/* Resize the tables */
				mem1 = realloc(dest->names, sizeof(IdentifierNode *) * newnumvals);
				if (!mem1) {
					perror("realloc");
					goto deleteScopeValueAbort;
				}
				mem2 = realloc(dest->values, sizeof(ValueObject *) * newnumvals);
				if (!mem2) {
					perror("realloc");
					goto deleteScopeValueAbort;
				}
				dest->names = mem1;
				dest->values = mem2;
				dest->numvals = newnumvals;
				return;
			}
		}
	} while ((current = current->parent));

	free(name);

	return;

deleteScopeValueAbort: /* In case something goes wrong... */

	/* Clean up any allocated structures */
	if (name) free(name);
	if (mem1) free(mem1);
	if (mem2) free(mem2);
	if (scope) free(scope);

	return;
}

/**
 * Creates a returned value.
 *
 * \param [in] type The type of returned value.
 *
 * \param [in] value An optional value to return.
 *
 * \return A pointer to a returned value with the desired properties.
 *
 * \retval NULL Memory allocation failed.
 */
ReturnObject *createReturnObject(ReturnType type,
                                 ValueObject *value)
{
	ReturnObject *p = malloc(sizeof(ReturnObject));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	p->type = type;
	p->value = value;
	return p;
}

/**
 * Deletes a returned value.
 *
 * \param [in,out] object The returned value to be deleted.
 *
 * \post The memory at \a object and all of its members will be freed.
 */
void deleteReturnObject(ReturnObject *object)
{
	if (!object) return;
	if (object->type == RT_RETURN)
		deleteValueObject(object->value);
	free(object);
}

/**
 * Starting from an initial parent scope and target identifier, traverses down
 * until the target identifier is not a scope.  Stores the value of the terminal
 * child identifier and its parent scope.
 *
 * \param [in] src The scope to resolve \a target in.
 *
 * \param [in] dest The scope to retrieve \a target from.
 *
 * \param [in] target The array slot to traverse.
 *
 * \param [out] parent The parent of the terminal child identifier of \a target.
 *
 * \param [out] child The terminal child identifier of \a target.
 *
 * \return A status code indicating success or failure.
 *
 * \retval 0 Failed to traverse array.
 *
 * \retval 1 Succeeded to traverse array.
 *
 * \post \a parent will point to the parent scope containing the terminal child.
 *
 * \post \a child will point to the terminal child.
 */
int resolveTerminalSlot(ScopeObject *src,
                        ScopeObject *dest,
                        IdentifierNode *target,
                        ScopeObject **parent,
                        IdentifierNode **child)
{
	ScopeObject *scope = NULL;

	/* Start with default values */
	*parent = dest;
	*child = target;

	/* Access any slots */
	while (target->slot) {
		/*
		 * Look up the target in the dest scope, using the src scope
		 * for resolving variables in indirect identifiers
		 */
		scope = getScopeObjectLocal(src, dest, target);
		if (!scope) goto resolveTerminalSlotAbort;
		dest = scope;

		/* Change the target to the old target's slot */
		target = target->slot;
	}

	/* Store the output values */
	*parent = dest;
	*child = target;

	return 1;

resolveTerminalSlotAbort: /* In case something goes wrong... */

	/* Clean up any allocated structures */
	if (scope) deleteScopeObject(scope);

	return 0;
}

/**
 * Casts the contents of a value to boolean type in an implicit way.  Casting is
 * not done directly to \a node, instead, it is performed on a copy which is
 * what is returned.
 *
 * \param [in] node The value to cast.
 * 
 * \param [in] scope The scope to use for variable interpolation.
 *
 * \return A pointer to a value with a copy of the contents of \a node, cast to
 * boolean type.
 *
 * \retval NULL An error occurred while casting.
 */
ValueObject *castBooleanImplicit(ValueObject *node,
                                 ScopeObject *scope)
{
	if (!node) return NULL;
	return castBooleanExplicit(node, scope);
}

/**
 * Casts the contents of a value to integer type in an implicit way.  Casting is
 * not done directly to \a node, instead, it is performed on a copy which is
 * what is returned.
 *
 * \param [in] node The value to cast.
 * 
 * \param [in] scope The scope to use for variable interpolation.
 *
 * \return A pointer to a value with a copy of the contents of \a node, cast to
 * integer type.
 *
 * \retval NULL An error occurred while casting.
 */
ValueObject *castIntegerImplicit(ValueObject *node,
                                 ScopeObject *scope)
{
	if (!node) return NULL;
	if (node->type == VT_NIL) {
		error(IN_CANNOT_IMPLICITLY_CAST_NIL);
		return NULL;
	}
	else return castIntegerExplicit(node, scope);
}

/**
 * Casts the contents of a value to decimal type in an implicit way.  Casting is
 * not done directly to \a node, instead, it is performed on a copy which is
 * what is returned.
 *
 * \param [in] node The value to cast.
 * 
 * \param [in] scope The scope to use for variable interpolation.
 *
 * \return A pointer to a value with a copy of the contents of \a node, cast to
 * decimal type.
 *
 * \retval NULL An error occurred while casting.
 */
ValueObject *castFloatImplicit(ValueObject *node,
                               ScopeObject *scope)
{
	if (!node) return NULL;
	if (node->type == VT_NIL) {
		error(IN_CANNOT_IMPLICITLY_CAST_NIL);
		return NULL;
	}
	else return castFloatExplicit(node, scope);
}

/**
 * Casts the contents of a value to string type in an implicit way.  Casting is
 * not done directly to \a node, instead, it is performed on a copy which is
 * what is returned.
 *
 * \param [in] node The value to cast.
 * 
 * \param [in] scope The scope to use for variable interpolation.
 *
 * \note \a scope is used to resolve variable interpolation within the string
 * before casting it.  Therefore, a simple way to interpolate the variables
 * within a string is to call this function with it.
 *
 * \return A pointer to a value with a copy of the contents of \a node, cast to
 * string type.
 *
 * \retval NULL An error occurred while casting.
 */
ValueObject *castStringImplicit(ValueObject *node,
                                ScopeObject *scope)
{
	if (!node) return NULL;
	if (node->type == VT_NIL) {
		error(IN_CANNOT_IMPLICITLY_CAST_NIL);
		return NULL;
	}
	else return castStringExplicit(node, scope);
}

/**
 * Casts the contents of a value to boolean type in an explicit way.  Casting is
 * not done directly to \a node, instead, it is performed on a copy which is
 * what is returned.
 *
 * \param [in] node The value to cast.
 * 
 * \param [in] scope The scope to use for variable interpolation.
 *
 * \return A pointer to a value with a copy of the contents of \a node, cast to
 * boolean type.
 *
 * \retval NULL An error occurred while casting.
 */
ValueObject *castBooleanExplicit(ValueObject *node,
                                 ScopeObject *scope)
{
	if (!node) return NULL;
	switch (node->type) {
		case VT_NIL:
			return createBooleanValueObject(0);
		case VT_BOOLEAN:
			return createBooleanValueObject(getInteger(node));
		case VT_INTEGER:
			return createBooleanValueObject(getInteger(node) != 0);
		case VT_FLOAT:
			return createBooleanValueObject(fabs(getFloat(node) - 0.0) > FLT_EPSILON);
		case VT_STRING:
			if (strstr(getString(node), ":{")) {
				/* Perform interpolation */
				ValueObject *ret = NULL;
				ValueObject *interp = castStringExplicit(node, scope);
				if (!interp) return NULL;
				ret = createBooleanValueObject(getString(interp)[0] != '\0');
				deleteValueObject(interp);
				return ret;
			}
			else
				return createBooleanValueObject(getString(node)[0] != '\0');
		case VT_FUNC:
			error(IN_CANNOT_CAST_FUNCTION_TO_BOOLEAN);
			return NULL;
		case VT_ARRAY:
			error(IN_CANNOT_CAST_ARRAY_TO_BOOLEAN);
			return NULL;
		default:
			error(IN_UNKNOWN_VALUE_DURING_BOOLEAN_CAST);
			return NULL;
	}
}

/**
 * Casts the contents of a value to integer type in an explicit way.  Casting is
 * not done directly to \a node, instead, it is performed on a copy which is
 * what is returned.
 *
 * \param [in] node The value to cast.
 * 
 * \param [in] scope The scope to use for variable interpolation.
 *
 * \return A pointer to a value with a copy of the contents of \a node, cast to
 * integer type.
 *
 * \retval NULL An error occurred while casting.
 */
ValueObject *castIntegerExplicit(ValueObject *node,
                                 ScopeObject *scope)
{
	if (!node) return NULL;
	switch (node->type) {
		case VT_NIL:
			return createIntegerValueObject(0);
		case VT_BOOLEAN:
		case VT_INTEGER:
			return createIntegerValueObject(getInteger(node));
		case VT_FLOAT:
			return createIntegerValueObject((long long)getFloat(node));
		case VT_STRING:
			if (strstr(getString(node), ":{")) {
				/* Perform interpolation */
				ValueObject *ret = NULL;
				ValueObject *interp = castStringExplicit(node, scope);
				long long value;
				if (!interp) return NULL;
				if (!isDecString(getString(interp))) {
					error(IN_UNABLE_TO_CAST_VALUE);
					deleteValueObject(interp);
					return NULL;
				}
				if (sscanf(getString(interp), "%lli", &value) != 1) {
					error(IN_EXPECTED_INTEGER_VALUE);
					deleteValueObject(interp);
					return NULL;
				}
				ret = createIntegerValueObject(value);
				deleteValueObject(interp);
				return ret;
			}
			else {
				long long value;
				if (!isDecString(getString(node))) {
					error(IN_UNABLE_TO_CAST_VALUE);
					return NULL;
				}
				if (sscanf(getString(node), "%lli", &value) != 1) {
					error(IN_EXPECTED_INTEGER_VALUE);
					return NULL;
				}
				return createIntegerValueObject(value);
			}
		case VT_FUNC:
			error(IN_CANNOT_CAST_FUNCTION_TO_INTEGER);
			return NULL;
		case VT_ARRAY:
			error(IN_CANNOT_CAST_ARRAY_TO_INTEGER);
			return NULL;
		default:
			error(IN_UNKNOWN_VALUE_DURING_INTEGER_CAST);
			return NULL;
	}
}

/**
 * Casts the contents of a value to decimal type in an explicit way.  Casting is
 * not done directly to \a node, instead, it is performed on a copy which is
 * what is returned.
 *
 * \param [in] node The value to cast.
 * 
 * \param [in] scope The scope to use for variable interpolation.
 *
 * \return A pointer to a value with a copy of the contents of \a node, cast to
 * decimal type.
 *
 * \retval NULL An error occurred while casting.
 */
ValueObject *castFloatExplicit(ValueObject *node,
                               ScopeObject *scope)
{
	if (!node) return NULL;
	switch (node->type) {
		case VT_NIL:
			return createFloatValueObject(0.0);
		case VT_BOOLEAN:
		case VT_INTEGER:
			return createFloatValueObject((float)getInteger(node));
		case VT_FLOAT:
			return createFloatValueObject(getFloat(node));
		case VT_STRING:
			if (strstr(getString(node), ":{")) {
				/* Perform interpolation */
				ValueObject *ret = NULL;
				ValueObject *interp = castStringExplicit(node, scope);
				float value;
				if (!interp) return NULL;
				if (!isDecString(getString(interp))) {
					error(IN_UNABLE_TO_CAST_VALUE);
					deleteValueObject(interp);
					return NULL;
				}
				if (sscanf(getString(interp), "%f", &value) != 1) {
					error(IN_EXPECTED_DECIMAL);
					deleteValueObject(interp);
					return NULL;
				}
				ret = createFloatValueObject(value);
				deleteValueObject(interp);
				return ret;
			}
			else {
				float value;
				if (!isDecString(getString(node))) {
					error(IN_UNABLE_TO_CAST_VALUE);
					return NULL;
				}
				if (sscanf(getString(node), "%f", &value) != 1) {
					error(IN_EXPECTED_DECIMAL);
					return NULL;
				}
				return createFloatValueObject(value);
			}
		case VT_FUNC:
			error(IN_CANNOT_CAST_FUNCTION_TO_DECIMAL);
			return NULL;
		case VT_ARRAY:
			error(IN_CANNOT_CAST_ARRAY_TO_DECIMAL);
			return NULL;
		default:
			error(IN_UNKNOWN_VALUE_DURING_DECIMAL_CAST);
			return NULL;
	}
}

/**
 * Casts the contents of a value to string type in an explicit way.  Casting is
 * not done directly to \a node, instead, it is performed on a copy which is
 * what is returned.
 *
 * \param [in] node The value to cast.
 * 
 * \param [in] scope The scope to use for variable interpolation.
 *
 * \note \a scope is used to resolve variable interpolation within the string
 * before casting it.  Therefore, a simple way to interpolate the variables
 * within a string is to call this function with it.
 *
 * \return A pointer to a value with a copy of the contents of \a node, cast to
 * string type.
 *
 * \retval NULL An error occurred while casting.
 */
ValueObject *castStringExplicit(ValueObject *node,
                                ScopeObject *scope)
{
	if (!node) return NULL;
	switch (node->type) {
		case VT_NIL: {
			char *str = copyString("");
			if (!str) return NULL;
			return createStringValueObject(str);
		}
		case VT_BOOLEAN: {
			/*
			 * \note The spec does not define how TROOFs may be cast
			 * to YARNs.
			 */
			error(IN_CANNOT_CAST_BOOLEAN_TO_STRING);
			return NULL;
		}
		case VT_INTEGER: {
			char *data = NULL;
			/*
			 * One character per integer bit plus one more for the
			 * null character
			 */
			size_t size = sizeof(long long) * 8 + 1;
			data = malloc(sizeof(char) * size);
			if (!data) return NULL;
			sprintf(data, "%lli", getInteger(node));
			return createStringValueObject(data);
		}
		case VT_FLOAT: {
			char *data = NULL;
			unsigned int precision = 2;
			/*
			 * One character per float bit plus one more for the
			 * null character
			 */
			size_t size = sizeof(float) * 8 + 1;
			data = malloc(sizeof(char) * size);
			if (!data) return NULL;
			sprintf(data, "%f", getFloat(node));
			/* Truncate to a certain number of decimal places */
			strchr(data, '.')[precision + 1] = '\0';
			return createStringValueObject(data);
		}
		case VT_STRING: {
			char *temp = NULL;
			char *data = NULL;
			char *str = getString(node);
			unsigned int a, b;
			size_t size;
			/* Perform interpolation */
			size = strlen(getString(node)) + 1;
			temp = malloc(sizeof(char) * size);
			for (a = 0, b = 0; str[b] != '\0'; ) {
				if (!strncmp(str + b, ":)", 2)) {
					temp[a] = '\n';
					a++, b += 2;
				}
				else if (!strncmp(str + b, ":3", 2)) {
					temp[a] = '\r';
					a++, b += 2;
				}
				else if (!strncmp(str + b, ":>", 2)) {
					temp[a] = '\t';
					a++, b += 2;
				}
				else if (!strncmp(str + b, ":o", 2)) {
					temp[a] = '\a';
					a++, b += 2;
				}
				else if (!strncmp(str + b, ":\"", 2)) {
					temp[a] = '"';
					a++, b += 2;
				}
				else if (!strncmp(str + b, "::", 2)) {
					temp[a] = ':';
					a++, b += 2;
				}
				else if (!strncmp(str + b, ":(", 2)) {
					const char *start = str + b + 2;
					const char *end = strchr(start, ')');
					size_t len;
					char *image = NULL;
					long codepoint;
					char out[3];
					size_t num;
					void *mem = NULL;
					if (end < start) {
						error(IN_EXPECTED_CLOSING_PAREN);
						free(temp);
						return NULL;
					}
					len = (size_t)(end - start);
					image = malloc(sizeof(char) * (len + 1));
					strncpy(image, start, len);
					image[len] = '\0';
					if (!isHexString(image)) {
						error(IN_INVALID_HEX_NUMBER);
						free(temp);
						free(image);
						return NULL;
					}
					codepoint = strtol(image, NULL, 16);
					free(image);
					if (codepoint < 0) {
						error(IN_CODE_POINT_MUST_BE_POSITIVE);
						free(temp);
						return NULL;
					}
					num = convertCodePointToUTF8((unsigned int)codepoint, out);
					if (num == 0) {
						free(temp);
						return NULL;
					}
					size += num;
					mem = realloc(temp, size);
					if (!mem) {
						perror("realloc");
						free(temp);
						return NULL;
					}
					temp = mem;
					strncpy(temp + a, out, num);
					a += num, b += len + 3;
				}
				else if (!strncmp(str + b, ":[", 2)) {
					const char *start = str + b + 2;
					const char *end = strchr(start, ']');
					size_t len;
					char *image = NULL;
					long codepoint;
					char out[3];
					size_t num;
					void *mem = NULL;
					if (end < start) {
						error(IN_EXPECTED_CLOSING_SQUARE_BRACKET);
						free(temp);
						return NULL;
					}
					len = (size_t)(end - start);
					image = malloc(sizeof(char) * (len + 1));
					strncpy(image, start, len);
					strncpy(image, start, len);
					image[len] = '\0';
					codepoint = convertNormativeNameToCodePoint(image);
					free(image);
					if (codepoint < 0) {
						error(IN_CODE_POINT_MUST_BE_POSITIVE);
						free(temp);
						return NULL;
					}
					num = convertCodePointToUTF8((unsigned int)codepoint, out);
					size += num;
					mem = realloc(temp, size);
					if (!mem) {
						perror("realloc");
						free(temp);
						return NULL;
					}
					temp = mem;
					strncpy(temp + a, out, num);
					a += num, b += len + 3;
				}
				else if (!strncmp(str + b, ":{", 2)) {
					IdentifierNode *target = NULL;
					ValueObject *val = NULL, *use = NULL;
					/* Copy the variable name into image */
					const char *start = str + b + 2;
					const char *end = strchr(start, '}');
					size_t len;
					char *image = NULL;
					void *mem = NULL;
					if (end < start) {
						error(IN_EXPECTED_CLOSING_CURLY_BRACE);
						free(temp);
						return NULL;
					}
					len = (size_t)(end - start);
					image = malloc(sizeof(char) * (len + 1));
					strncpy(image, start, len);
					image[len] = '\0';
					if (!strcmp(image, "IT"))
						/* Lookup implicit variable */
						val = scope->impvar;
					else {
						/*
						 * Create a new IdentifierNode
						 * structure and look up its
						 * value
						 */
						target = createIdentifierNode(IT_DIRECT, image, NULL, NULL, 0);
						if (!target) {
							free(temp);
							return NULL;
						}
						val = getScopeValue(scope, scope, target);
						if (!val) {
							error(IN_VARIABLE_DOES_NOT_EXIST, target->fname, target->line, image);
							deleteIdentifierNode(target);
							free(temp);
							return NULL;
						}
						deleteIdentifierNode(target);
					}
					/* Cast the variable value to a string */
					if (!(use = castStringImplicit(val, scope))) {
						free(temp);
						return NULL;
					}
					/* Update the size of the new string */
					size += strlen(getString(use));
					mem = realloc(temp, size);
					if (!mem) {
						perror("realloc");
						free(temp);
					}
					temp = mem;
					/* Copy the variable string into the new string */
					strcpy(temp + a, getString(use));
					a += strlen(getString(use)), b += len + 3;
					deleteValueObject(use);
				}
				else {
					temp[a] = str[b];
					a++, b++;
				}
			}
			temp[a] = '\0';
			data = malloc(sizeof(char) * (strlen(temp) + 1));
			strcpy(data, temp);
			free(temp);
			return createStringValueObject(data);
		}
		case VT_FUNC: {
			error(IN_CANNOT_CAST_FUNCTION_TO_STRING);
			return NULL;
		}
		case VT_ARRAY:
			error(IN_CANNOT_CAST_ARRAY_TO_STRING);
			return NULL;
		default:
			error(IN_UNKNOWN_VALUE_DURING_STRING_CAST);
			return NULL;
	}
}

/**
 * Interprets an implicit variable.
 *
 * \param [in] node Not used (see note).
 *
 * \param [in] scope The scope from which to use the implicit variable.
 *
 * \note \a node is not used by this function but is still included in its
 * prototype to allow this function to be stored in a jump table for fast
 * execution.
 *
 * \return A pointer to the value of \a scope's implicit variable.
 */
ValueObject *interpretImpVarExprNode(ExprNode *node,
                                     ScopeObject *scope)
{
	node = NULL;
	return scope->impvar;
}

/**
 * Interprets a cast.
 *
 * \param [in] node A pointer to the expression to interpret.
 *
 * \param [in] scope A pointer to a scope to evaluate \a node under.
 *
 * \pre \a node contains a expression created by createCastExprNode().
 *
 * \return A pointer to the cast value.
 *
 * \retval NULL An error occurred during interpretation.
 */
ValueObject *interpretCastExprNode(ExprNode *node,
                                   ScopeObject *scope)
{
	CastExprNode *expr = (CastExprNode *)node->expr;
	ValueObject *val = interpretExprNode(expr->target, scope);
	ValueObject *ret = NULL;
	if (!val) return NULL;
	switch(expr->newtype->type) {
		case CT_NIL:
			deleteValueObject(val);
			return createNilValueObject();
		case CT_BOOLEAN:
			ret = castBooleanExplicit(val, scope);
			deleteValueObject(val);
			return ret;
		case CT_INTEGER:
			ret = castIntegerExplicit(val, scope);
			deleteValueObject(val);
			return ret;
		case CT_FLOAT:
			ret = castFloatExplicit(val, scope);
			deleteValueObject(val);
			return ret;
		case CT_STRING:
			ret = castStringExplicit(val, scope);
			deleteValueObject(val);
			return ret;
		default:
			error(IN_UNKNOWN_CAST_TYPE);
			deleteValueObject(val);
			return NULL;
	}
}

/**
 * Interprets a function call.
 *
 * \param [in] node A pointer to the expression to interpret.
 *
 * \param [in,out] scope A pointer to a scope to evaluate \a node under.
 *
 * \pre \a node contains an expression created by createFuncCallExprNode().
 *
 * \return A pointer to the returned value.
 *
 * \retval NULL An error occurred during interpretation.
 */
ValueObject *interpretFuncCallExprNode(ExprNode *node,
                                       ScopeObject *scope)
{
	FuncCallExprNode *expr = (FuncCallExprNode *)node->expr;
	unsigned int n;
	ScopeObject *outer = NULL;
	ValueObject *def = NULL;
	ReturnObject *retval = NULL;
	ValueObject *ret = NULL;
	ScopeObject *dest = NULL;
	ScopeObject *target = NULL;

	dest = getScopeObject(scope, scope, expr->scope);

	target = getScopeObjectLocal(scope, dest, expr->name);
	if (!target) return NULL;

	outer = createScopeObjectCaller(scope, target);
	if (!outer) return NULL;

	def = getScopeValue(scope, dest, expr->name);

	if (!def || def->type != VT_FUNC) {
		IdentifierNode *id = (IdentifierNode *)(expr->name);
		char *name = resolveIdentifierName(id, scope);
		if (name) {
			error(IN_UNDEFINED_FUNCTION, id->fname, id->line, name);
			free(name);
		}
		deleteScopeObject(outer);
		return NULL;
	}
	/* Check for correct supplied arity */
	if (getFunction(def)->args->num != expr->args->num) {
		IdentifierNode *id = (IdentifierNode *)(expr->name);
		char *name = resolveIdentifierName(id, scope);
		if (name) {
			error(IN_INCORRECT_NUMBER_OF_ARGUMENTS, id->fname, id->line, name);
			free(name);
		}
		deleteScopeObject(outer);
		return NULL;
	}
	for (n = 0; n < getFunction(def)->args->num; n++) {
		ValueObject *val = NULL;
		if (!createScopeValue(scope, outer, getFunction(def)->args->ids[n])) {
			deleteScopeObject(outer);
			return NULL;
		}
		if (!(val = interpretExprNode(expr->args->exprs[n], scope))) {
			deleteScopeObject(outer);
			return NULL;
		}
		if (!updateScopeValue(scope, outer, getFunction(def)->args->ids[n], val)) {
			deleteScopeObject(outer);
			deleteValueObject(val);
			return NULL;
		}
	}
	/**
	 * \note We use interpretStmtNodeList here because we want to have
	 * access to the function's scope as we may need to retrieve the
	 * implicit variable in the case of a default return.
	 */
	if (!(retval = interpretStmtNodeList(getFunction(def)->body->stmts, outer))) {
		deleteScopeObject(outer);
		return NULL;
	}
	switch (retval->type) {
		case RT_DEFAULT:
			/* Extract return value */
			ret = outer->impvar;
			outer->impvar = NULL;
			break;
		case RT_BREAK:
			ret = createNilValueObject();
			break;
		case RT_RETURN:
			/* Extract return value */
			ret = retval->value;
			retval->value = NULL;
			break;
		default:
			error(IN_INVALID_RETURN_TYPE);
			break;
	}
	deleteReturnObject(retval);
	deleteScopeObject(outer);
	return ret;
}

/**
 * Interprets a system command
 *
 * \param [in] node A pointer to the expression to interpret.
 *
 * \param [in,out] scope A pointer to a scope to evaluate \a node under.
 *
 * \pre \a node contains an expression created by createSystemCommandExprNode().
 *
 * \return A pointer to the returned value.
 *
 * \retval NULL An error occurred during interpretation.
 */
ValueObject *interpretSystemCommandExprNode(ExprNode *node,
                                       ScopeObject *scope)
{
	SystemCommandExprNode *expr = (SystemCommandExprNode *)node->expr;
	ValueObject *val = NULL;
	FILE *f;
	char *cmd = NULL;
	unsigned int buflen = 2048;
	char buf[buflen];
	char *out = NULL;
	unsigned int len;
	unsigned int total = 0;
	unsigned int prev = 0;

	/* Sanity checks */
	if (scope == NULL) return NULL;
	if (node->type != ET_SYSTEMCOMMAND) return NULL;
	if (expr == NULL) return NULL;

	val = interpretExprNode(expr->cmd, scope);
        if (!val) return NULL;

	cmd = getString(val);


	/* Open the command for reading */
	f = popen(cmd, "r");
	if (f == NULL) {
		error(IN_UNABLE_TO_EXECUTE_COMMAND);
		return NULL;
	}

	while ((len = fread(buf, 1, buflen, f)) > 0) {
		prev = total;
		total += len;
		out = realloc(out, total + 1);
		memcpy(out + prev, buf, len);
	}
	if (total > 0) out[total] = '\0';

	/* Close */
	pclose(f);

	/* Return the command object */
	return createStringValueObject(out == NULL ? "" : out);
}

/**
 * Interprets an identifier.
 *
 * \param [in] node A pointer to the expression to interpret.
 *
 * \param [in,out] scope A pointer to a scope to evaluate \a node under.
 *
 * \pre \a node contains an identifier created by createIdentifierNode().
 *
 * \return A pointer to the cast value.
 *
 * \retval NULL An error occurred during interpretation.
 */
ValueObject *interpretIdentifierExprNode(ExprNode *node,
                                         ScopeObject *scope)
{
	ValueObject *val = getScopeValue(scope, scope, node->expr);
	if (!val) return NULL;
	return copyValueObject(val);
}

/**
 * Interprets a constant.
 *
 * \param [in] node A pointer to the expression to interpret.
 *
 * \param [in] scope Not used (see note).
 *
 * \note \a node is not used by this function but is still included in its
 * prototype to allow this function to be stored in a jump table for fast
 * execution.
 *
 * \pre \a node contains a constant created by createXConstantNode(), where X is
 * either Boolean, Integer, Float, or String.
 *
 * \return A pointer to the constant value.
 *
 * \retval NULL An error occurred during interpretation.
 */
ValueObject *interpretConstantExprNode(ExprNode *node,
                                       ScopeObject *scope)
{
	ConstantNode *expr = (ConstantNode *)node->expr;
	scope = NULL;
	switch (expr->type) {
		case CT_NIL:
			return createNilValueObject();
		case CT_BOOLEAN:
			return createBooleanValueObject(expr->data.i);
		case CT_INTEGER:
			return createIntegerValueObject(expr->data.i);
		case CT_FLOAT:
			return createFloatValueObject(expr->data.f);
		case CT_STRING: {
			/*
			 * \note For efficiency, string interpolation should be
			 * performed by caller because it only needs to be
			 * performed when necessary.
			 */
			char *str = copyString(expr->data.s);
			if (!str) return NULL;
			return createStringValueObject(str);
		}
		default:
			error(IN_UNKNOWN_CONSTANT_TYPE);
			return NULL;
	}
}

/**
 * Interprets a logical NOT operation.
 *
 * \param [in] expr A pointer to the expression to interpret.
 *
 * \param [in] scope A pointer to a scope to evaluate \a node under.
 *
 * \note Only the first element of \a args is used.
 *
 * \return A pointer to the value of the logical negation of the first element
 * of \a args.
 *
 * \retval NULL An error occurred during interpretation.
 */
ValueObject *interpretNotOpExprNode(OpExprNode *expr,
                                    ScopeObject *scope)
{
	ValueObject *val = interpretExprNode(expr->args->exprs[0], scope);
	ValueObject *use = val;
	int retval;
	unsigned short cast = 0;
	if (!val) return NULL;
	if (val->type != VT_BOOLEAN && val->type != VT_INTEGER) {
		use = castBooleanImplicit(val, scope);
		if (!use) {
			deleteValueObject(val);
			return NULL;
		}
		cast = 1;
	}
	retval = getInteger(use);
	if (cast) deleteValueObject(use);
	deleteValueObject(val);
	return createBooleanValueObject(!retval);
}

/**
 * Adds an integer to an integer.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the sum of \a a and \a b.
 */
ValueObject *opAddIntegerInteger(ValueObject *a,
                                 ValueObject *b)
{
	return createIntegerValueObject(getInteger(a) + getInteger(b));
}

/**
 * Subtracts an integer from an integer.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the difference of \a a and \a b.
 */
ValueObject *opSubIntegerInteger(ValueObject *a,
                                 ValueObject *b)
{
	return createIntegerValueObject(getInteger(a) - getInteger(b));
}

/**
 * Multiplies an integer by an integer.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the product of \a a and \a b.
 */
ValueObject *opMultIntegerInteger(ValueObject *a,
                                  ValueObject *b)
{
	return createIntegerValueObject(getInteger(a) * getInteger(b));
}

/**
 * Divides an integer by an integer.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the quotient of \a a and \a b.
 *
 * \retval NULL Division by zero.
 */
ValueObject *opDivIntegerInteger(ValueObject *a,
                                 ValueObject *b)
{
	if (getInteger(b) == 0) {
		error(IN_DIVISION_BY_ZERO);
		return NULL;
	}
	return createIntegerValueObject(getInteger(a) / getInteger(b));
}

/**
 * Finds the maximum of an integer and an integer.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the maximum of \a a and \a b.
 */
ValueObject *opMaxIntegerInteger(ValueObject *a,
                                 ValueObject *b)
{
	return createIntegerValueObject(getInteger(a) > getInteger(b) ? getInteger(a) : getInteger(b));
}

/**
 * Finds the minimum of an integer and an integer.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the minimum of \a a and \a b.
 */
ValueObject *opMinIntegerInteger(ValueObject *a,
                                 ValueObject *b)
{
	return createIntegerValueObject(getInteger(a) < getInteger(b) ? getInteger(a) : getInteger(b));
}

/**
 * Calculates the modulus of an integer and an integer.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the modulus of \a a and \a b.
 */
ValueObject *opModIntegerInteger(ValueObject *a,
                                 ValueObject *b)
{
	if (getInteger(b) == 0) {
		error(IN_DIVISION_BY_ZERO);
		return NULL;
	}
	return createIntegerValueObject(getInteger(a) % getInteger(b));
}

/**
 * Adds an integer to a decimal.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the sum of \a a and \a b.
 */
ValueObject *opAddIntegerFloat(ValueObject *a,
                               ValueObject *b)
{
	return createFloatValueObject((float)(getInteger(a) + getFloat(b)));
}

/**
 * Subtracts an integer from a decimal.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the difference of \a a and \a b.
 */
ValueObject *opSubIntegerFloat(ValueObject *a,
                               ValueObject *b)
{
	return createFloatValueObject((float)(getInteger(a) - getFloat(b)));
}

/**
 * Multiplies an integer by a decimal.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the product of \a a and \a b.
 */
ValueObject *opMultIntegerFloat(ValueObject *a,
                                ValueObject *b)
{
	return createFloatValueObject((float)(getInteger(a) * getFloat(b)));
}

/**
 * Divides an integer by a decimal.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the quotient of \a a and \a b.
 *
 * \retval NULL Division by zero.
 */
ValueObject *opDivIntegerFloat(ValueObject *a,
                               ValueObject *b)
{
	if (fabs(getFloat(b) - 0.0) < FLT_EPSILON) {
		error(IN_DIVISION_BY_ZERO);
		return NULL;
	}
	return createFloatValueObject((float)(getInteger(a) / getFloat(b)));
}

/**
 * Finds the maximum of an integer and a decimal.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the maximum of \a a and \a b.
 */
ValueObject *opMaxIntegerFloat(ValueObject *a,
                               ValueObject *b)
{
	return createFloatValueObject((float)(getInteger(a)) > getFloat(b) ? (float)(getInteger(a)) : getFloat(b));
}

/**
 * Finds the minimum of an integer and a decimal.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the minimum of \a a and \a b.
 */
ValueObject *opMinIntegerFloat(ValueObject *a,
                               ValueObject *b)
{
	return createFloatValueObject((float)(getInteger(a)) < getFloat(b) ? (float)(getInteger(a)) : getFloat(b));
}

/**
 * Calculates the modulus of an integer and a decimal.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the modulus of \a a and \a b.
 */
ValueObject *opModIntegerFloat(ValueObject *a,
                               ValueObject *b)
{
	if (fabs(getFloat(b) - 0.0) < FLT_EPSILON) {
		error(IN_DIVISION_BY_ZERO);
		return NULL;
	}
	return createFloatValueObject((float)(fmod((double)(getInteger(a)), getFloat(b))));
}

/**
 * Adds a decimal to an integer.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the sum of \a a and \a b.
 */
ValueObject *opAddFloatInteger(ValueObject *a,
                               ValueObject *b)
{
	return createFloatValueObject(getFloat(a) + getInteger(b));
}

/**
 * Subtracts a decimal from an integer.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the difference of \a a and \a b.
 */
ValueObject *opSubFloatInteger(ValueObject *a,
                               ValueObject *b)
{
	return createFloatValueObject(getFloat(a) - getInteger(b));
}

/**
 * Multiplies a decimal by an integer.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the product of \a a and \a b.
 */
ValueObject *opMultFloatInteger(ValueObject *a,
                                ValueObject *b)
{
	return createFloatValueObject(getFloat(a) * getInteger(b));
}

/**
 * Divides a decimal by an integer.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the quotient of \a a and \a b.
 *
 * \retval NULL Division by zero.
 */
ValueObject *opDivFloatInteger(ValueObject *a,
                               ValueObject *b)
{
	if (getInteger(b) == 0) {
		error(IN_DIVISION_BY_ZERO);
		return NULL;
	}
	return createFloatValueObject(getFloat(a) / getInteger(b));
}

/**
 * Finds the maximum of a decimal and an integer.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the maximum of \a a and \a b.
 */
ValueObject *opMaxFloatInteger(ValueObject *a,
                               ValueObject *b)
{
	return createFloatValueObject(getFloat(a) > (float)(getInteger(b)) ? getFloat(a) : (float)(getInteger(b)));
}

/**
 * Finds the minimum of a decimal and an integer.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the minimum of \a a and \a b.
 */
ValueObject *opMinFloatInteger(ValueObject *a,
                               ValueObject *b)
{
	return createFloatValueObject(getFloat(a) < (float)(getInteger(b)) ? getFloat(a) : (float)(getInteger(b)));
}

/**
 * Calculates the modulus of a decimal and an integer.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the modulus of \a a and \a b.
 */
ValueObject *opModFloatInteger(ValueObject *a,
                               ValueObject *b)
{
	if (getInteger(b) == 0) {
		error(IN_DIVISION_BY_ZERO);
		return NULL;
	}
	return createFloatValueObject((float)(fmod(getFloat(a), (double)(getInteger(b)))));
}

/**
 * Adds a decimal to a decimal.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the sum of \a a and \a b.
 */
ValueObject *opAddFloatFloat(ValueObject *a,
                             ValueObject *b)
{
	return createFloatValueObject(getFloat(a) + getFloat(b));
}

/**
 * Subtracts a decimal from a decimal.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the difference of \a a and \a b.
 */
ValueObject *opSubFloatFloat(ValueObject *a,
                             ValueObject *b)
{
	return createFloatValueObject(getFloat(a) - getFloat(b));
}

/**
 * Multiplies a decimal by a decimal.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the product of \a a and \a b.
 */
ValueObject *opMultFloatFloat(ValueObject *a,
                              ValueObject *b)
{
	return createFloatValueObject(getFloat(a) * getFloat(b));
}

/**
 * Divides a decimal by a decimal.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the quotient of \a a and \a b.
 *
 * \retval NULL Division by zero.
 */
ValueObject *opDivFloatFloat(ValueObject *a,
                             ValueObject *b)
{
	if (fabs(getFloat(b) - 0.0) < FLT_EPSILON) {
		error(IN_DIVISION_BY_ZERO);
		return NULL;
	}
	return createFloatValueObject(getFloat(a) / getFloat(b));
}

/**
 * Finds the maximum of a decimal and a decimal.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the maximum of \a a and \a b.
 */
ValueObject *opMaxFloatFloat(ValueObject *a,
                             ValueObject *b)
{
	return createFloatValueObject(getFloat(a) > getFloat(b) ? getFloat(a) : getFloat(b));
}

/**
 * Finds the minimum of a decimal and a decimal.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the minimum of \a a and \a b.
 */
ValueObject *opMinFloatFloat(ValueObject *a,
                             ValueObject *b)
{
	return createFloatValueObject(getFloat(a) < getFloat(b) ? getFloat(a) : getFloat(b));
}

/**
 * Calculates the modulus of a decimal and a decimal.
 *
 * \param [in] a The first operand.
 *
 * \param [in] b The second operand.
 *
 * \return A pointer to the value of the modulus of \a a and \a b.
 */
ValueObject *opModFloatFloat(ValueObject *a,
                             ValueObject *b)
{
	if (fabs(getFloat(b) - 0.0) < FLT_EPSILON) {
		error(IN_DIVISION_BY_ZERO);
		return NULL;
	}
	return createFloatValueObject((float)(fmod(getFloat(a), getFloat(b))));
}

/*
 * A jump table for arithmetic operations.  The first index determines the
 * particular arithmetic operation to perform, the second index determines the
 * type of the first argument, and the third index determines the type of the
 * second object.
 */
static ValueObject *(*ArithOpJumpTable[7][2][2])(ValueObject *, ValueObject *) = {
	{ { opAddIntegerInteger, opAddIntegerFloat }, { opAddFloatInteger, opAddFloatFloat } },
	{ { opSubIntegerInteger, opSubIntegerFloat }, { opSubFloatInteger, opSubFloatFloat } },
	{ { opMultIntegerInteger, opMultIntegerFloat }, { opMultFloatInteger, opMultFloatFloat } },
	{ { opDivIntegerInteger, opDivIntegerFloat }, { opDivFloatInteger, opDivFloatFloat } },
	{ { opModIntegerInteger, opModIntegerFloat }, { opModFloatInteger, opModFloatFloat } },
	{ { opMaxIntegerInteger, opMaxIntegerFloat }, { opMaxFloatInteger, opMaxFloatFloat } },
	{ { opMinIntegerInteger, opMinIntegerFloat }, { opMinFloatInteger, opMinFloatFloat } }
};

/**
 * Interprets an arithmetic operation.
 *
 * \param [in] expr The operation to interpret.
 *
 * \param [in] scope The scope to evaluate \a expr under.
 *
 * \note Only supports binary arithmetic operations.
 *
 * \return A pointer to the value of the arithmetic operation.
 *
 * \retval NULL An error occurred during interpretation.
 */
ValueObject *interpretArithOpExprNode(OpExprNode *expr,
                                      ScopeObject *scope)
{
	ValueObject *val1 = interpretExprNode(expr->args->exprs[0], scope);
	ValueObject *val2 = interpretExprNode(expr->args->exprs[1], scope);
	ValueObject *use1 = val1;
	ValueObject *use2 = val2;
	unsigned int cast1 = 0;
	unsigned int cast2 = 0;
	ValueObject *ret = NULL;
	if (!val1 || !val2) {
		deleteValueObject(val1);
		deleteValueObject(val2);
		return NULL;
	}
	/* Check if a floating point decimal string and cast */
	switch (val1->type) {
		case VT_NIL:
		case VT_BOOLEAN:
			use1 = castIntegerImplicit(val1, scope);
			if (!use1) {
				deleteValueObject(val1);
				deleteValueObject(val2);
				return NULL;
			}
			cast1 = 1;
			break;
		case VT_INTEGER:
		case VT_FLOAT:
			break;
		case VT_STRING: {
			/* Perform interpolation */
			ValueObject *interp = castStringExplicit(val1, scope);
			if (!interp) {
				deleteValueObject(val1);
				deleteValueObject(val2);
				return NULL;
			}
			if (strchr(getString(interp), '.'))
				use1 = castFloatImplicit(interp, scope);
			else
				use1 = castIntegerImplicit(interp, scope);
			deleteValueObject(interp);
			if (!use1) {
				deleteValueObject(val1);
				deleteValueObject(val2);
				return NULL;
			}
			cast1 = 1;
			break;
		}
		default:
			error(IN_INVALID_OPERAND_TYPE);
	}
	switch (val2->type) {
		case VT_NIL:
		case VT_BOOLEAN:
			use2 = castIntegerImplicit(val2, scope);
			if (!use2) {
				deleteValueObject(val1);
				deleteValueObject(val2);
				if (cast1) deleteValueObject(use1);
				return NULL;
			}
			cast2 = 1;
			break;
		case VT_INTEGER:
		case VT_FLOAT:
			break;
		case VT_STRING: {
			/* Perform interpolation */
			ValueObject *interp = castStringExplicit(val2, scope);
			if (!interp) {
				deleteValueObject(val1);
				deleteValueObject(val2);
				if (cast1) deleteValueObject(use1);
				return NULL;
			}
			if (strchr(getString(interp), '.'))
				use2 = castFloatImplicit(interp, scope);
			else
				use2 = castIntegerImplicit(interp, scope);
			deleteValueObject(interp);
			if (!use2) {
				deleteValueObject(val1);
				deleteValueObject(val2);
				if (cast1) deleteValueObject(use1);
				return NULL;
			}
			cast2 = 1;
			break;
		}
		default:
			error(IN_INVALID_OPERAND_TYPE);
	}
	/* Do math depending on value types */
	ret = ArithOpJumpTable[expr->type][use1->type][use2->type](use1, use2);
	/* Clean up after floating point decimal casts */
	if (cast1) deleteValueObject(use1);
	if (cast2) deleteValueObject(use2);
	deleteValueObject(val1);
	deleteValueObject(val2);
	return ret;
}

/**
 * Interprets a boolean operation.
 *
 * \param [in] expr The operation to interpret.
 *
 * \param [in] scope The scope to evaluate \a expr under.
 *
 * \return A pointer to the value of the boolean operation.
 *
 * \retval NULL An error occurred during interpretation.
 */
ValueObject *interpretBoolOpExprNode(OpExprNode *expr,
                                     ScopeObject *scope)
{
	unsigned int n;
	int acc = 0;
	/*
	 * Proceed to apply the same operation on the accumulator for the
	 * remaining arguments.
	 */
	for (n = 0; n < expr->args->num; n++) {
		ValueObject *val = interpretExprNode(expr->args->exprs[n], scope);
		ValueObject *use = val;
		int temp;
		unsigned int cast = 0;
		if (!val) return NULL;
		if (val->type != VT_BOOLEAN && val->type != VT_INTEGER) {
			use = castBooleanImplicit(val, scope);
			if (!use) {
				deleteValueObject(val);
				return NULL;
			}
			cast = 1;
		}
		temp = getInteger(use);
		if (cast) deleteValueObject(use);
		deleteValueObject(val);
		if (n == 0) acc = temp;
		else {
			switch (expr->type) {
				case OP_AND:
					acc &= temp;
					break;
				case OP_OR:
					acc |= temp;
					break;
				case OP_XOR:
					acc ^= temp;
					break;
				default:
					error(IN_INVALID_BOOLEAN_OPERATION_TYPE);
					return NULL;
			}
		}
		/**
		 * \note The specification does not say whether boolean logic
		 * short circuits or not.  Here, we assume it does.
		 */
		if (expr->type == OP_AND && acc == 0) break;
		else if (expr->type == OP_OR && acc == 1) break;
	}
	return createBooleanValueObject(acc);
}

/**
 * Checks if an integer value is equal to another integer value.
 *
 * \param [in] a The first value to check.
 *
 * \param [in] b The second value to check.
 *
 * \return A pointer to a boolean value indicating if \a is equal to \a b.
 */
ValueObject *opEqIntegerInteger(ValueObject *a,
                                ValueObject *b)
{
	return createBooleanValueObject(getInteger(a) == getInteger(b));
}

/**
 * Checks if an integer value is not equal to another integer value.
 *
 * \param [in] a The first value to check.
 *
 * \param [in] b The second value to check.
 *
 * \return A pointer to a boolean value indicating if \a is not equal to \a b.
 */
ValueObject *opNeqIntegerInteger(ValueObject *a,
                                 ValueObject *b)
{
	return createBooleanValueObject(getInteger(a) != getInteger(b));
}

/**
 * Checks if an integer value is equal to a decimal value.
 *
 * \param [in] a The first value to check.
 *
 * \param [in] b The second value to check.
 *
 * \return A pointer to a boolean value indicating if \a is equal to \a b.
 */
ValueObject *opEqIntegerFloat(ValueObject *a,
                              ValueObject *b)
{
	return createBooleanValueObject(fabs((float)(getInteger(a)) - getFloat(b)) < FLT_EPSILON);
}

/**
 * Checks if an integer value is not equal to a decimal value.
 *
 * \param [in] a The first value to check.
 *
 * \param [in] b The second value to check.
 *
 * \return A pointer to a boolean value indicating if \a is not equal to \a b.
 */
ValueObject *opNeqIntegerFloat(ValueObject *a,
                               ValueObject *b)
{
	return createBooleanValueObject(fabs((float)(getInteger(a)) - getFloat(b)) > FLT_EPSILON);
}

/**
 * Checks if a decimal value is equal to an integer value.
 *
 * \param [in] a The first value to check.
 *
 * \param [in] b The second value to check.
 *
 * \return A pointer to a boolean value indicating if \a is equal to \a b.
 */
ValueObject *opEqFloatInteger(ValueObject *a,
                              ValueObject *b)
{
	return createBooleanValueObject(fabs(getFloat(a) - (float)(getInteger(b))) < FLT_EPSILON);
}

/**
 * Checks if a decimal value is not equal to an integer value.
 *
 * \param [in] a The first value to check.
 *
 * \param [in] b The second value to check.
 *
 * \return A pointer to a boolean value indicating if \a is not equal to \a b.
 */
ValueObject *opNeqFloatInteger(ValueObject *a,
                               ValueObject *b)
{
	return createBooleanValueObject(fabs(getFloat(a) - (float)(getInteger(b))) > FLT_EPSILON);
}

/**
 * Checks if a decimal value is equal to another decimal value.
 *
 * \param [in] a The first value to check.
 *
 * \param [in] b The second value to check.
 *
 * \return A pointer to a boolean value indicating if \a is equal to \a b.
 */
ValueObject *opEqFloatFloat(ValueObject *a,
                            ValueObject *b)
{
	return createBooleanValueObject(fabs(getFloat(a) - getFloat(b)) < FLT_EPSILON);
}

/**
 * Checks if a decimal value is not equal to another decimal value.
 *
 * \param [in] a The first value to check.
 *
 * \param [in] b The second value to check.
 *
 * \return A pointer to a boolean value indicating if \a is not equal to \a b.
 */
ValueObject *opNeqFloatFloat(ValueObject *a,
                             ValueObject *b)
{
	return createBooleanValueObject(fabs(getFloat(a) - getFloat(b)) > FLT_EPSILON);
}

/**
 * Checks if a boolean value is equal to another boolean value.
 *
 * \param [in] a The first value to check.
 *
 * \param [in] b The second value to check.
 *
 * \return A pointer to a boolean value indicating if \a is equal to \a b.
 */
ValueObject *opEqBooleanBoolean(ValueObject *a,
                                ValueObject *b)
{
	return createBooleanValueObject(getInteger(a) == getInteger(b));
}

/**
 * Checks if a boolean value is not equal to another boolean value.
 *
 * \param [in] a The first value to check.
 *
 * \param [in] b The second value to check.
 *
 * \return A pointer to a boolean value indicating if \a is not equal to \a b.
 */
ValueObject *opNeqBooleanBoolean(ValueObject *a,
                                 ValueObject *b)
{
	return createBooleanValueObject(getInteger(a) != getInteger(b));
}

/**
 * Checks if a string value is equal to another string value.
 *
 * \param [in] a The first value to check.
 *
 * \param [in] b The second value to check.
 *
 * \return A pointer to a boolean value indicating if \a is equal to \a b.
 */
ValueObject *opEqStringString(ValueObject *a,
                              ValueObject *b)
{
	return createBooleanValueObject(strcmp(getString(a), getString(b)) == 0);
}

/**
 * Checks if a string value is not equal to another string value.
 *
 * \param [in] a The first value to check.
 *
 * \param [in] b The second value to check.
 *
 * \return A pointer to a boolean value indicating if \a is not equal to \a b.
 */
ValueObject *opNeqStringString(ValueObject *a,
                               ValueObject *b)
{
	return createBooleanValueObject(strcmp(getString(a), getString(b)) != 0);
}

/**
 * Returns true because two nil values are always equal.
 *
 * \param [in] a Not used.
 *
 * \param [in] b Not used.
 *
 * \return A true boolean value.
 */
ValueObject *opEqNilNil(ValueObject *a,
                        ValueObject *b)
{
	a = NULL;
	b = NULL;
	return createBooleanValueObject(1);
}

/**
 * Returns false because two nil values are never not equal.
 *
 * \param [in] a Not used.
 *
 * \param [in] b Not used.
 *
 * \return A false boolean value.
 */
ValueObject *opNeqNilNil(ValueObject *a,
                         ValueObject *b)
{
	a = NULL;
	b = NULL;
	return createBooleanValueObject(0);
}

/*
 * A jump table for boolean operations.  The first index determines the
 * particular boolean operation to perform, the second index determines the type
 * of the first argument, and the third index determines the type of the second
 * object.
 */
static ValueObject *(*BoolOpJumpTable[2][5][5])(ValueObject *, ValueObject *) = {
	{ /* OP_EQ */
	              /* Integer, Float, Boolean, String, Nil */
	/* Integer */ { opEqIntegerInteger, opEqIntegerFloat, NULL, NULL, NULL },
	/* Float   */ { opEqFloatInteger, opEqFloatFloat, NULL, NULL, NULL },
	/* Boolean */ { NULL, NULL, opEqBooleanBoolean, NULL, NULL },
	/* String  */ { NULL, NULL, NULL, opEqStringString, NULL },
	/* Nil     */ { NULL, NULL, NULL, NULL, opEqNilNil }
	},
	{ /* OP_NEQ */
	              /* Integer, Float, Boolean, String, Nil */
	/* Integer */ { opNeqIntegerInteger, opNeqIntegerFloat, NULL, NULL, NULL },
	/* Float   */ { opNeqFloatInteger, opNeqFloatFloat, NULL, NULL, NULL },
	/* Boolean */ { NULL, NULL, opNeqBooleanBoolean, NULL, NULL },
	/* String  */ { NULL, NULL, NULL, opNeqStringString, NULL },
	/* Nil     */ { NULL, NULL, NULL, NULL, opNeqNilNil }
	}
};

/**
 * Interprets an equality operation.
 *
 * \param [in] expr The operation to interpret.
 *
 * \param [in] scope The scope to evaluate \a expr under.
 *
 * \return A pointer to the resulting value of the equality operation.
 *
 * \retval NULL An error occurred during interpretation.
 */
ValueObject *interpretEqualityOpExprNode(OpExprNode *expr,
                                         ScopeObject *scope)
{
	ValueObject *val1 = interpretExprNode(expr->args->exprs[0], scope);
	ValueObject *val2 = interpretExprNode(expr->args->exprs[1], scope);
	ValueObject *ret = NULL;
	if (!val1 || !val2) {
		deleteValueObject(val1);
		deleteValueObject(val2);
		return NULL;
	}
	/*
	 * Since there is no automatic casting, an equality (inequality) test
	 * against a non-number type will always fail (succeed).
	 */
	if ((val1->type != val2->type)
			&& ((val1->type != VT_INTEGER && val1->type != VT_FLOAT)
			|| (val2->type != VT_INTEGER && val2->type != VT_FLOAT))) {
		switch (expr->type) {
			case OP_EQ:
				ret = createBooleanValueObject(0);
				break;
			case OP_NEQ:
				ret = createBooleanValueObject(1);
				break;
			default:
				error(IN_INVALID_EQUALITY_OPERATION_TYPE);
				deleteValueObject(val1);
				deleteValueObject(val2);
				return NULL;
		}
	}
	else
		ret = BoolOpJumpTable[expr->type - OP_EQ][val1->type][val2->type](val1, val2);
	deleteValueObject(val1);
	deleteValueObject(val2);
	return ret;
}

/**
 * Interprets a concatenation operation.
 *
 * \param [in] expr The operation to interpret.
 *
 * \param [in] scope The scope to evaluate \a expr under.
 *
 * \return A pointer to the resulting value of the concatenation operation.
 *
 * \retval NULL An error occurred during interpretation.
 */
ValueObject *interpretConcatOpExprNode(OpExprNode *expr,
                                       ScopeObject *scope)
{
	unsigned int n;
	/* Start out with the first string to concatenate. */
	ValueObject *val = interpretExprNode(expr->args->exprs[0], scope);
	ValueObject *use = castStringImplicit(val, scope);
	char *acc = NULL;
	void *mem = NULL;
	if (!val || !use) {
		deleteValueObject(val);
		deleteValueObject(use);
		return NULL;
	}
	/* Start out an accumulator with the first string. */
	mem = realloc(acc, sizeof(char) * (strlen(getString(use)) + 1));
	if (!mem) {
		perror("realloc");
		deleteValueObject(val);
		deleteValueObject(use);
		free(acc);
		return NULL;
	}
	acc = mem;
	acc[0] = '\0';
	strcat(acc, getString(use));
	deleteValueObject(val);
	deleteValueObject(use);
	for (n = 1; n < expr->args->num; n++) {
		/* Grab the next string to concatenate. */
		val = interpretExprNode(expr->args->exprs[n], scope);
		use = castStringImplicit(val, scope);
		if (!val || !use) {
			deleteValueObject(val);
			deleteValueObject(use);
			free(acc);
			return NULL;
		}
		/* Add the next string to the accumulator. */
		mem = realloc(acc, sizeof(char) * (strlen(acc) + strlen(getString(use)) + 1));
		if (!mem) {
			perror("realloc");
			deleteValueObject(val);
			deleteValueObject(use);
			free(acc);
			return NULL;
		}
		acc = mem;
		strcat(acc, getString(use));
		deleteValueObject(val);
		deleteValueObject(use);
	}
	return createStringValueObject(acc);
}

/*
 * A jump table for operations.  The index of a function in the table is given
 * by its its index in the enumerated OpType type.
 */
static ValueObject *(*OpExprJumpTable[14])(OpExprNode *, ScopeObject *) = {
	interpretArithOpExprNode,
	interpretArithOpExprNode,
	interpretArithOpExprNode,
	interpretArithOpExprNode,
	interpretArithOpExprNode,
	interpretArithOpExprNode,
	interpretArithOpExprNode,
	interpretBoolOpExprNode,
	interpretBoolOpExprNode,
	interpretBoolOpExprNode,
	interpretNotOpExprNode,
	interpretEqualityOpExprNode,
	interpretEqualityOpExprNode,
	interpretConcatOpExprNode };

/**
 * Interprets an operation.
 *
 * \param [in] node The operation to interpret.
 *
 * \param [in] scope The scope to evaluate \a expr under.
 *
 * \return A pointer to the resulting value of the operation.
 *
 * \retval NULL An error occurred during interpretation.
 */
ValueObject *interpretOpExprNode(ExprNode *node,
                                 ScopeObject *scope)
{
	OpExprNode *expr = (OpExprNode *)node->expr;
	return OpExprJumpTable[expr->type](expr, scope);
}

/*
 * A jump table for expressions.  The index of a function in the table is given
 * by its its index in the enumerated ExprType type.
 */
static ValueObject *(*ExprJumpTable[7])(ExprNode *, ScopeObject *) = {
	interpretCastExprNode,
	interpretConstantExprNode,
	interpretIdentifierExprNode,
	interpretFuncCallExprNode,
	interpretOpExprNode,
	interpretImpVarExprNode,
	interpretSystemCommandExprNode };

/**
 * Interprets an expression.
 *
 * \param [in] node The expression to interpret.
 *
 * \param [in] scope The scope to evaluate \a expr under.
 *
 * \return A pointer to the value of \a expr evaluated under \a scope.
 *
 * \retval NULL An error occurred during interpretation.
 */
ValueObject *interpretExprNode(ExprNode *node,
                               ScopeObject *scope)
{
	return ExprJumpTable[node->type](node, scope);
}

/**
 * Interprets a cast statement.
 *
 * \param [in] node The statement to interpret.
 *
 * \param [in] scope The scope to evaluate \a node under.
 *
 * \pre \a node contains a statement created by createCastStmtNode().
 *
 * \return A pointer to a default return value.
 *
 * \retval NULL An error occurred during interpretation.
 */
ReturnObject *interpretCastStmtNode(StmtNode *node,
                                    ScopeObject *scope)
{
	CastStmtNode *stmt = (CastStmtNode *)node->stmt;
	ValueObject *val = getScopeValue(scope, scope, stmt->target);
	ValueObject *cast = NULL;
	if (!val) {
		IdentifierNode *id = (IdentifierNode *)(stmt->target);
		char *name = resolveIdentifierName(id, scope);
		if (name) {
			error(IN_VARIABLE_DOES_NOT_EXIST, id->fname, id->line, name);
			free(name);
		}
		return NULL;
	}
	switch(stmt->newtype->type) {
		case CT_NIL:
			if (!(cast = createNilValueObject())) return NULL;
			break;
		case CT_BOOLEAN:
			if (!(cast = castBooleanExplicit(val, scope))) return NULL;
			break;
		case CT_INTEGER:
			if (!(cast = castIntegerExplicit(val, scope))) return NULL;
			break;
		case CT_FLOAT:
			if (!(cast = castFloatExplicit(val, scope))) return NULL;
			break;
		case CT_STRING:
			if (!(cast = castStringExplicit(val, scope))) return NULL;
			break;
		case CT_ARRAY: {
			IdentifierNode *id = (IdentifierNode *)(stmt->target);
			char *name = resolveIdentifierName(id, scope);
			if (name) {
				error(IN_CANNOT_CAST_VALUE_TO_ARRAY, id->fname, id->line, name);
				free(name);
			}
			return NULL;
			break;
		}
	}
	if (!updateScopeValue(scope, scope, stmt->target, cast)) {
		deleteValueObject(cast);
		return NULL;
	}
	return createReturnObject(RT_DEFAULT, NULL);
}

/**
 * Interprets a print statement.
 *
 * \param [in] node The statement to interpret.
 *
 * \param [in] scope The scope to evaluate \a node under.
 *
 * \pre \a node contains a statement created by createPrintStmtNode().
 *
 * \return A pointer to a default return value.
 *
 * \retval NULL An error occurred during interpretation.
 */
ReturnObject *interpretPrintStmtNode(StmtNode *node,
                                     ScopeObject *scope)
{
	PrintStmtNode *stmt = (PrintStmtNode *)node->stmt;
	unsigned int n;
	for (n = 0; n < stmt->args->num; n++) {
		ValueObject *val = interpretExprNode(stmt->args->exprs[n], scope);
		ValueObject *use = castStringImplicit(val, scope);
		if (!val || !use) {
			deleteValueObject(val);
			deleteValueObject(use);
			return NULL;
		}
		printf("%s", getString(use));
		deleteValueObject(val);
		deleteValueObject(use);
	}
	if (!stmt->nonl)
		printf("\n");
	return createReturnObject(RT_DEFAULT, NULL);
}

/**
 * Interprets an input statement.
 *
 * \param [in] node The statement to interpret.
 *
 * \param [in] scope The scope to evaluate \a node under.
 *
 * \pre \a node contains a statement created by createInputStmtNode().
 *
 * \return A pointer to a default return value.
 *
 * \retval NULL An error occurred during interpretation.
 */
ReturnObject *interpretInputStmtNode(StmtNode *node,
                                     ScopeObject *scope)
{
	unsigned int size = 16;
	unsigned int cur = 0;
	char *temp = malloc(sizeof(char) * size);
	int c;
	void *mem = NULL;
	InputStmtNode *stmt = (InputStmtNode *)node->stmt;
	ValueObject *val = NULL;
	while ((c = getchar()) && !feof(stdin)) {
		/**
		 * \note The specification is unclear as to the exact semantics
		 * of input.  Here, we read up until the first newline or EOF
		 * but do not store it.
		 */
		if (c == EOF || c == (int)'\r' || c == (int)'\n') break;
		if (cur > size - 1) {
			/* Increasing buffer size. */
			size *= 2;
			mem = realloc(temp, sizeof(char) * size);
			if (!mem) {
				perror("realloc");
				free(temp);
				return NULL;
			}
			temp = mem;
		}
		temp[cur] = (char)c;
		cur++;
	}
	temp[cur] = '\0';
	val = createStringValueObject(temp);
	if (!val) {
		free(temp);
		return NULL;
	}
	if (!updateScopeValue(scope, scope, stmt->target, val)) {
		deleteValueObject(val);
		return NULL;
	}
	return createReturnObject(RT_DEFAULT, NULL);
}

/**
 * Interprets an assignment statement.
 *
 * \param [in] node The statement to interpret.
 *
 * \param [in] scope The scope to evaluate \a node under.
 *
 * \pre \a node contains a statement created by createAssignmentStmtNode().
 *
 * \return A pointer to a default return value.
 *
 * \retval NULL An error occurred during interpretation.
 */
ReturnObject *interpretAssignmentStmtNode(StmtNode *node,
                                          ScopeObject *scope)
{
	AssignmentStmtNode *stmt = (AssignmentStmtNode *)node->stmt;
	ValueObject *val = interpretExprNode(stmt->expr, scope);
	if (!val) return NULL;
	if (!updateScopeValue(scope, scope, stmt->target, val)) {
		deleteValueObject(val);
		return NULL;
	}
	return createReturnObject(RT_DEFAULT, NULL);
}

/**
 * Interprets a declaration statement.
 *
 * \param [in] node The statement to interpret.
 *
 * \param [in] scope The scope to evaluate \a node under.
 *
 * \pre \a node contains a statement created by createDeclarationStmtNode().
 *
 * \return A pointer to a default return value.
 *
 * \retval NULL An error occurred during interpretation.
 */
ReturnObject *interpretDeclarationStmtNode(StmtNode *node,
                                           ScopeObject *scope)
{
	DeclarationStmtNode *stmt = (DeclarationStmtNode *)node->stmt;
	ValueObject *init = NULL;
	ScopeObject *dest = NULL;
	dest = getScopeObject(scope, scope, stmt->scope);
	if (!dest) return NULL;
	if (getScopeValueLocal(scope, dest, stmt->target)) {
		IdentifierNode *id = (IdentifierNode *)(stmt->target);
		char *name = resolveIdentifierName(id, scope);
		if (name) {
			error(IN_REDEFINITION_OF_VARIABLE, id->fname, id->line, name);
			free(name);
		}
		return NULL;
	}
	if (stmt->expr)
		init = interpretExprNode(stmt->expr, scope);
	else if (stmt->type) {
		switch (stmt->type->type) {
			case CT_NIL:
				init = createNilValueObject();
				break;
			case CT_BOOLEAN:
				init = createBooleanValueObject(0);
				break;
			case CT_INTEGER:
				init = createIntegerValueObject(0);
				break;
			case CT_FLOAT:
				init = createFloatValueObject(0.0);
				break;
			case CT_STRING:
				init = createStringValueObject(copyString(""));
				break;
			case CT_ARRAY:
				init = createArrayValueObject(scope);
				break;
			default:
				error(IN_INVALID_DECLARATION_TYPE);
				return NULL;
		}
	}
	else if (stmt->parent) {
		ScopeObject *parent = getScopeObject(scope, scope, stmt->parent);
		if (!parent) return NULL;
		init = createArrayValueObject(parent);
	}
	else
		init = createNilValueObject();
	if (!init) return NULL;
	if (!createScopeValue(scope, dest, stmt->target)) {
		deleteValueObject(init);
		return NULL;
	}
	if (!updateScopeValue(scope, dest, stmt->target, init)) {
		deleteValueObject(init);
		return NULL;
	}
	return createReturnObject(RT_DEFAULT, NULL);
}

/**
 * Interprets an if/then/else statement.
 *
 * \param [in] node The statement to interpret.
 *
 * \param [in] scope The scope to evaluate \a node under.
 *
 * \pre \a node contains a statement created by createIfThenElseStmtNode().
 *
 * \return A pointer to a default return value.
 *
 * \retval NULL An error occurred during interpretation.
 */
ReturnObject *interpretIfThenElseStmtNode(StmtNode *node,
                                          ScopeObject *scope)
{
	IfThenElseStmtNode *stmt = (IfThenElseStmtNode *)node->stmt;
	ValueObject *use1 = scope->impvar;
	int use1val;
	unsigned int cast1 = 0;
	BlockNode *path = NULL;
	if (scope->impvar->type != VT_BOOLEAN && scope->impvar->type != VT_INTEGER) {
		use1 = castBooleanImplicit(scope->impvar, scope);
		if (!use1) return NULL;
		cast1 = 1;
	}
	use1val = getInteger(use1);
	if (cast1) deleteValueObject(use1);
	/* Determine which block of code to execute */
	if (use1val)
		path = stmt->yes;
	else {
		unsigned int n;
		for (n = 0; n < stmt->guards->num; n++) {
			ValueObject *val = interpretExprNode(stmt->guards->exprs[n], scope);
			ValueObject *use2 = val;
			int use2val;
			unsigned int cast2 = 0;
			if (!val) return NULL;
			if (val->type != VT_BOOLEAN && val->type != VT_INTEGER) {
				use2 = castBooleanImplicit(val, scope);
				if (!use2) {
					deleteValueObject(val);
					return NULL;
				}
				cast2 = 1;
			}
			use2val = getInteger(use2);
			deleteValueObject(val);
			if (cast2) deleteValueObject(use2);
			if (use2val) {
				path = stmt->blocks->blocks[n];
				break;
			}
		}
		/* Reached the end without satisfying any guard */
		if (n == stmt->guards->num)
			path = stmt->no;
	}
	/* Interpret a path if one was reached */
	if (path) {
		ReturnObject *r = interpretBlockNode(path, scope);
		if (!r)
			return NULL;
		/* Pass this up to the outer block to handle. */
		else if (r->type == RT_BREAK || r->type == RT_RETURN)
			return r;
		else
			deleteReturnObject(r);
	}
	return createReturnObject(RT_DEFAULT, NULL);
}

/**
 * Interprets a switch statement.
 *
 * \param [in] node The statement to interpret.
 *
 * \param [in] scope The scope to evaluate \a node under.
 *
 * \pre \a node contains a statement created by createSwitchStmtNode().
 *
 * \note The specification is unclear as to whether guards are implicitly cast
 * to the type of the implicit variable.  This only matters in the case that
 * mixed guard types are present, and in this code, the action that is performed
 * is the same as the comparison operator, that is, in order for a guard to
 * match, both its type and value must match the implicit variable.
 *
 * \return A pointer to a default return value.
 *
 * \retval NULL An error occurred during interpretation.
 */
ReturnObject *interpretSwitchStmtNode(StmtNode *node,
                                      ScopeObject *scope)
{
	SwitchStmtNode *stmt = (SwitchStmtNode *)node->stmt;
	unsigned int n;
	/*
	 * Loop over each of the guards, checking if any match the implicit
	 * variable.
	 */
	for (n = 0; n < stmt->guards->num; n++) {
		ValueObject *use1 = scope->impvar;
		ValueObject *use2 = interpretExprNode(stmt->guards->exprs[n], scope);
		unsigned int done = 0;
		if (!use2) return NULL;
		if (use1->type == use2->type) {
			switch (use1->type) {
				case VT_NIL:
					break;
				case VT_BOOLEAN:
				case VT_INTEGER:
					if (getInteger(use1) == getInteger(use2))
						done = 1;
					break;
				case VT_FLOAT:
					if (fabs(getFloat(use1) - getFloat(use2)) < FLT_EPSILON)
						done = 1;
					break;
				case VT_STRING:
					/**
					 * \note Strings with interpolation
					 * should have already been checked for.
					 */
					if (!strcmp(getString(use1), getString(use2)))
						done = 1;
					break;
				default:
					error(IN_INVALID_TYPE);
					deleteValueObject(use2);
					return NULL;
			}
		}
		deleteValueObject(use2);
		if (done) break;
	}
	/* If none of the guards match and a default block exists */
	if (n == stmt->blocks->num && stmt->def) {
		ReturnObject *r = interpretBlockNode(stmt->def, scope);
		if (!r)
			return NULL;
		else if (r->type == RT_RETURN)
			return r;
		else
			deleteReturnObject(r);
	}
	else {
		/*
		 * Keep interpreting blocks starting at n until a break or
		 * return is encountered.
		 */
		for (; n < stmt->blocks->num; n++) {
			ReturnObject *r = interpretBlockNode(stmt->blocks->blocks[n], scope);
			if (!r)
				return NULL;
			else if (r->type == RT_BREAK) {
				deleteReturnObject(r);
				break;
			}
			else if (r->type == RT_RETURN)
				return r;
			else
				deleteReturnObject(r);
		}
	}
	return createReturnObject(RT_DEFAULT, NULL);
}

/**
 * Interprets a break statement.
 *
 * \param [in] node Not used (see note).
 *
 * \param [in] scope Not used (see note).
 *
 * \pre \a node contains a statement created by createStmtNode() with arguments
 * ST_BREAK and NULL.
 *
 * \note \a node and \a scope are not used by this function but are still
 * included in its prototype to allow this function to be stored in a jump table
 * for fast execution.
 *
 * \return A pointer to a break return value.
 *
 * \retval NULL An error occurred during interpretation.
 */
ReturnObject *interpretBreakStmtNode(StmtNode *node,
                                     ScopeObject *scope)
{
	node = NULL;
	scope = NULL;
	return createReturnObject(RT_BREAK, NULL);
}

/**
 * Interprets a return statement.
 *
 * \param [in] node The statement to interpret.
 *
 * \param [in] scope The scope to evaluate \a node under.
 *
 * \pre \a node contains a statement created by createReturnStmtNode().
 *
 * \return A pointer to a return value of \a node interpreted under \a scope.
 *
 * \retval NULL An error occurred during interpretation.
 */
ReturnObject *interpretReturnStmtNode(StmtNode *node,
                                      ScopeObject *scope)
{
	/* Evaluate and return the expression. */
	ReturnStmtNode *stmt = (ReturnStmtNode *)node->stmt;
	ValueObject *value = interpretExprNode(stmt->value, scope);
	if (!value) return NULL;
	return createReturnObject(RT_RETURN, value);
}

/**
 * Interprets a loop statement.
 *
 * \param [in] node The statement to interpret.
 *
 * \param [in] scope The scope to evaluate \a node under.
 *
 * \pre \a node contains a statement created by createLoopStmtNode().
 *
 * \return A pointer to a return value of \a node interpreted under \a scope.
 *
 * \retval NULL An error occurred during interpretation.
 */
ReturnObject *interpretLoopStmtNode(StmtNode *node,
                                    ScopeObject *scope)
{
	LoopStmtNode *stmt = (LoopStmtNode *)node->stmt;
	ScopeObject *outer = createScopeObject(scope);
	ValueObject *var = NULL;
	if (!outer) return NULL;
	/* Create a temporary loop variable if required */
	if (stmt->var) {
		var = createScopeValue(scope, outer, stmt->var);
		if (!var) {
			deleteScopeObject(outer);
			return NULL;
		}
		var->type = VT_INTEGER;
		var->data.i = 0;
		var->semaphore = 1;
	}
	while (1) {
		if (stmt->guard) {
			ValueObject *val = interpretExprNode(stmt->guard, outer);
			ValueObject *use = val;
			unsigned short cast = 0;
			int guardval;
			if (val->type != VT_BOOLEAN && val->type != VT_INTEGER) {
				use = castBooleanImplicit(val, scope);
				if (!use) {
					deleteScopeObject(outer);
					deleteValueObject(val);
					return NULL;
				}
				cast = 1;
			}
			guardval = getInteger(use);
			if (cast) deleteValueObject(use);
			deleteValueObject(val);
			if (guardval == 0) break;
		}
		if (stmt->body) {
			ReturnObject *result = interpretBlockNode(stmt->body, outer);
			if (!result) {
				deleteScopeObject(outer);
				return NULL;
			}
			else if (result->type == RT_BREAK) {
				deleteReturnObject(result);
				break;
			}
			else if (result->type == RT_RETURN) {
				deleteScopeObject(outer);
				return result;
			}
			else
				deleteReturnObject(result);
		}
		if (stmt->update) {
			/*
			 * A little efficiency hack: if we know the operation to
			 * perform, don't bother evaluating the ExprNode
			 * structure, just go ahead and do it to the loop
			 * variable.
			 */
			if (stmt->update->type == ET_OP) {
				var = getScopeValue(scope, outer, stmt->var);
				OpExprNode *op = (OpExprNode *)stmt->update->expr;
				if (op->type == OP_ADD)
					var->data.i++;
				else if (op->type == OP_SUB)
					var->data.i--;
			}
			else {
				ValueObject *update = interpretExprNode(stmt->update, outer);
				if (!update) {
					deleteScopeObject(outer);
					return NULL;
				}
				if (!updateScopeValue(scope, outer, stmt->var, update)) {
					deleteScopeObject(outer);
					deleteValueObject(update);
					return NULL;
				}
			}
		}
	}
	deleteScopeObject(outer);
	return createReturnObject(RT_DEFAULT, NULL);
}

/**
 * Interprets a deallocation statement.
 *
 * \param [in] node The statement to interpret.
 *
 * \param [in] scope The scope to evaluate \a node under.
 *
 * \pre \a node contains a statement created by createDeallocationStmtNode().
 *
 * \return A pointer to a default return value.
 *
 * \retval NULL An error occurred during interpretation.
 */
ReturnObject *interpretDeallocationStmtNode(StmtNode *node,
                                            ScopeObject *scope)
{
	DeallocationStmtNode *stmt = (DeallocationStmtNode *)node->stmt;
	if (!updateScopeValue(scope, scope, stmt->target, NULL)) return NULL;
	/* If we want to completely remove the variable, use:
	deleteScopeValue(scope, stmt->target);
	*/
	return createReturnObject(RT_DEFAULT, NULL);
}

/**
 * Interprets a function definition statement.
 *
 * \param [in] node The statement to interpret.
 *
 * \param [in] scope The scope to evaluate \a node under.
 *
 * \pre \a node contains a statement created by createFuncDefStmtNode().
 *
 * \return A pointer to a default return value.
 *
 * \retval NULL An error occurred during interpretation.
 */
ReturnObject *interpretFuncDefStmtNode(StmtNode *node,
                                       ScopeObject *scope)
{
	/* Add the function to the current scope */
	FuncDefStmtNode *stmt = (FuncDefStmtNode *)node->stmt;
	ValueObject *init = NULL;
	ScopeObject *dest = NULL;

	dest = getScopeObject(scope, scope, stmt->scope);
	if (!dest) return NULL;
	if (getScopeValueLocal(scope, dest, stmt->name)) {
		IdentifierNode *id = (IdentifierNode *)(stmt->name);
		char *name = resolveIdentifierName(id, scope);
		if (name) {
			error(IN_FUNCTION_NAME_USED_BY_VARIABLE, id->fname, id->line, name);
			free(name);
		}
		return NULL;
	}
	init = createFunctionValueObject(stmt);
	if (!init) return NULL;
	if (!createScopeValue(scope, dest, stmt->name)) {
		deleteValueObject(init);
		return NULL;
	}
	if (!updateScopeValue(scope, dest, stmt->name, init)) {
		deleteValueObject(init);
		return NULL;
	}
	return createReturnObject(RT_DEFAULT, NULL);
}

/**
 * Interprets an expression statement.
 *
 * \param [in] node The statement to interpret.
 *
 * \param [in] scope The scope to evaluate \a node under.
 *
 * \pre \a node contains a statement created by createExprNode().
 *
 * \post The implicit variable of \a scope will be set the the value of \a node
 * evaluated under \a scope.
 *
 * \return A pointer to a default return value.
 *
 * \retval NULL An error occurred during interpretation.
 */
ReturnObject *interpretExprStmtNode(StmtNode *node,
                                    ScopeObject *scope)
{
	/* Set the implicit variable to the result of the expression */
	ExprNode *expr = (ExprNode *)node->stmt;
	deleteValueObject(scope->impvar);
	scope->impvar = interpretExprNode(expr, scope);
	if (!scope->impvar) return NULL;
	return createReturnObject(RT_DEFAULT, NULL);
}

/**
 * Interprets an alternate array definition statement.
 *
 * \param [in] node The statement to interpret.
 *
 * \param [in] scope The scope to evaluate \a node under.
 *
 * \pre \a node contains a statement created by createAltArrayDefStmtNode().
 *
 * \return A pointer to a default return value.
 *
 * \retval NULL An error occurred during interpretation.
 */
ReturnObject *interpretAltArrayDefStmtNode(StmtNode *node,
                                           ScopeObject *scope)
{
	AltArrayDefStmtNode *stmt = (AltArrayDefStmtNode *)node->stmt;
	ValueObject *init = NULL;
	ScopeObject *dest = scope;
	ReturnObject *ret = NULL;
	if (getScopeValueLocal(scope, dest, stmt->name)) {
		IdentifierNode *id = (IdentifierNode *)(stmt->name);
		char *name = resolveIdentifierName(id, scope);
		if (name) {
			fprintf(stderr, "%s:%u: redefinition of existing variable at: %s\n", id->fname, id->line, name);
			free(name);
		}
		return NULL;
	}
	if (stmt->parent) {
		ScopeObject *parent = getScopeObject(scope, scope, stmt->parent);
		if (!parent) return NULL;
		init = createArrayValueObject(parent);
	}
	else {
		init = createArrayValueObject(scope);
	}
	if (!init) return NULL;

	/* Populate the array body */
	ret = interpretStmtNodeList(stmt->body->stmts, getArray(init));
	if (!ret) {
		deleteValueObject(init);
		return NULL;
	}
	deleteReturnObject(ret);
	if (!createScopeValue(scope, dest, stmt->name)) {
		deleteValueObject(init);
		return NULL;
	}
	if (!updateScopeValue(scope, dest, stmt->name, init)) {
		deleteValueObject(init);
		return NULL;
	}
	return createReturnObject(RT_DEFAULT, NULL);
}

/**
 * Interprets a binding statement.
 *
 * \param [in] node The statement to interpret.
 *
 * \param [in] scope The scope to evaluate \a node under.
 *
 * \pre \a node contains a statement created by createBindingStmtNode().
 *
 * \return A pointer to a default return value.
 *
 * \retval NULL An error occurred during interpretation.
 */
ReturnObject *interpretBindingStmtNode(StmtNode *node,
                                       ScopeObject *scope)
{
	BindingStmtNode *stmt = (BindingStmtNode *)node->stmt;
	return (stmt->binding)(scope);
}

/**
 * Interprets a library import statement.
 *
 * \param [in] node The statement to interpret.
 *
 * \param [in] scope The scope to evaluate \a node under.
 *
 * \pre \a node contains a statement created by createImportStmtNode().
 *
 * \return A pointer to a default return value.
 *
 * \retval NULL An error occurred during interpretation.
 */
ReturnObject *interpretImportStmtNode(StmtNode *node,
                                      ScopeObject *scope)
{
	ImportStmtNode *stmt = (ImportStmtNode *)node->stmt;
	loadLibrary(scope, stmt->name);
	return createReturnObject(RT_DEFAULT, NULL);
}

/*
 * A jump table for statements.  The index of a function in the table is given
 * by its its index in the enumerated StmtType type.
 */
static ReturnObject *(*StmtJumpTable[16])(StmtNode *, ScopeObject *) = {
	interpretCastStmtNode,
	interpretPrintStmtNode,
	interpretInputStmtNode,
	interpretAssignmentStmtNode,
	interpretDeclarationStmtNode,
	interpretIfThenElseStmtNode,
	interpretSwitchStmtNode,
	interpretBreakStmtNode,
	interpretReturnStmtNode,
	interpretLoopStmtNode,
	interpretDeallocationStmtNode,
	interpretFuncDefStmtNode,
	interpretExprStmtNode,
	interpretAltArrayDefStmtNode,
	interpretBindingStmtNode,
	interpretImportStmtNode };

/**
 * Interprets a statement.
 *
 * \param [in] node The statement to interpret.
 *
 * \param [in] scope The scope to evaluate \a node under.
 *
 * \pre \a node contains a statement created by parseStmtNode().
 *
 * \return A pointer to a return value set appropriately depending on the
 * statement interpreted.
 *
 * \retval NULL An error occurred during interpretation.
 */
ReturnObject *interpretStmtNode(StmtNode *node,
                                ScopeObject *scope)
{
	return StmtJumpTable[node->type](node, scope);
}

/**
 * Interprets a list of statements.
 *
 * \param [in] list The statements to interpret.
 *
 * \param [in] scope The scope to evaluate \a list under.
 *
 * \return A pointer to a return value set appropriately depending on the
 * statements interpreted.
 *
 * \retval NULL An error occurred during interpretation.
 */
ReturnObject *interpretStmtNodeList(StmtNodeList *list,
                                    ScopeObject *scope)
{
	ReturnObject *ret = NULL;
	unsigned int n;
	for (n = 0; n < list->num; n++) {
		ret = interpretStmtNode(list->stmts[n], scope);
		if (!ret)
			return NULL;
		else if (ret->type == RT_BREAK || ret->type == RT_RETURN)
			return ret;
		else {
			deleteReturnObject(ret);
			ret = NULL;
		}
	}
	if (!ret) ret = createReturnObject(RT_DEFAULT, NULL);
	return ret;
}

/**
 * Interprets a block of code.
 *
 * \param [in] node The block of code to interpret.
 *
 * \param [in] scope The scope to evaluate \a block under.
 *
 * \pre \a block contains a block of code created by parseBlockNode().
 *
 * \return A pointer to a return value set appropriately depending on the
 * statements interpreted.
 *
 * \retval NULL An error occurred during interpretation.
 */
ReturnObject *interpretBlockNode(BlockNode *node,
                                 ScopeObject *scope)
{
	ReturnObject *ret = NULL;
	ScopeObject *inner = createScopeObject(scope);
	if (!inner) return NULL;
	ret = interpretStmtNodeList(node->stmts, inner);
	deleteScopeObject(inner);
	return ret;
}

/**
 * Interprets a the main block of code.
 *
 * \param [in] main The main block of code to interpret.
 *
 * \param [in] scope The scope to evaluate \a main under.
 *
 * \pre \a main contains a block of code created by parseMainNode().
 *
 * \return The final status of the program.
 *
 * \retval 0 \a main was interpreted without any errors.
 *
 * \retval 1 An error occurred while interpreting \a main.
 */
int interpretMainNodeScope(MainNode *main, ScopeObject *scope)
{
	ReturnObject *ret = NULL;
	if (!main) return 1;
	ret = interpretBlockNode(main->block, scope);
	if (!ret) return 1;
	deleteReturnObject(ret);
	return 0;
}
