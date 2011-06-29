#include "interpreter.h"

/** Creates a string by copying the contents of another string.
  *
  * \return A pointer to a string containing the copied contents of \a data.
  *
  * \retval NULL malloc was unable to allocate memory. */
char *createString(char *data) /**< [in] A pointer to the string data to store. */
{
	char *p = malloc(sizeof(char) * (strlen(data) + 1));
	if (!p) {
		perror("malloc");
		return NULL;
	}
	strcpy(p, data);
	return p;
}

/** Creates a nil type ValueObject structure.
  *
  * \return A pointer to a nil type ValueObject structure.
  *
  * \retval NULL malloc was unable to allocate memory. */
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

/** Creates a boolean type ValueObject structure.
  *
  * \return A pointer to a boolean type ValueObject structure with value
  *         \c 0 if \a data equals 0 and \c 1 otherwise.
  *
  * \retval NULL malloc was unable to allocate memory. */
ValueObject *createBooleanValueObject(int data) /**< [in] The boolean data to store. */
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

/** Creates an integer type ValueObject structure.
  *
  * \return A pointer to an integer type ValueObject structure with value
  *         \a data.
  *
  * \retval NULL malloc was unable to allocate memory. */
ValueObject *createIntegerValueObject(int data) /**< [in] The integer data to store. */
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

/** Creates a floating point data decimal type ValueObject structure.
  *
  * \return A pointer to a floating point decimal type ValueObject structure
  *         with value \a data.
  *
  * \retval NULL malloc was unable to allocate memory. */
ValueObject *createFloatValueObject(float data) /**< [in] The floating point data to store. */
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

/** Creates a string type ValueObject structure.
  *
  * \return A pointer to a string type ValueObject structure with value \a data.
  *
  * \retval NULL malloc was unable to allocate memory. */
ValueObject *createStringValueObject(char *data) /**< [in] The string data to store. */
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

/** Copies a ValueObject structure.
  *
  * \note What this function actually does is increment a semaphore in \a value
  *       and return \a value.  The semaphore gets decremented when \a value
  *       gets deleted by deleteValueObject(ValueObject *).  This way, an
  *       immutable copy of a ValueObject structure may be made without actaully
  *       copying its blocks of memory; this reduces the overhead associated
  *       with copying a ValueObject strcuture while still preserving its
  *       functionality.
  *
  * \pre \a value was created by either createNilValueObject(void), createBooleanValueObject(int),
  *      createIntegerValueObject(int), createFloatValueObject(float), createStringValueObject(char *),
  *      or copied with copyValueObject(ValueObject *).
  * 
  * \return A pointer to a ValueObject with the same type and contents as
  *         \a value.
  *
  * \retval NULL The type of \a value is unknown.
  *
  * \see createNilValueObject(void)
  * \see createBooleanValueObject(int)
  * \see createIntegerValueObject(int)
  * \see createFloatValueObject(float)
  * \see createStringValueObject(char *)
  * \see deleteValueObject(ValueObject *) */
ValueObject *copyValueObject(ValueObject *value) /**< [in,out] The ValueObject structure to create a copy of. */
{
	V(value);
	return value;
}

/** Deletes a ValueObject structure.
  * 
  * \pre \a value was created by either createNilValueObject(void), createBooleanValueObject(int),
  *      createIntegerValueObject(int), createFloatValueObject(float), createStringValueObject(char *),
  *      or copied with copyValueObject(ValueObject *).
  *
  * \post The memory at \a value and any of its associated members will be
  *       freed (see note).
  *
  * \note What this function actually does is decrement a semaphore in \a value
  *       and delete \a value if the semaphore reaches 0 as a result of the
  *       decrement.  The semaphore gets incremented when either the value is
  *       created or it gets copied by copyValueObject(ValueObject *).  This
  *       way, an immutable copy of a ValueObject structure may be made without
  *       actually copying its blocks of memory.
  *
  * \see createNilValueObject(void)
  * \see createBooleanValueObject(int)
  * \see createIntegerValueObject(int)
  * \see createFloatValueObject(float)
  * \see createStringValueObject(char *)
  * \see copyValueObject(ValueObject *) */
void deleteValueObject(ValueObject *value) /**< [in,out] A pointer to the ValueObject structure to be deleted. */
{
	if (!value) return;
	P(value);
	if (!value->semaphore) {
		if (value->type == VT_STRING) free(value->data.s);
		free(value);
	}
}

/** Creates a ReturnObject structure.
  *
  * \pre \a value was created by either createNilValueObject(void), createBooleanValueObject(int),
  *      createIntegerValueObject(int), createFloatValueObject(float), createStringValueObject(char *),
  *      or copied with copyValueObject(ValueObject *).
  *
  * \return A pointer to a ReturnObject structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteReturnObject(ReturnObject *) */
ReturnObject *createReturnObject(ReturnType type,    /**< [in] The type of return encountered. */
                                 ValueObject *value) /**< [in] A pointer to the ValueObject structure specifying the return value (optional, \c NULL if unused). */
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

/** Deletes a ReturnObject structure.
  *
  * \pre \a object was created by createReturnObject(ReturnType, ValueObject *).
  *
  * \post The memory at \a object and any of its associated members will be
  *       freed.
  *
  * \see createReturnObject(ReturnType, ValueObject *) */
void deleteReturnObject(ReturnObject *object) /**< [in,out] The ReturnObject structure to be deleted. */
{
	if (!object) return;
	if (object->type == RT_RETURN)
		deleteValueObject(object->value);
	free(object);
}

/** Creates a ScopeObject structure.
  *
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *)
  *      or is \c NULL if creating the root parent.
  *
  * \return A pointer to a ScopeObject structure with the desired properties.
  *
  * \retval NULL malloc was unable to allocate memory.
  *
  * \see deleteScopeObject(ScopeObject *) */
ScopeObject *createScopeObject(ScopeObject *parent) /**< [in] A pointer to the parent ScopeObject. */
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
	return p;
}

/** Deletes a ScopeObject structure.
  *
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \post The memory at \a scope and any of its associated members will be
  *       freed.
  *
  * \see createScopeObject(ScopeObject *) */
void deleteScopeObject(ScopeObject *scope) /**< [in,out] The ScopeObject structure to delete. */
{
	unsigned int n;
	if (!scope) return;
	for (n = 0; n < scope->numvals; n++) {
		/* The individual names are pointers to existing IdentifierNode
		 * structures, so don't free them here. */
		deleteValueObject(scope->values[n]);
	}
	free(scope->names);
	free(scope->values);
	deleteValueObject(scope->impvar);
	free(scope);
}

/** Retrieves a named ValueObject structure from a ScopeObject structure,
  * traversing its parents if necessary.
  *
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  * \pre \a target was created by createIdentifierNode(char *).
  *
  * \return A pointer to the stored ValueObject structure named by \a target.
  *
  * \retval NULL \a target could not be found in \a scope.
  *
  * \see getLocalScopeValue(ScopeObject *, IdentifierNode *)
  * \see createScopeValue(ScopeObject *, IdentifierNode *)
  * \see updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *) */
ValueObject *getScopeValue(ScopeObject *scope,     /**< [in] The ScopeObject structure to check. */
                           IdentifierNode *target) /**< [in] The name of the value to find. */
{
	ScopeObject *current = scope;
	/* Traverse upwards through scopes */
	do {
		unsigned int n;
		/* Check for value in current scope */
		for (n = 0; n < current->numvals; n++) {
			if (!strcmp(current->names[n]->image, target->image))
				return current->values[n];
		}
	} while ((current = current->parent));
	return NULL;
}

/** Retrieves a named ValueObject structure from a ScopeObject structure without
  * traversing through its parents.
  *
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  * \pre \a target was created by createIdentifierNode(char *).
  *
  * \return A pointer to the stored ValueObject structure named by \a target.
  *
  * \retval NULL \a target could not be found in \a scope.
  *
  * \see getScopeValue(ScopeObject *, IdentifierNode *)
  * \see createScopeValue(ScopeObject *, IdentifierNode *)
  * \see updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *) */
ValueObject *getLocalScopeValue(ScopeObject *scope,     /**< [in] The ScopeObject structure to check. */
                                IdentifierNode *target) /**< [in] The name of the value to find. */
{
	unsigned int n;
	/* Check for value in current scope */
	for (n = 0; n < scope->numvals; n++) {
		if (!strcmp(scope->names[n]->image, target->image))
			return scope->values[n];
	}
	return NULL;
}

/** Creates a new, nil, named ValueObject structure in a ScopeObject structure.
  *
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  * \pre \a target was created by createIdentifierNode(char *).
  *
  * \return A pointer to the newly created ValueObject structure named by
  *         \a target.
  *
  * \retval NULL realloc was unable to allocate memory.
  *
  * \see getScopeValue(ScopeObject *, IdentifierNode *)
  * \see getLocalScopeValue(ScopeObject *, IdentifierNode *)
  * \see updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *) */
ValueObject *createScopeValue(ScopeObject *scope,     /**< [in,out] The ScopeObject structure to add a value to. */
                              IdentifierNode *target) /**< [in] The name of the value to add. */
{
	unsigned int newnumvals = scope->numvals + 1;
	void *mem1 = NULL, *mem2 = NULL;
	/* Add value to local scope */
	mem1 = realloc(scope->names, sizeof(IdentifierNode *) * newnumvals);
	if (!mem1) {
		perror("realloc");
		return NULL;
	}
	mem2 = realloc(scope->values, sizeof(ValueObject *) * newnumvals);
	if (!mem2) {
		perror("realloc");
		return NULL;
	}
	scope->names = mem1;
	scope->values = mem2;
	scope->names[scope->numvals] = target;
	scope->values[scope->numvals] = createNilValueObject();
	if (!scope->values[scope->numvals])
		return NULL;
	scope->numvals = newnumvals;
	return scope->values[scope->numvals - 1];
}

/** Updates a ValueObject structure named by an IdentifierNode structure in a
  * ScopeObject structure.
  *
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  * \pre \a target was created by createIdentifierNode(char *).
  * \pre The value named by \a target was created by createScopeValue(ScopeObject *, IdentifierNode *).
  * \pre \a value was created by either createNilValueObject(void), createBooleanValueObject(int),
  *      createIntegerValueObject(int), createFloatValueObject(float), createStringValueObject(char *),
  *      or copied with copyValueObject(ValueObject *).
  *
  * \return A pointer to the updated ValueObject structure named by \a target
  *         (will be the same as \a val).
  *
  * \retval NULL \a target could not be found in \a scope.
  *
  * \see getScopeValue(ScopeObject *, IdentifierNode *)
  * \see getLocalScopeValue(ScopeObject *, IdentifierNode *)
  * \see createScopeValue(ScopeObject *, IdentifierNode *) */
ValueObject *updateScopeValue(ScopeObject *scope,     /**< [in,out] A pointer to the ScopeObject structure to update. */
                              IdentifierNode *target, /**< [in] A pointer to the IdentifierNode structure containing the name of the value to update. */
                              ValueObject *value)     /**< [in] A pointer to the ValueObject structure containing the value to copy for the update. */
{
	ScopeObject *current = scope;
	/* Traverse upwards through scopes */
	do {
		unsigned int n;
		/* Check for existing value in current scope */
		for (n = 0; n < current->numvals; n++) {
			if (!strcmp(current->names[n]->image, target->image)) {
				/* Wipe out the old value */
				deleteValueObject(current->values[n]);
				/* Assign the new value */
				if (value)
					current->values[n] = value;
				else
					current->values[n] = createNilValueObject();
				return current->values[n];
			}
		}
	} while ((current = current->parent));
	fprintf(stderr, "%s:%d: unable to store variable at: %s\n", target->fname, target->line, target->image);
	return NULL;
}

/** Checks if a string of characters follows the format for a number.
  *
  * \retval 0 The string of characters is not a number.
  * \retval 1 The string of characters is a number. */
unsigned int isNumString(const char *stringdata) /**< [in] The array of characters to check the format of. */
{
	unsigned int n;
	unsigned int len = strlen(stringdata);
	/* Check for empty string */
	if (len == 0) return 0;
	/* Check for non-digit, non-hyphen, and non-period characters */
	for (n = 0; n < len; n++) {
		if (!isdigit(stringdata[n])
				&& stringdata[n] != '.'
				&& stringdata[n] != '-')
			return 0;
	}
	return 1;
}

/** Checks if a string of characters follows the format for a hexadecimal
  * number.
  *
  * \retval 0 The string of characters is not a hexadecimal number.
  * \retval 1 The string of characters is a hexadecimal number. */
unsigned int isHexString(const char *stringdata) /**< [in] The array of characters to check the format of. */
{
	unsigned int n;
	unsigned int len = strlen(stringdata);
	/* Check for empty string */
	if (len == 0) return 0;
	/* Check for non-digit and non-A-through-F characters */
	for (n = 0; n < len; n++) {
		if (!isdigit(stringdata[n])
				&& stringdata[n] != 'A'
				&& stringdata[n] != 'B'
				&& stringdata[n] != 'C'
				&& stringdata[n] != 'D'
				&& stringdata[n] != 'E'
				&& stringdata[n] != 'F'
				&& stringdata[n] != 'a'
				&& stringdata[n] != 'b'
				&& stringdata[n] != 'c'
				&& stringdata[n] != 'd'
				&& stringdata[n] != 'e'
				&& stringdata[n] != 'f')
			return 0;
	}
	return 1;
}

/** Casts the contents of a ValueObject structure to boolean type in an
  * implicit context.  Casting is not done directly to the ValueObject structure
  * pointed to by \a node, instead, it is performed on a copy of that structure,
  * which is what is returned.
  *
  * \pre \a node was created by either createNilValueObject(void), createBooleanValueObject(int),
  *      createIntegerValueObject(int), createFloatValueObject(float), createStringValueObject(char *),
  *      or copied with copyValueObject(ValueObject *).
  * \pre \a scope was created by createScopeObject(ScopeObject *).
  *
  * \return A pointer to a ValueObject structure with a copy of the contents of
  *         \a node, cast to boolean type.
  *
  * \retval NULL An error occurred while casting.
  *
  * \see castIntegerImplicit(ValueObject *, ScopeObject *)
  * \see castFloatImplicit(ValueObject *, ScopeObject *)
  * \see castStringImplicit(ValueObject *, ScopeObject *) */
ValueObject *castBooleanImplicit(ValueObject *node,  /**< [in] The ValueObject structure to cast. */
                                 ScopeObject *scope) /**< [in] The ScopeObject structure to use for variable interpolation. */
{
	if (!node) return NULL;
	return castBooleanExplicit(node, scope);
}

/** Casts the contents of a ValueObject structure to integer type in an
  * implicit context.  Casting is not done directly to the ValueObject structure
  * pointed to by \a node, instead, it is performed on a copy of that structure,
  * which is what is returned.
  *
  * \pre \a node was created by either createNilValueObject(void), createBooleanValueObject(int),
  *      createIntegerValueObject(int), createFloatValueObject(float), createStringValueObject(char *),
  *      or copied with copyValueObject(ValueObject *).
  * \pre \a scope was created by createScopeObject(ScopeObject *).
  *
  * \return A pointer to a ValueObject structure with a copy of the contents of
  *         \a node, cast to integer type.
  *
  * \retval NULL An error occurred while casting.
  *
  * \see castBooleanImplicit(ValueObject *, ScopeObject *)
  * \see castFloatImplicit(ValueObject *, ScopeObject *)
  * \see castStringImplicit(ValueObject *, ScopeObject *) */
ValueObject *castIntegerImplicit(ValueObject *node,  /**< [in] The ValueObject structure to cast. */
                                 ScopeObject *scope) /**< [in] The ScopeObject structure to use for variable interpolation. */
{
	if (!node) return NULL;
	if (node->type == VT_NIL) {
		fprintf(stderr, "Cannot implicitly cast nil\n");
		return NULL;
	}
	else return castIntegerExplicit(node, scope);
}

/** Casts the contents of a ValueObject structure to floating point decimal
  * type in an implicit context.  Casting is not done directly to the
  * ValueObject structure pointed to by \a node, instead, it is performed on a
  * copy of that structure, which is what is returned.
  *
  * \pre \a node was created by either createNilValueObject(void), createBooleanValueObject(int),
  *      createIntegerValueObject(int), createFloatValueObject(float), createStringValueObject(char *),
  *      or copied with copyValueObject(ValueObject *).
  * \pre \a scope was created by createScopeObject(ScopeObject *).
  *
  * \return A pointer to a ValueObject structure with a copy of the contents of
  *         \a node, cast to floating point decimal type.
  *
  * \retval NULL An error occurred while casting.
  *
  * \see castBooleanImplicit(ValueObject *, ScopeObject *)
  * \see castIntegerImplicit(ValueObject *, ScopeObject *)
  * \see castStringImplicit(ValueObject *, ScopeObject *) */
ValueObject *castFloatImplicit(ValueObject *node,  /**< [in] The ValueObject structure to cast. */
                               ScopeObject *scope) /**< [in] The ScopeObject structure to use for variable interpolation. */
{
	if (!node) return NULL;
	if (node->type == VT_NIL) {
		fprintf(stderr, "Cannot implicitly cast nil\n");
		return NULL;
	}
	else return castFloatExplicit(node, scope);
}

/** Casts the contents of a ValueObject structure to string type in an implicit
  * context.  Casting is not done directly to the ValueObject structure pointed
  * to by \a node, instead, it is performed on a copy of that structure, which
  * is what is returned.
  *
  * \note \a scope is used to resolve variable interpolation within the string
  *       before casting it.  Therefore, a simple way to interpolate the
  *       variables within a string is to call this function with it.
  *
  * \pre \a node was created by either createNilValueObject(void), createBooleanValueObject(int),
  *      createIntegerValueObject(int), createFloatValueObject(float), createStringValueObject(char *),
  *      or copied with copyValueObject(ValueObject *).
  * \pre \a scope was created by createScopeObject(ScopeObject *).
  *
  * \return A pointer to a ValueObject structure with a copy of the contents of
  *         \a node, cast to string type.
  *
  * \retval NULL An error occurred while casting.
  *
  * \see castBooleanImplicit(ValueObject *, ScopeObject *)
  * \see castIntegerImplicit(ValueObject *, ScopeObject *)
  * \see castFloatImplicit(ValueObject *, ScopeObject *) */
ValueObject *castStringImplicit(ValueObject *node,  /**< [in] The ValueObject structure to cast. */
                                ScopeObject *scope) /**< [in] The ScopeObject structure to use for variable interpolation. */
{
	if (!node) return NULL;
	if (node->type == VT_NIL) {
		fprintf(stderr, "Cannot implicitly cast nil\n");
		return NULL;
	}
	else return castStringExplicit(node, scope);
}

/** Casts the contents of a ValueObject structure to boolean type in an
  * explicit context.  Casting is not done directly to the ValueObject structure
  * pointed to by \a node, instead, it is performed on a copy of that structure,
  * which is what is returned.
  *
  * \pre \a node was created by either createNilValueObject(void), createBooleanValueObject(int),
  *      createIntegerValueObject(int), createFloatValueObject(float), createStringValueObject(char *),
  *      or copied with copyValueObject(ValueObject *).
  * \pre \a scope was created by createScopeObject(ScopeObject *).
  *
  * \return A pointer to a ValueObject structure with a copy of the contents of
  *         \a node, cast to boolean type.
  *
  * \retval NULL An error occurred while casting.
  *
  * \see castIntegerExplicit(ValueObject *, ScopeObject *)
  * \see castFloatExplicit(ValueObject *, ScopeObject *)
  * \see castStringExplicit(ValueObject *, ScopeObject *) */
ValueObject *castBooleanExplicit(ValueObject *node,  /**< [in] The ValueObject structure to cast. */
                                 ScopeObject *scope) /**< [in] The ScopeObject structure to use for variable interpolation. */
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
			return createBooleanValueObject(getFloat(node) != 0.0);
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
	}
	fprintf(stderr, "Unknown value type encountered during boolean cast\n");
	return NULL;
}

/** Casts the contents of a ValueObject structure to integer type in an
  * explicit context.  Casting is not done directly to the ValueObject structure
  * pointed to by \a node, instead, it is performed on a copy of that structure,
  * which is what is returned.
  *
  * \pre \a node was created by either createNilValueObject(void), createBooleanValueObject(int),
  *      createIntegerValueObject(int), createFloatValueObject(float), createStringValueObject(char *),
  *      or copied with copyValueObject(ValueObject *).
  * \pre \a scope was created by createScopeObject(ScopeObject *).
  *
  * \return A pointer to a ValueObject structure with a copy of the contents of
  *         \a node, cast to integer type.
  *
  * \retval NULL An error occurred while casting.
  *
  * \see castBooleanExplicit(ValueObject *, ScopeObject *)
  * \see castFloatExplicit(ValueObject *, ScopeObject *)
  * \see castStringExplicit(ValueObject *, ScopeObject *) */
ValueObject *castIntegerExplicit(ValueObject *node,  /**< [in] The ValueObject structure to cast. */
                                 ScopeObject *scope) /**< [in] The ScopeObject structure to use for variable interpolation. */
{
	if (!node) return NULL;
	switch (node->type) {
		case VT_NIL:
			return createIntegerValueObject(0);
		case VT_BOOLEAN:
		case VT_INTEGER:
			return createIntegerValueObject(getInteger(node));
		case VT_FLOAT:
			return createIntegerValueObject(getFloat(node));
		case VT_STRING:
			if (strstr(getString(node), ":{")) {
				/* Perform interpolation */
				ValueObject *ret = NULL;
				ValueObject *interp = castStringExplicit(node, scope);
				int value;
				if (!interp) return NULL;
				if (!isNumString(getString(interp))) {
					fprintf(stderr, "Unable to cast value\n");
					deleteValueObject(interp);
					return NULL;
				}
				sscanf(getString(interp), "%i", &value);
				ret = createIntegerValueObject(value);
				deleteValueObject(interp);
				return ret;
			}
			else {
				int value;
				if (!isNumString(getString(node))) {
					fprintf(stderr, "Unable to cast value\n");
					return NULL;
				}
				sscanf(getString(node), "%i", &value);
				return createIntegerValueObject(value);
			}
	}
	fprintf(stderr, "Unknown value type encountered during integer cast\n");
	return NULL;
}

/** Casts the contents of a ValueObject structure to floating point decimal
  * type in an explicit context.  Casting is not done directly to the
  * ValueObject structure pointed to by \a node, instead, it is performed on a
  * copy of that structure, which is what is returned.
  *
  * \pre \a node was created by either createNilValueObject(void), createBooleanValueObject(int),
  *      createIntegerValueObject(int), createFloatValueObject(float), createStringValueObject(char *),
  *      or copied with copyValueObject(ValueObject *).
  * \pre \a scope was created by createScopeObject(ScopeObject *).
  *
  * \return A pointer to a ValueObject structure with a copy of the contents of
  *         \a node, cast to floating point decimal type.
  *
  * \retval NULL An error occurred while casting.
  *
  * \see castBooleanExplicit(ValueObject *, ScopeObject *)
  * \see castIntegerExplicit(ValueObject *, ScopeObject *)
  * \see castStringExplicit(ValueObject *, ScopeObject *) */
ValueObject *castFloatExplicit(ValueObject *node,  /**< [in] The ValueObject structure to cast. */
                               ScopeObject *scope) /**< [in] The ScopeObject structure to use for variable interpolation. */
{
	if (!node) return NULL;
	switch (node->type) {
		case VT_NIL:
			return createFloatValueObject(0.0);
		case VT_BOOLEAN:
		case VT_INTEGER:
			return createFloatValueObject(getInteger(node));
		case VT_FLOAT:
			return createFloatValueObject(getFloat(node));
		case VT_STRING:
			if (strstr(getString(node), ":{")) {
				/* Perform interpolation */
				ValueObject *ret = NULL;
				ValueObject *interp = castStringExplicit(node, scope);
				float value;
				if (!interp) return NULL;
				if (!isNumString(getString(interp))) {
					fprintf(stderr, "Unable to cast value\n");
					deleteValueObject(interp);
					return NULL;
				}
				sscanf(getString(interp), "%f", &value);
				ret = createFloatValueObject(value);
				deleteValueObject(interp);
				return ret;
			}
			else {
				float value;
				if (!isNumString(getString(node))) {
					fprintf(stderr, "Unable to cast value\n");
					return NULL;
				}
				sscanf(getString(node), "%f", &value);
				return createFloatValueObject(value);
			}
	}
	fprintf(stderr, "Unknown value type encountered during floating point decimal cast\n");
	return NULL;
}

/** Casts the contents of a ValueObject structure to string type in an explicit
  * context.  Casting is not done to directly the ValueObject structure pointed
  * to by \a node, instead, it is performed on a copy of that structure, which
  * is what is returned.
  *
  * \note \a scope is used to resolve variable interpolation within the string
  *       before casting it.  Therefore, a simple way to interpolate the
  *       variables within a string is to call this function with it.
  *
  * \pre \a node was created by either createNilValueObject(void), createBooleanValueObject(int),
  *      createIntegerValueObject(int), createFloatValueObject(float), createStringValueObject(char *),
  *      or copied with copyValueObject(ValueObject *).
  * \pre \a scope was created by createScopeObject(ScopeObject *).
  *
  * \return A pointer to a ValueObject structure with a copy of the contents of
  *         \a node, cast to string type.
  *
  * \retval NULL An error occurred while casting.
  *
  * \see castBooleanExplicit(ValueObject *, ScopeObject *)
  * \see castIntegerExplicit(ValueObject *, ScopeObject *)
  * \see castFloatExplicit(ValueObject *, ScopeObject *) */
ValueObject *castStringExplicit(ValueObject *node,  /**< [in] The ValueObject structure to cast. */
                                ScopeObject *scope) /**< [in] The ScopeObject structure to use for variable interpolation. */
{
	if (!node) return NULL;
	switch (node->type) {
		case VT_NIL: {
			char *str = createString("");
			if (!str) return NULL;
			return createStringValueObject(str);
		}
		case VT_BOOLEAN: {
			/** \note The spec does not define how TROOFs may be
			  *       cast to YARNs. */
			fprintf(stderr, "Cannot cast TROOF to YARN\n");
			return NULL;
		}
		case VT_INTEGER: {
			char *data = NULL;
			/* One character per integer bit plus one more for the
			 * null character */
			size_t size = sizeof(int) * 8 + 1;
			data = malloc(sizeof(char) * size);
			if (!data) return NULL;
			sprintf(data, "%i", getInteger(node));
			return createStringValueObject(data);
		}
		case VT_FLOAT: {
			char *data = NULL;
			unsigned int precision = 2;
			/* One character per float bit plus one more for the
			 * null character */
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
			unsigned int a, b, size;
			/* Perform interpolation */
			size = strlen(getString(node)) + 1;
			temp = malloc(sizeof(char) * size);
			for (a = 0, b = 0; str[b] != '\0'; ) {
				if (!strncmp(str + b, ":)", 2)) {
					temp[a] = '\n';
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
					unsigned short len = end - start;
					char *image = malloc(sizeof(char) * (len + 1));
					long codepoint;
					char out[3];
					unsigned short num;
					void *mem = NULL;
					strncpy(image, start, len);
					image[len] = '\0';
					if (!isHexString(image)) {
						fprintf(stderr, "Please supply a valid hexadecimal number\n");
						free(temp);
						free(image);
						return NULL;
					}
					codepoint = strtol(image, NULL, 16);
					free(image);
					num = convertCodePointToUTF8(codepoint, out);
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
					unsigned short len = end - start;
					char *image = malloc(sizeof(char) * (len + 1));
					long codepoint;
					char out[3];
					unsigned short num;
					void *mem = NULL;
					strncpy(image, start, len);
					strncpy(image, start, len);
					image[len] = '\0';
					codepoint = convertNormativeNameToCodePoint(image);
					free(image);
					if (codepoint < 0) {
						free(temp);
						return NULL;
					}
					num = convertCodePointToUTF8(codepoint, out);
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
					unsigned short len = end - start;
					char *image = malloc(sizeof(char) * (len + 1));
					void *mem = NULL;
					strncpy(image, start, len);
					image[len] = '\0';
					if (!strcmp(image, "IT"))
						/* Lookup implicit variable */
						val = scope->impvar;
					else {
						/* Create a new IdentifierNode
						 * structure and lookup its
						 * value */
						target = createIdentifierNode(image, NULL, 0);
						if (!target) {
							free(temp);
							return NULL;
						}
						val = getScopeValue(scope, target);
						if (!val) {
							fprintf(stderr, "Variable does not exist at: %s\n", image);
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
	}
	fprintf(stderr, "Unknown value type encountered during string cast\n");
	return NULL;
}

/** Interprets an implicit variable expression.
  *
  * \pre \a node was created by createExprNode(ExprType type, void *expr)
  *      where \a type is ET_IMPVAR and \a expr is NULL.
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \note \a node is not used by this function but is still included in its
  *       prototype to allow this function to be stored in a jump table for fast
  *       execution.
  *
  * \return A pointer to a ValueObject structure containing the value of the
  *         current scope's implicit variable.
  *
  * \see interpretCastExprNode(ExprNode *, ScopeObject *)
  * \see interpretFuncCallExprNode(ExprNode *, ScopeObject *)
  * \see interpretIdentifierExprNode(ExprNode *, ScopeObject *)
  * \see interpretConstantExprNode(ExprNode *, ScopeObject *) */
ValueObject *interpretImpVarExprNode(ExprNode *node,     /**< [in] A pointer to an ExprNode structure with type set to ET_IMPVAR. */
                                     ScopeObject *scope) /**< Not used (see note). */
{
	node = NULL;
	return scope->impvar;
}

/** Interprets a cast expression.
  *
  * \pre \a node was created by createExprNode(ExprType type, void *expr)
  *      where \a type is ET_CAST and \a expr is a CastExprNode structure
  *      created by createCastExprNode(ExprNode *, TypeNode *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return A pointer to a ValueObject structure containing the result of the
  *         cast.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretImpVarExprNode(ExprNode *, ScopeObject *)
  * \see interpretFuncCallExprNode(ExprNode *, ScopeObject *)
  * \see interpretIdentifierExprNode(ExprNode *, ScopeObject *)
  * \see interpretConstantExprNode(ExprNode *, ScopeObject *) */
ValueObject *interpretCastExprNode(ExprNode *node,     /**< [in] A pointer to an ExprNode structure containing the CastExprNode structure to interpret. */
                                   ScopeObject *scope) /**< [in,out] A pointer to a ScopeObject structure to evaluate \a node under. */
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
			fprintf(stderr, "Unknown cast type\n");
			deleteValueObject(val);
			return NULL;
	}
}

/** Interprets a function call expression.
  *
  * \pre \a node was created by createExprNode(ExprType type, void *expr)
  *      where \a type is ET_FUNCCALL and \a expr is a FunctionCallExprNode
  *      structure created by createFuncCallExprNode(FuncDefStmtNode *, ExprNodeList *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return A pointer to a ValueObject structure containing the return value of
  *         function.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretImpVarExprNode(ExprNode *, ScopeObject *)
  * \see interpretCastExprNode(ExprNode *, ScopeObject *)
  * \see interpretIdentifierExprNode(ExprNode *, ScopeObject *)
  * \see interpretConstantExprNode(ExprNode *, ScopeObject *) */
ValueObject *interpretFuncCallExprNode(ExprNode *node,     /**< [in] A pointer to an ExprNode structure containing the FuncCallExprNode structure to interpret. */
                                       ScopeObject *scope) /**< [in,out] A pointer to a ScopeObject structure to evaluate \a node under. */
{
	FuncCallExprNode *expr = (FuncCallExprNode *)node->expr;
	unsigned int n;
	ScopeObject *outer = createScopeObject(scope);
	ReturnObject *retval = NULL;
	ValueObject *ret = NULL;
	if (!outer) return NULL;
	for (n = 0; n < expr->def->args->num; n++) {
		ValueObject *val = NULL;
		if (!createScopeValue(outer, expr->def->args->ids[n])) {
			deleteScopeObject(outer);
			return NULL;
		}
		if (!(val = interpretExprNode(expr->args->exprs[n], scope))) {
			deleteScopeObject(outer);
			return NULL;
		}
		if (!updateScopeValue(outer, expr->def->args->ids[n], val)) {
			deleteScopeObject(outer);
			deleteValueObject(val);
			return NULL;
		}
	}
	/* \note We use interpretStmtNodeList(StmtNodeList *, ScopeObject *)
	 *       here because we want to have access to the function's ScopeObject
	 *       as we may need to retrieve the implicit variable in the case of
	 *       a default return. */
	if (!(retval = interpretStmtNodeList(expr->def->body->stmts, outer))) {
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
			fprintf(stderr, "Invalid return type\n");
			break;
	}
	deleteReturnObject(retval);
	deleteScopeObject(outer);
	return ret;
}

/** Interprets an identifier expression.
  *
  * \pre \a node was created by createExprNode(ExprType type, void *expr)
  *      where \a type is ET_IDENTIFIER and \a expr is an IdentifierNode
  *      structure created by createIdentifierNode(char *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \note \a scope is not used by this function but is still included in its
  *       prototype to allow this function to be stored in a jump table for fast
  *       execution.
  *
  * \return A pointer to a ValueObject structure containing the return value of
  *         function.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretImpVarExprNode(ExprNode *, ScopeObject *)
  * \see interpretCastExprNode(ExprNode *, ScopeObject *)
  * \see interpretFuncCallExprNode(ExprNode *, ScopeObject *)
  * \see interpretConstantExprNode(ExprNode *, ScopeObject *) */
ValueObject *interpretIdentifierExprNode(ExprNode *node,     /**< [in] A pointer to an ExprNode structure containing the IdentifierNode structure to interpret. */
                                         ScopeObject *scope) /**< Not used (see note). */
{
	ValueObject *val = getScopeValue(scope, node->expr);
	if (!val) {
		IdentifierNode *id = (IdentifierNode *)(node->expr);
		fprintf(stderr, "%s:%d: variable does not exist at: %s\n", id->fname, id->line, id->image);
		return NULL;
	}
	return copyValueObject(val);
}

/** Interprets a constant expression.
  *
  * \pre \a node was created by createExprNode(ExprType type, void *expr)
  *      where \a type is ET_CONSTANT and \a expr is a ConstantNode structure
  *      created by either createBooleanConstantNode(int), createIntegerConstantNode(int),
  *      createFloatConstantNode(float), or createStringConstantNode(char *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \note \a scope is not used by this function but is still included in its
  *       prototype to allow this function to be stored in a jump table for fast
  *       execution.
  *
  * \return A pointer to a ValueObject structure containing the return value of
  *         function.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretImpVarExprNode(ExprNode *, ScopeObject *)
  * \see interpretCastExprNode(ExprNode *, ScopeObject *)
  * \see interpretFuncCallExprNode(ExprNode *, ScopeObject *)
  * \see interpretIdentifierExprNode(ExprNode *, ScopeObject *) */
ValueObject *interpretConstantExprNode(ExprNode *node,     /**< [in] A pointer to an ExprNode structure containing the ConstantNode structure to interpret. */
                                       ScopeObject *scope) /**< Not used (see note). */
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
			/** \note For efficiency, string interpolation should be
			  *       performed by caller because it only needs to
			  *       be performed when necessary. */
			char *str = createString(expr->data.s);
			if (!str) return NULL;
			return createStringValueObject(str);
		}
		default:
			fprintf(stderr, "Unknown constant type\n");
			return NULL;
	}
}

/** Interprets a logical NOT operation expression.
  *
  * \pre \a expr was created by createOpExprNode(OpType type, ExprNodeList *args)
  *      where \a type is OP_NOT and \a args was created by createExprNodeList(void)
  *      and populated with ExprNode structures using addExprNode(ExprNodeList *, ExprNode *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \note Only the first element in \a args is considered.
  *
  * \return A pointer to a ValueObject structure containing the interpreted
  *         operation expression value.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretArithOpExprNode(OpExprNode *, ScopeObject *)
  * \see interpretBoolOpExprNode(OpExprNode *, ScopeObject *)
  * \see interpretEqualityOpExprNode(OpExprNode *, ScopeObject *)
  * \see interpretConcatOpExprNode(OpExprNode *, ScopeObject *) */
ValueObject *interpretNotOpExprNode(OpExprNode *expr,   /**< [in] A pointer to the OpExprNode structure to interpret. */
                                    ScopeObject *scope) /**< [in] A pointer to the ScopeObject structure to evaluate \a expr under. */
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

/** Adds two integers.
  *
  * \pre \a a and \a b were created by createIntegerValueObject(int).
  *
  * \return A pointer to a ValueObject structure containing the sum of the
  *         values \a a and \a b.
  *
  * \see opSubIntegerInteger(ValueObject *, ValueObject *)
  * \see opMultIntegerInteger(ValueObject *, ValueObject *)
  * \see opDivIntegerInteger(ValueObject *, ValueObject *)
  * \see opMaxIntegerInteger(ValueObject *, ValueObject *)
  * \see opMinIntegerInteger(ValueObject *, ValueObject *)
  * \see opModIntegerInteger(ValueObject *, ValueObject *) */
ValueObject *opAddIntegerInteger(ValueObject *a, /**< [in] The first term to add. */
                                 ValueObject *b) /**< [in] The second term to add. */
{
	return createIntegerValueObject(getInteger(a) + getInteger(b));
}

/** Subtracts two integers.
  *
  * \pre \a a and \a b were created by createIntegerValueObject(int).
  *
  * \return A pointer to a ValueObject structure containing the difference of
  *         the values \a a and \a b.
  *
  * \see opAddIntegerInteger(ValueObject *, ValueObject *)
  * \see opMultIntegerInteger(ValueObject *, ValueObject *)
  * \see opDivIntegerInteger(ValueObject *, ValueObject *)
  * \see opMaxIntegerInteger(ValueObject *, ValueObject *)
  * \see opMinIntegerInteger(ValueObject *, ValueObject *)
  * \see opModIntegerInteger(ValueObject *, ValueObject *) */
ValueObject *opSubIntegerInteger(ValueObject *a, /**< [in] The minuend. */
                                 ValueObject *b) /**< [in] The subtrahend. */
{
	return createIntegerValueObject(getInteger(a) - getInteger(b));
}

/** Multiplies two integers.
  *
  * \pre \a a and \a b were created by createIntegerValueObject(int).
  *
  * \return A pointer to a ValueObject structure containing the product of the
  *         values \a a and \a b.
  *
  * \see opAddIntegerInteger(ValueObject *, ValueObject *)
  * \see opSubIntegerInteger(ValueObject *, ValueObject *)
  * \see opDivIntegerInteger(ValueObject *, ValueObject *)
  * \see opMaxIntegerInteger(ValueObject *, ValueObject *)
  * \see opMinIntegerInteger(ValueObject *, ValueObject *)
  * \see opModIntegerInteger(ValueObject *, ValueObject *) */
ValueObject *opMultIntegerInteger(ValueObject *a, /**< [in] The first factor to multiply. */
                                  ValueObject *b) /**< [in] The second factor to multiply. */
{
	return createIntegerValueObject(getInteger(a) * getInteger(b));
}

/** Divides two integers.
  *
  * \pre \a a and \a b were created by createIntegerValueObject(int).
  *
  * \return A pointer to a ValueObject structure containing the quotient of the
  *         values \a a and \a b.
  *
  * \retval NULL Division by zero was attempted.
  *
  * \see opAddIntegerInteger(ValueObject *, ValueObject *)
  * \see opSubIntegerInteger(ValueObject *, ValueObject *)
  * \see opMultIntegerInteger(ValueObject *, ValueObject *)
  * \see opMaxIntegerInteger(ValueObject *, ValueObject *)
  * \see opMinIntegerInteger(ValueObject *, ValueObject *)
  * \see opModIntegerInteger(ValueObject *, ValueObject *) */
ValueObject *opDivIntegerInteger(ValueObject *a, /**< [in] The dividend. */
                                 ValueObject *b) /**< [in] The divisor. */
{
	if (getInteger(b) == 0) {
		fprintf(stderr, "Division by zero undefined\n");
		return NULL;
	}
	return createIntegerValueObject(getInteger(a) / getInteger(b));
}

/** Finds the maximum of two integers.
  *
  * \pre \a a and \a b were created by createIntegerValueObject(int).
  *
  * \return A pointer to a ValueObject structure containing the maximum of the
  *         values \a a and \a b.
  *
  * \see opAddIntegerInteger(ValueObject *, ValueObject *)
  * \see opSubIntegerInteger(ValueObject *, ValueObject *)
  * \see opMultIntegerInteger(ValueObject *, ValueObject *)
  * \see opDivIntegerInteger(ValueObject *, ValueObject *)
  * \see opMinIntegerInteger(ValueObject *, ValueObject *)
  * \see opModIntegerInteger(ValueObject *, ValueObject *) */
ValueObject *opMaxIntegerInteger(ValueObject *a, /**< [in] The first number to compare. */
                                 ValueObject *b) /**< [in] The second number to compare. */
{
	return createIntegerValueObject(getInteger(a) > getInteger(b) ? getInteger(a) : getInteger(b));
}

/** Finds the minimum of two integers.
  *
  * \pre \a a and \a b were created by createIntegerValueObject(int).
  *
  * \return A pointer to a ValueObject structure containing the minimum of the
  *         values \a a and \a b.
  *
  * \see opAddIntegerInteger(ValueObject *, ValueObject *)
  * \see opSubIntegerInteger(ValueObject *, ValueObject *)
  * \see opMultIntegerInteger(ValueObject *, ValueObject *)
  * \see opDivIntegerInteger(ValueObject *, ValueObject *)
  * \see opMaxIntegerInteger(ValueObject *, ValueObject *)
  * \see opModIntegerInteger(ValueObject *, ValueObject *) */
ValueObject *opMinIntegerInteger(ValueObject *a, /**< [in] The first number to compare. */
                                 ValueObject *b) /**< [in] The second number to compare. */
{
	return createIntegerValueObject(getInteger(a) < getInteger(b) ? getInteger(a) : getInteger(b));
}

/** Calculates the modulus of two integers.
  *
  * \pre \a a and \a b were created by createIntegerValueObject(int).
  *
  * \return A pointer to a ValueObject structure containing the modulus of the
  *         values \a a and \a b.
  *
  * \see opAddIntegerInteger(ValueObject *, ValueObject *)
  * \see opSubIntegerInteger(ValueObject *, ValueObject *)
  * \see opMultIntegerInteger(ValueObject *, ValueObject *)
  * \see opDivIntegerInteger(ValueObject *, ValueObject *)
  * \see opMaxIntegerInteger(ValueObject *, ValueObject *)
  * \see opMinIntegerInteger(ValueObject *, ValueObject *) */
ValueObject *opModIntegerInteger(ValueObject *a, /**< [in] The dividend. */
                                 ValueObject *b) /**< [in] The divisor. */
{
	if (getInteger(b) == 0) {
		fprintf(stderr, "Division by zero undefined\n");
		return NULL;
	}
	return createIntegerValueObject(getInteger(a) % getInteger(b));
}

/** Adds an integer and a float.
  *
  * \pre \a a was created by createIntegerValueObject(int) and \a b was created
  *      by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing the sum of the
  *         values \a a and \a b.
  *
  * \see opSubIntegerFloat(ValueObject *, ValueObject *)
  * \see opMultIntegerFloat(ValueObject *, ValueObject *)
  * \see opDivIntegerFloat(ValueObject *, ValueObject *)
  * \see opMaxIntegerFloat(ValueObject *, ValueObject *)
  * \see opMinIntegerFloat(ValueObject *, ValueObject *)
  * \see opModIntegerFloat(ValueObject *, ValueObject *) */
ValueObject *opAddIntegerFloat(ValueObject *a, /**< [in] The first term to add. */
                               ValueObject *b) /**< [in] The second term to add. */
{
	return createFloatValueObject(getInteger(a) + getFloat(b));
}

/** Subtracts an integer and a float.
  *
  * \pre \a a was created by createIntegerValueObject(int) and \a b was created
  *      by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing the difference of
  *         the values \a a and \a b.
  *
  * \see opAddIntegerFloat(ValueObject *, ValueObject *)
  * \see opMultIntegerFloat(ValueObject *, ValueObject *)
  * \see opDivIntegerFloat(ValueObject *, ValueObject *)
  * \see opMaxIntegerFloat(ValueObject *, ValueObject *)
  * \see opMinIntegerFloat(ValueObject *, ValueObject *)
  * \see opModIntegerFloat(ValueObject *, ValueObject *) */
ValueObject *opSubIntegerFloat(ValueObject *a, /**< [in] The minuend. */
                               ValueObject *b) /**< [in] The subtrahend. */
{
	return createFloatValueObject(getInteger(a) - getFloat(b));
}

/** Multiplies an integer and a float.
  *
  * \pre \a a was created by createIntegerValueObject(int) and \a b was created
  *      by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing the product of the
  *         values \a a and \a b.
  *
  * \see opAddIntegerFloat(ValueObject *, ValueObject *)
  * \see opSubIntegerFloat(ValueObject *, ValueObject *)
  * \see opDivIntegerFloat(ValueObject *, ValueObject *)
  * \see opMaxIntegerFloat(ValueObject *, ValueObject *)
  * \see opMinIntegerFloat(ValueObject *, ValueObject *)
  * \see opModIntegerFloat(ValueObject *, ValueObject *) */
ValueObject *opMultIntegerFloat(ValueObject *a, /**< [in] The first factor to multiply. */
                                ValueObject *b) /**< [in] The second factor to multiply. */
{
	return createFloatValueObject(getInteger(a) * getFloat(b));
}

/** Divides an integer and a float.
  *
  * \pre \a a was created by createIntegerValueObject(int) and \a b was created
  *      by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing the quotient of the
  *         values \a a and \a b.
  *
  * \retval NULL Division by zero was attempted.
  *
  * \see opAddIntegerFloat(ValueObject *, ValueObject *)
  * \see opSubIntegerFloat(ValueObject *, ValueObject *)
  * \see opMultIntegerFloat(ValueObject *, ValueObject *)
  * \see opMaxIntegerFloat(ValueObject *, ValueObject *)
  * \see opMinIntegerFloat(ValueObject *, ValueObject *)
  * \see opModIntegerFloat(ValueObject *, ValueObject *) */
ValueObject *opDivIntegerFloat(ValueObject *a, /**< [in] The dividend. */
                               ValueObject *b) /**< [in] The divisor. */
{
	if (getFloat(b) == 0.0) {
		fprintf(stderr, "Division by zero undefined\n");
		return NULL;
	}
	return createFloatValueObject(getInteger(a) / getFloat(b));
}

/** Finds the maximum of an integer and a float.
  *
  * \pre \a a was created by createIntegerValueObject(int) and \a b was created
  *      by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing the maximum of the
  *         values \a a and \a b.
  *
  * \see opAddIntegerFloat(ValueObject *, ValueObject *)
  * \see opSubIntegerFloat(ValueObject *, ValueObject *)
  * \see opMultIntegerFloat(ValueObject *, ValueObject *)
  * \see opDivIntegerFloat(ValueObject *, ValueObject *)
  * \see opMinIntegerFloat(ValueObject *, ValueObject *)
  * \see opModIntegerFloat(ValueObject *, ValueObject *) */
ValueObject *opMaxIntegerFloat(ValueObject *a, /**< [in] The first number to compare. */
                               ValueObject *b) /**< [in] The second number to compare. */
{
	return createFloatValueObject(getInteger(a) > getFloat(b) ? getInteger(a) : getFloat(b));
}

/** Finds the minimum of an integer and a float.
  *
  * \pre \a a was created by createIntegerValueObject(int) and \a b was created
  *      by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing the minimum of the
  *         values \a a and \a b.
  *
  * \see opAddIntegerFloat(ValueObject *, ValueObject *)
  * \see opSubIntegerFloat(ValueObject *, ValueObject *)
  * \see opMultIntegerFloat(ValueObject *, ValueObject *)
  * \see opDivIntegerFloat(ValueObject *, ValueObject *)
  * \see opMaxIntegerFloat(ValueObject *, ValueObject *)
  * \see opModIntegerFloat(ValueObject *, ValueObject *) */
ValueObject *opMinIntegerFloat(ValueObject *a, /**< [in] The first number to compare. */
                               ValueObject *b) /**< [in] The second number to compare. */
{
	return createFloatValueObject(getInteger(a) < getFloat(b) ? getInteger(a) : getFloat(b));
}

/** Calculates the modulus of an integer and a float.
  *
  * \pre \a a was created by createIntegerValueObject(int) and \a b was created
  *      by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing the modulus of the
  *         values \a a and \a b.
  *
  * \see opAddIntegerFloat(ValueObject *, ValueObject *)
  * \see opSubIntegerFloat(ValueObject *, ValueObject *)
  * \see opMultIntegerFloat(ValueObject *, ValueObject *)
  * \see opDivIntegerFloat(ValueObject *, ValueObject *)
  * \see opMaxIntegerFloat(ValueObject *, ValueObject *)
  * \see opMinIntegerFloat(ValueObject *, ValueObject *) */
ValueObject *opModIntegerFloat(ValueObject *a, /**< [in] The dividend. */
                               ValueObject *b) /**< [in] The divisor. */
{
	if (getFloat(b) == 0.0) {
		fprintf(stderr, "Division by zero undefined\n");
		return NULL;
	}
	return createFloatValueObject(fmod(getInteger(a), getFloat(b)));
}

/** Adds a float and an integer.
  *
  * \pre \a a was created by createFloatValueObject(float) and \a b was created
  *      by createIntegerValueObject(int).
  *
  * \return A pointer to a ValueObject structure containing the sum of the
  *         values \a a and \a b.
  *
  * \see opSubFloatInteger(ValueObject *, ValueObject *)
  * \see opMultFloatInteger(ValueObject *, ValueObject *)
  * \see opDivFloatInteger(ValueObject *, ValueObject *)
  * \see opMaxFloatInteger(ValueObject *, ValueObject *)
  * \see opMinFloatInteger(ValueObject *, ValueObject *)
  * \see opModFloatInteger(ValueObject *, ValueObject *) */
ValueObject *opAddFloatInteger(ValueObject *a, /**< [in] The first term to add. */
                               ValueObject *b) /**< [in] The second term to add. */
{
	return createFloatValueObject(getFloat(a) + getInteger(b));
}

/** Subtracts a float and an integer.
  *
  * \pre \a a was created by createFloatValueObject(float) and \a b was created
  *      by createIntegerValueObject(int).
  *
  * \return A pointer to a ValueObject structure containing the difference of
  *         the values \a a and \a b.
  *
  * \see opAddFloatInteger(ValueObject *, ValueObject *)
  * \see opMultFloatInteger(ValueObject *, ValueObject *)
  * \see opDivFloatInteger(ValueObject *, ValueObject *)
  * \see opMaxFloatInteger(ValueObject *, ValueObject *)
  * \see opMinFloatInteger(ValueObject *, ValueObject *)
  * \see opModFloatInteger(ValueObject *, ValueObject *) */
ValueObject *opSubFloatInteger(ValueObject *a, /**< [in] The minuend. */
                               ValueObject *b) /**< [in] The subtrahend. */
{
	return createFloatValueObject(getFloat(a) - getInteger(b));
}

/** Multiplies a float and an integer.
  *
  * \pre \a a was created by createFloatValueObject(float) and \a b was created
  *      by createIntegerValueObject(int).
  *
  * \return A pointer to a ValueObject structure containing the product of the
  *         values \a a and \a b.
  *
  * \see opAddFloatInteger(ValueObject *, ValueObject *)
  * \see opSubFloatInteger(ValueObject *, ValueObject *)
  * \see opDivFloatInteger(ValueObject *, ValueObject *)
  * \see opMaxFloatInteger(ValueObject *, ValueObject *)
  * \see opMinFloatInteger(ValueObject *, ValueObject *)
  * \see opModFloatInteger(ValueObject *, ValueObject *) */ ValueObject *opMultFloatInteger(ValueObject *a, /**< [in] The first factor to multiply. */
                                ValueObject *b) /**< [in] The second factor to multiply. */
{
	return createFloatValueObject(getFloat(a) * getInteger(b));
}

/** Divides a float and an integer.
  *
  * \pre \a a was created by createFloatValueObject(float) and \a b was created
  *      by createIntegerValueObject(int).
  *
  * \return A pointer to a ValueObject structure containing the quotient of the
  *         values \a a and \a b.
  *
  * \retval NULL Division by zero was attempted.
  *
  * \see opAddFloatInteger(ValueObject *, ValueObject *)
  * \see opSubFloatInteger(ValueObject *, ValueObject *)
  * \see opMultFloatInteger(ValueObject *, ValueObject *)
  * \see opMaxFloatInteger(ValueObject *, ValueObject *)
  * \see opMinFloatInteger(ValueObject *, ValueObject *)
  * \see opModFloatInteger(ValueObject *, ValueObject *) */
ValueObject *opDivFloatInteger(ValueObject *a, /**< [in] The dividend. */
                               ValueObject *b) /**< [in] The divisor. */
{
	if (getInteger(b) == 0) {
		fprintf(stderr, "Division by zero undefined\n");
		return NULL;
	}
	return createFloatValueObject(getFloat(a) / getInteger(b));
}

/** Finds the maximum of a float and an integer.
  *
  * \pre \a a was created by createFloatValueObject(float) and \a b was created
  *      by createIntegerValueObject(int).
  *
  * \return A pointer to a ValueObject structure containing the maximum of the
  *         values \a a and \a b.
  *
  * \see opAddFloatInteger(ValueObject *, ValueObject *)
  * \see opSubFloatInteger(ValueObject *, ValueObject *)
  * \see opMultFloatInteger(ValueObject *, ValueObject *)
  * \see opDivFloatInteger(ValueObject *, ValueObject *)
  * \see opMinFloatInteger(ValueObject *, ValueObject *)
  * \see opModFloatInteger(ValueObject *, ValueObject *) */
ValueObject *opMaxFloatInteger(ValueObject *a, /**< [in] The first number to compare. */
                               ValueObject *b) /**< [in] The second number to compare. */
{
	return createFloatValueObject(getFloat(a) > getInteger(b) ? getFloat(a) : getInteger(b));
}

/** Finds the minimum of a float and an integer.
  *
  * \pre \a a was created by createFloatValueObject(float) and \a b was created
  *      by createIntegerValueObject(int).
  *
  * \return A pointer to a ValueObject structure containing the minimum of the
  *         values \a a and \a b.
  *
  * \see opAddFloatInteger(ValueObject *, ValueObject *)
  * \see opSubFloatInteger(ValueObject *, ValueObject *)
  * \see opMultFloatInteger(ValueObject *, ValueObject *)
  * \see opDivFloatInteger(ValueObject *, ValueObject *)
  * \see opMaxFloatInteger(ValueObject *, ValueObject *)
  * \see opModFloatInteger(ValueObject *, ValueObject *) */
ValueObject *opMinFloatInteger(ValueObject *a, /**< [in] The first number to compare. */
                               ValueObject *b) /**< [in] The second number to compare. */
{
	return createFloatValueObject(getFloat(a) < getInteger(b) ? getFloat(a) : getInteger(b));
}

/** Calculates the modulus of a float and an integer.
  *
  * \pre \a a was created by createFloatValueObject(float) and \a b was created
  *      by createIntegerValueObject(int).
  *
  * \return A pointer to a ValueObject structure containing the modulus of the
  *         values \a a and \a b.
  *
  * \see opAddFloatInteger(ValueObject *, ValueObject *)
  * \see opSubFloatInteger(ValueObject *, ValueObject *)
  * \see opMultFloatInteger(ValueObject *, ValueObject *)
  * \see opDivFloatInteger(ValueObject *, ValueObject *)
  * \see opMaxFloatInteger(ValueObject *, ValueObject *)
  * \see opMinFloatInteger(ValueObject *, ValueObject *) */
ValueObject *opModFloatInteger(ValueObject *a, /**< [in] The dividend. */
                               ValueObject *b) /**< [in] The divisor. */
{
	if (getInteger(b) == 0) {
		fprintf(stderr, "Division by zero undefined\n");
		return NULL;
	}
	return createFloatValueObject(fmod(getFloat(a), getInteger(b)));
}

/** Adds two floats.
  *
  * \pre \a a and \a b were created by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing the sum of the
  *         values \a a and \a b.
  *
  * \see opSubFloatFloat(ValueObject *, ValueObject *)
  * \see opMultFloatFloat(ValueObject *, ValueObject *)
  * \see opDivFloatFloat(ValueObject *, ValueObject *)
  * \see opMaxFloatFloat(ValueObject *, ValueObject *)
  * \see opMinFloatFloat(ValueObject *, ValueObject *)
  * \see opModFloatFloat(ValueObject *, ValueObject *) */
ValueObject *opAddFloatFloat(ValueObject *a, /**< [in] The first term to add. */
                             ValueObject *b) /**< [in] The second term to add. */
{
	return createFloatValueObject(getFloat(a) + getFloat(b));
}

/** Subtracts two floats.
  *
  * \pre \a a and \a b were created by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing the difference of
  *         the values \a a and \a b.
  *
  * \see opAddFloatFloat(ValueObject *, ValueObject *)
  * \see opMultFloatFloat(ValueObject *, ValueObject *)
  * \see opDivFloatFloat(ValueObject *, ValueObject *)
  * \see opMaxFloatFloat(ValueObject *, ValueObject *)
  * \see opMinFloatFloat(ValueObject *, ValueObject *)
  * \see opModFloatFloat(ValueObject *, ValueObject *) */
ValueObject *opSubFloatFloat(ValueObject *a, /**< [in] The minuend. */
                             ValueObject *b) /**< [in] The subtrahend. */
{
	return createFloatValueObject(getFloat(a) - getFloat(b));
}

/** Multiplies two floats.
  *
  * \pre \a a and \a b were created by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing the product of the
  *         values \a a and \a b.
  *
  * \see opAddFloatFloat(ValueObject *, ValueObject *)
  * \see opSubFloatFloat(ValueObject *, ValueObject *)
  * \see opDivFloatFloat(ValueObject *, ValueObject *)
  * \see opMaxFloatFloat(ValueObject *, ValueObject *)
  * \see opMinFloatFloat(ValueObject *, ValueObject *)
  * \see opModFloatFloat(ValueObject *, ValueObject *) */
ValueObject *opMultFloatFloat(ValueObject *a, /**< [in] The first factor to multiply. */
                              ValueObject *b) /**< [in] The second factor to multiply. */
{
	return createFloatValueObject(getFloat(a) * getFloat(b));
}

/** Divides two floats.
  *
  * \pre \a a and \a b were created by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing the quotient of the
  *         values \a a and \a b.
  *
  * \retval NULL Division by zero was attempted.
  *
  * \see opAddFloatFloat(ValueObject *, ValueObject *)
  * \see opSubFloatFloat(ValueObject *, ValueObject *)
  * \see opMultFloatFloat(ValueObject *, ValueObject *)
  * \see opMaxFloatFloat(ValueObject *, ValueObject *)
  * \see opMinFloatFloat(ValueObject *, ValueObject *)
  * \see opModFloatFloat(ValueObject *, ValueObject *) */
ValueObject *opDivFloatFloat(ValueObject *a, /**< [in] The dividend. */
                             ValueObject *b) /**< [in] The divisor. */
{
	if (getFloat(b) == 0.0) {
		fprintf(stderr, "Division by zero undefined\n");
		return NULL;
	}
	return createFloatValueObject(getFloat(a) / getFloat(b));
}

/** Finds the maximum of two floats.
  *
  * \pre \a a and \a b were created by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing the maximum of the
  *         values \a a and \a b.
  *
  * \see opAddFloatFloat(ValueObject *, ValueObject *)
  * \see opSubFloatFloat(ValueObject *, ValueObject *)
  * \see opMultFloatFloat(ValueObject *, ValueObject *)
  * \see opDivFloatFloat(ValueObject *, ValueObject *)
  * \see opMinFloatFloat(ValueObject *, ValueObject *)
  * \see opModFloatFloat(ValueObject *, ValueObject *) */
ValueObject *opMaxFloatFloat(ValueObject *a, /**< [in] The first number to compare. */
                             ValueObject *b) /**< [in] The second number to compare. */
{
	return createFloatValueObject(getFloat(a) > getFloat(b) ? getFloat(a) : getFloat(b));
}

/** Finds the minimum of two floats.
  *
  * \pre \a a and \a b were created by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing the minimum of the
  *         values \a a and \a b.
  *
  * \see opAddFloatFloat(ValueObject *, ValueObject *)
  * \see opSubFloatFloat(ValueObject *, ValueObject *)
  * \see opMultFloatFloat(ValueObject *, ValueObject *)
  * \see opDivFloatFloat(ValueObject *, ValueObject *)
  * \see opMaxFloatFloat(ValueObject *, ValueObject *)
  * \see opModFloatFloat(ValueObject *, ValueObject *) */
ValueObject *opMinFloatFloat(ValueObject *a, /**< [in] The first number to compare. */
                             ValueObject *b) /**< [in] The second number to compare. */
{
	return createFloatValueObject(getFloat(a) < getFloat(b) ? getFloat(a) : getFloat(b));
}

/** Calculates the modulus of two floats.
  *
  * \pre \a a and \a b were created by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing the modulus of the
  *         values \a a and \a b.
  *
  * \see opAddFloatFloat(ValueObject *, ValueObject *)
  * \see opSubFloatFloat(ValueObject *, ValueObject *)
  * \see opMultFloatFloat(ValueObject *, ValueObject *)
  * \see opDivFloatFloat(ValueObject *, ValueObject *)
  * \see opMaxFloatFloat(ValueObject *, ValueObject *)
  * \see opMinFloatFloat(ValueObject *, ValueObject *) */
ValueObject *opModFloatFloat(ValueObject *a, /**< [in] The dividend. */
                             ValueObject *b) /**< [in] The divisor. */
{
	if (getFloat(b) == 0.0) {
		fprintf(stderr, "Division by zero undefined\n");
		return NULL;
	}
	return createFloatValueObject(fmod(getFloat(a), getFloat(b)));
}

/* A jump table for arithmetic operations.  The first index determines the
 * particular arithmetic operation to parform, the second index determines the
 * type of the first argument, and the third index determines the type of the
 * second object. */
static ValueObject *(*ArithOpJumpTable[7][2][2])(ValueObject *, ValueObject *) = {
	{ { opAddIntegerInteger, opAddIntegerFloat }, { opAddFloatInteger, opAddFloatFloat } },
	{ { opSubIntegerInteger, opSubIntegerFloat }, { opSubFloatInteger, opSubFloatFloat } },
	{ { opMultIntegerInteger, opMultIntegerFloat }, { opMultFloatInteger, opMultFloatFloat } },
	{ { opDivIntegerInteger, opDivIntegerFloat }, { opDivFloatInteger, opDivFloatFloat } },
	{ { opModIntegerInteger, opModIntegerFloat }, { opModFloatInteger, opModFloatFloat } },
	{ { opMaxIntegerInteger, opMaxIntegerFloat }, { opMaxFloatInteger, opMaxFloatFloat } },
	{ { opMinIntegerInteger, opMinIntegerFloat }, { opMinFloatInteger, opMinFloatFloat } }
};

/** Interprets an arithmetic operation expression.
  *
  * \pre \a expr was created by createOpExprNode(OpType type, ExprNodeList *args)
  *      where \a type is either OP_ADD, OP_SUB, OP_MULT, OP_DIV, or OP_MOD and
  *      \a args was created by createExprNodeList(void) and populated with
  *      ExprNode structures using addExprNode(ExprNodeList *, ExprNode *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \note Only the first two elements in \a args are considered.
  *
  * \return A pointer to a ValueObject structure containing the interpreted
  *         operation expression value.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretNotOpExprNode(OpExprNode *, ScopeObject *)
  * \see interpretBoolOpExprNode(OpExprNode *, ScopeObject *)
  * \see interpretEqualityOpExprNode(OpExprNode *, ScopeObject *)
  * \see interpretConcatOpExprNode(OpExprNode *, ScopeObject *) */
ValueObject *interpretArithOpExprNode(OpExprNode *expr,   /**< [in] A pointer to the OpExprNode structure to interpret. */
                                      ScopeObject *scope) /**< [in] A pointer to the ScopeObject structure to evaluate \a expr under. */
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
			fprintf(stderr, "Invalid operand type\n");
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
			fprintf(stderr, "Invalid operand type\n");
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

/** Interprets a boolean operation expression.
  *
  * \pre \a expr was created by createOpExprNode(OpType type, ExprNodeList *args)
  *      where \a type is either OP_AND, OP_OR, or OP_XOR and \a args was
  *      created by createExprNodeList(void) and populated with ExprNode
  *      structures using addExprNode(ExprNodeList *, ExprNode *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return A pointer to a ValueObject structure containing the interpreted
  *         operation expression value.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretArithOpExprNode(OpExprNode *, ScopeObject *)
  * \see interpretNotOpExprNode(OpExprNode *, ScopeObject *)
  * \see interpretEqualityOpExprNode(OpExprNode *, ScopeObject *)
  * \see interpretConcatOpExprNode(OpExprNode *, ScopeObject *) */
ValueObject *interpretBoolOpExprNode(OpExprNode *expr,   /**< [in] A pointer to the OpExprNode structure to interpret. */
                                     ScopeObject *scope) /**< [in] A pointer to the ScopeObject structure to evaluate \a expr under. */
{
	unsigned int n;
	int acc = 0;
	/* Proceed to apply the same operation on the accumulator for the
	 * remaining arguments. */
	for (n = 0; n < expr->args->num; n++) {
		ValueObject *val = interpretExprNode(expr->args->exprs[n], scope);
		ValueObject *use = val;
		unsigned int temp;
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
					fprintf(stderr, "Invalid boolean operation type\n");
					return NULL;
			}
		}
		/** \note The specification does not say whether boolean logic
		  *       short circuits or not.  Here, we assume it does. */
		if (expr->type == OP_AND && acc == 0) break;
		else if (expr->type == OP_OR && acc == 1) break;
	}
	return createBooleanValueObject(acc);
}

/** Tests if two integers are equal.
  *
  * \pre \a a and \a b were created by createIntegerValueObject(int).
  *
  * \return A pointer to a ValueObject structure containing a boolean value
  *         indicating whether \a a is equal to \a b.
  *
  * \see opNeqIntegerInteger(ValueObject *, ValueObject *) */
ValueObject *opEqIntegerInteger(ValueObject *a, /**< [in] The first value to test. */
                                ValueObject *b) /**< [in] The second value to test. */
{
	return createBooleanValueObject(getInteger(a) == getInteger(b));
}

/** Tests if two integers are not equal.
  *
  * \pre \a a and \a b were created by createIntegerValueObject(int).
  *
  * \return A pointer to a ValueObject structure containing a boolean value
  *         indicating whether \a a is not equal to \a b.
  *
  * \see opEqIntegerInteger(ValueObject *, ValueObject *) */
ValueObject *opNeqIntegerInteger(ValueObject *a, /**< [in] The first value to test. */
                                 ValueObject *b) /**< [in] The second value to test. */
{
	return createBooleanValueObject(getInteger(a) != getInteger(b));
}

/** Tests if an integer and a float are equal.
  *
  * \pre \a a was created by createIntegerValueObject(int) and \a b was created
  *      by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing a boolean value
  *         indicating whether \a a is equal to \a b.
  *
  * \see opNeqIntegerFloat(ValueObject *, ValueObject *) */
ValueObject *opEqIntegerFloat(ValueObject *a, /**< [in] The first value to test. */
                              ValueObject *b) /**< [in] The second value to test. */
{
	return createBooleanValueObject(getInteger(a) == getFloat(b));
}

/** Tests if an integer and a float are not equal.
  *
  * \pre \a a was created by createIntegerValueObject(int) and \a b was created
  *      by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing a boolean value
  *         indicating whether \a a is not equal to \a b.
  *
  * \see opEqIntegerFloat(ValueObject *, ValueObject *) */
ValueObject *opNeqIntegerFloat(ValueObject *a, /**< [in] The first value to test. */
                               ValueObject *b) /**< [in] The second value to test. */
{
	return createBooleanValueObject(getInteger(a) != getFloat(b));
}

/** Tests if a float and an integer are equal.
  *
  * \pre \a a was created by createFloatValueObject(float) and \a b was created
  *      by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing a boolean value
  *         indicating whether \a a is equal to \a b.
  *
  * \see opNeqFloatInteger(ValueObject *, ValueObject *) */
ValueObject *opEqFloatInteger(ValueObject *a, /**< [in] The first value to test. */
                              ValueObject *b) /**< [in] The second value to test. */
{
	return createBooleanValueObject(getFloat(a) == getInteger(b));
}

/** Tests if a float and an integer are not equal.
  *
  * \pre \a a was created by createFloatValueObject(float) and \a b was created
  *      by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing a boolean value
  *         indicating whether \a a is not equal to \a b.
  *
  * \see opEqFloatInteger(ValueObject *, ValueObject *) */
ValueObject *opNeqFloatInteger(ValueObject *a, /**< [in] The first value to test. */
                               ValueObject *b) /**< [in] The second value to test. */
{
	return createBooleanValueObject(getFloat(a) != getInteger(b));
}

/** Tests if two floats are equal.
  *
  * \pre \a a and \a b were created by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing a boolean value
  *         indicating whether \a a is equal to \a b.
  *
  * \see opNeqFloatFloat(ValueObject *, ValueObject *) */
ValueObject *opEqFloatFloat(ValueObject *a, /**< [in] The first value to test. */
                            ValueObject *b) /**< [in] The second value to test. */
{
	return createBooleanValueObject(getFloat(a) == getFloat(b));
}

/** Tests if two floats are not equal.
  *
  * \pre \a a and \a b were created by createFloatValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing a boolean value
  *         indicating whether \a a is not equal to \a b.
  *
  * \see opNeqFloatFloat(ValueObject *, ValueObject *) */
ValueObject *opNeqFloatFloat(ValueObject *a, /**< [in] The first value to test. */
                             ValueObject *b) /**< [in] The second value to test. */
{
	return createBooleanValueObject(getFloat(a) != getFloat(b));
}

/** Tests if two boolean values are equal.
  *
  * \pre \a a and \a b were created by createBooleanValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing a boolean value
  *         indicating whether \a a is equal to \a b.
  *
  * \see opNeqBooleanBoolean(ValueObject *, ValueObject *) */
ValueObject *opEqBooleanBoolean(ValueObject *a, /**< [in] The first value to test. */
                                ValueObject *b) /**< [in] The second value to test. */
{
	return createBooleanValueObject(getInteger(a) == getInteger(b));
}

/** Tests if two boolean values are not equal.
  *
  * \pre \a a and \a b were created by createBooleanValueObject(float).
  *
  * \return A pointer to a ValueObject structure containing a boolean value
  *         indicating whether \a a is not equal to \a b.
  *
  * \see opEqBooleanBoolean(ValueObject *, ValueObject *) */
ValueObject *opNeqBooleanBoolean(ValueObject *a, /**< [in] The first value to test. */
                                 ValueObject *b) /**< [in] The second value to test. */
{
	return createBooleanValueObject(getInteger(a) != getInteger(b));
}

/** Tests if two strings are equal.
  *
  * \pre \a a and \a b were created by createStringValueObject(char *).
  *
  * \return A pointer to a ValueObject structure containing a boolean value
  *         indicating whether \a a is equal to \a b.
  *
  * \see opNeqStringString(ValueObject *, ValueObject *) */
ValueObject *opEqStringString(ValueObject *a, /**< [in] The first value to test. */
                              ValueObject *b) /**< [in] The second value to test. */
{
	return createBooleanValueObject(strcmp(getString(a), getString(b)) == 0);
}

/** Tests if two strings are not equal.
  *
  * \pre \a a and \a b were created by createStringValueObject(char *).
  *
  * \return A pointer to a ValueObject structure containing a boolean value
  *         indicating whether \a a is not equal to \a b.
  *
  * \see opEqStringString(ValueObject *, ValueObject *) */
ValueObject *opNeqStringString(ValueObject *a, /**< [in] The first value to test. */
                               ValueObject *b) /**< [in] The second value to test. */
{
	return createBooleanValueObject(strcmp(getString(a), getString(b)) != 0);
}

/** Tests if two nil values are equal.
  *
  * \note Two nil values are \b always equal, therefore \a a and \a b are not
  *       used by this function but are still included in its prototype to allow
  *       this function to be stored in a jump table for fast execution.
  *
  * \pre \a a and \a b were created by createNilValueObject(void).
  *
  * \return A pointer to a ValueObject structure containing a boolean value
  *         indicating whether \a a is equal to \a b.
  *
  * \see opNeqNilNil(ValueObject *, ValueObject *) */
ValueObject *opEqNilNil(ValueObject *a, /**< Not used (see note). */
                        ValueObject *b) /**< Not used (see note). */
{
	a = NULL;
	b = NULL;
	return createBooleanValueObject(1);
}

/** Tests if two nil values are not equal.
  *
  * \note Two nil values are \b always equal and thus \b never not equal,
  *       therefore \a a and \a b are not used by this function but are still
  *       included in its prototype to allow this function to be stored in a
  *       jump table for fast execution.
  *
  * \pre \a a and \a b were created by createNilValueObject(void).
  *
  * \return A pointer to a ValueObject structure containing a boolean value
  *         indicating whether \a a is not equal to \a b.
  *
  * \see opEqNilNil(ValueObject *, ValueObject *) */
ValueObject *opNeqNilNil(ValueObject *a, /**< Not used (see note). */
                         ValueObject *b) /**< Not used (see note). */
{
	a = NULL;
	b = NULL;
	return createBooleanValueObject(0);
}

/* A jump table for boolean operations.  The first index determines the
 * particular boolean operation to perform, the second index determines the
 * type of the first argument, and the third index determines the type of the
 * second object. */
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

/** Interprets an equality operation expression.
  *
  * \pre \a expr was created by createOpExprNode(OpType type, ExprNodeList *args)
  *      where \a type is either OP_EQ or OP_NEQ and \a args was created by
  *      createExprNodeList(void) and populated with ExprNode structures using
  *      addExprNode(ExprNodeList *, ExprNode *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \note Only the first two elements in \a args are considered.
  *
  * \return A pointer to a ValueObject structure containing the interpreted
  *         operation expression value.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretArithOpExprNode(OpExprNode *, ScopeObject *)
  * \see interpretNotOpExprNode(OpExprNode *, ScopeObject *)
  * \see interpretBoolOpExprNode(OpExprNode *, ScopeObject *)
  * \see interpretConcatOpExprNode(OpExprNode *, ScopeObject *) */
ValueObject *interpretEqualityOpExprNode(OpExprNode *expr,   /**< [in] A pointer to the OpExprNode structure to interpret. */
                                         ScopeObject *scope) /**< [in] A pointer to the ScopeObject structure to evaluate \a expr under. */
{
	ValueObject *val1 = interpretExprNode(expr->args->exprs[0], scope);
	ValueObject *val2 = interpretExprNode(expr->args->exprs[1], scope);
	ValueObject *ret = NULL;
	if (!val1 || !val2) {
		deleteValueObject(val1);
		deleteValueObject(val2);
		return NULL;
	}
	/* Since there is no automatic casting, an equality (inequality) test
	 * against a non-number type will always fail (succeed). */
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
				fprintf(stderr, "Invalid equality operation type\n");
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

/** Interprets a concatenation operation expression.
  *
  * \pre \a expr was created by createOpExprNode(OpType type, ExprNodeList *args)
  *      where \a type is OP_CAT and \a args was created by createExprNodeList(void)
  *      and populated with ExprNode structures using addExprNode(ExprNodeList *, ExprNode *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return A pointer to a ValueObject structure containing the concatenation of
  *         all the arguments in \a args.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretNotOpExprNode(OpExprNode *, ScopeObject *)
  * \see interpretArithOpExprNode(OpExprNode *, ScopeObject *)
  * \see interpretBoolOpExprNode(OpExprNode *, ScopeObject *)
  * \see interpretEqualityOpExprNode(OpExprNode *, ScopeObject *) */
ValueObject *interpretConcatOpExprNode(OpExprNode *expr,   /**< [in] A pointer to the OpExprNode structure to interpret. */
                                       ScopeObject *scope) /**< [in] A pointer to the ScopeObject structure to evaluate \a expr under. */
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

/* A jump table for operations.  The index of a function in the table is given
 * by its its index in the enumerated OpType type. */
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

/** Interprets an operation expression.
  *
  * \pre \a expr was created by createOpExprNode(OpType, ExprNodeList *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return A pointer to a ValueObject structure containing the interpreted
  *         operation expression value.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretNotOpExprNode(OpExprNode *, ScopeObject *)
  * \see interpretArithOpExprNode(OpExprNode *, ScopeObject *)
  * \see interpretBoolOpExprNode(OpExprNode *, ScopeObject *)
  * \see interpretEqualityOpExprNode(OpExprNode *, ScopeObject *)
  * \see interpretConcatOpExprNode(OpExprNode *, ScopeObject *) */
ValueObject *interpretOpExprNode(ExprNode *node,     /**< [in] A pointer to an ExprNode structure to interpret. */
                                 ScopeObject *scope) /**< [in] A pointer to a ScopeObject structure to evaluate \a node under. */
{
	OpExprNode *expr = (OpExprNode *)node->expr;
	return OpExprJumpTable[expr->type](expr, scope);
}

/* A jump table for expressions.  The index of a function in the table is given
 * by its its index in the enumerated ExprType type. */
static ValueObject *(*ExprJumpTable[6])(ExprNode *, ScopeObject *) = {
	interpretCastExprNode,
	interpretConstantExprNode,
	interpretIdentifierExprNode,
	interpretFuncCallExprNode,
	interpretOpExprNode,
	interpretImpVarExprNode };

/** Interprets the contents of an ExprNode structure.
  *
  * \pre \a node was created by parseExprNode(Token ***, FunctionTable *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return A pointer to a ValueObject structure with the evaluated contents of
  *         \a node in the scope \a scope.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretStmtNode(StmtNode *, ScopeObject *)
  * \see interpretStmtNodeList(StmtNodeList *, ScopeObject *)
  * \see interpretBlockNode(BlockNode *, ScopeObject *)
  * \see interpretMainNode(MainNode *) */
ValueObject *interpretExprNode(ExprNode *node,     /**< [in] A pointer to an ExprNode structure to interpret. */
                               ScopeObject *scope) /**< [in] A pointer to a ScopeObject structure to evaluate \a node under. */
{
	return ExprJumpTable[node->type](node, scope);
}

/** Interprets a cast statement.
  *
  * \pre \a node was created by createStmtNode(StmtType type, void *stmt)
  *      where \a type is ST_CAST and \a stmt was created by createCastStmtNode(IdentifierNode *, TypeNode *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return A pointer to a ReturnObject structure with the default return value.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretPrintStmtNode(StmtNode *, ScopeObject *)
  * \see interpretInputStmtNode(StmtNode *, ScopeObject *)
  * \see interpretAssignmentStmtNode(StmtNode *, ScopeObject *)
  * \see interpretDeclarationStmtNode(StmtNode *, ScopeObject *)
  * \see interpretIfThenElseStmtNode(StmtNode *, ScopeObject *)
  * \see interpretSwitchStmtNode(StmtNode *, ScopeObject *)
  * \see interpretBreakStmtNode(StmtNode *, ScopeObject *)
  * \see interpretReturnStmtNode(StmtNode *, ScopeObject *)
  * \see interpretLoopStmtNode(StmtNode *, ScopeObject *)
  * \see interpretFuncDefStmtNode(StmtNode *, ScopeObject *)
  * \see interpretExprStmtNode(StmtNode *, ScopeObject *) */
ReturnObject *interpretCastStmtNode(StmtNode *node,     /**< [in] A pointer to the StmtNode structure containing the CastStmtNode structure to interpret. */
                                    ScopeObject *scope) /**< [in,out] A pointer to the ScopeObject structure to evaluate \a node under. */
{
	CastStmtNode *stmt = (CastStmtNode *)node->stmt;
	ValueObject *val = getScopeValue(scope, stmt->target);
	ValueObject *cast = NULL;
	if (!val) {
		IdentifierNode *id = (IdentifierNode *)(stmt->target);
		fprintf(stderr, "%s:%d: variable does not exist at: %s\n", id->fname, id->line, id->image);
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
	}
	if (!updateScopeValue(scope, stmt->target, cast)) {
		deleteValueObject(cast);
		return NULL;
	}
	return createReturnObject(RT_DEFAULT, NULL);
}

/** Interprets a print statement.
  *
  * \pre \a node was created by createStmtNode(StmtType type, void *stmt)
  *      where \a type is ST_PRINT and \a stmt was created by createPrintStmtNode(ExprNodeList *, int).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return A pointer to a ReturnObject structure with the default return value.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretCastStmtNode(StmtNode *, ScopeObject *)
  * \see interpretInputStmtNode(StmtNode *, ScopeObject *)
  * \see interpretAssignmentStmtNode(StmtNode *, ScopeObject *)
  * \see interpretDeclarationStmtNode(StmtNode *, ScopeObject *)
  * \see interpretIfThenElseStmtNode(StmtNode *, ScopeObject *)
  * \see interpretSwitchStmtNode(StmtNode *, ScopeObject *)
  * \see interpretBreakStmtNode(StmtNode *, ScopeObject *)
  * \see interpretReturnStmtNode(StmtNode *, ScopeObject *)
  * \see interpretLoopStmtNode(StmtNode *, ScopeObject *)
  * \see interpretFuncDefStmtNode(StmtNode *, ScopeObject *)
  * \see interpretExprStmtNode(StmtNode *, ScopeObject *) */
ReturnObject *interpretPrintStmtNode(StmtNode *node,     /**< [in] A pointer to a StmtNode structure containing the PrintStmtNode structure to interpret. */
                                     ScopeObject *scope) /**< [in] A pointer to the ScopeObject structure to evaluate \a node under. */
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

/** Interprets an input statement.
  *
  * \pre \a node was created by createStmtNode(StmtType type, void *stmt) where
  *      \a type is ST_INPUT and \a stmt was created by createInputStmtNode(IdentifierNode *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return A pointer to a ReturnObject structure with the default return value.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretCastStmtNode(StmtNode *, ScopeObject *)
  * \see interpretPrintStmtNode(StmtNode *, ScopeObject *)
  * \see interpretAssignmentStmtNode(StmtNode *, ScopeObject *)
  * \see interpretDeclarationStmtNode(StmtNode *, ScopeObject *)
  * \see interpretIfThenElseStmtNode(StmtNode *, ScopeObject *)
  * \see interpretSwitchStmtNode(StmtNode *, ScopeObject *)
  * \see interpretBreakStmtNode(StmtNode *, ScopeObject *)
  * \see interpretReturnStmtNode(StmtNode *, ScopeObject *)
  * \see interpretLoopStmtNode(StmtNode *, ScopeObject *)
  * \see interpretFuncDefStmtNode(StmtNode *, ScopeObject *)
  * \see interpretExprStmtNode(StmtNode *, ScopeObject *) */
ReturnObject *interpretInputStmtNode(StmtNode *node,     /**< [in] A pointer to a StmtNode structure containing an InputStmtNode structure to interpret. */
                                     ScopeObject *scope) /**< [in,out] A pointer to the ScopeObject structure to evaluate \a node under. */
{
	unsigned int size = 16;
	unsigned int cur = 0;
	char *temp = malloc(sizeof(char) * size);
	char c;
	void *mem = NULL;
	InputStmtNode *stmt = (InputStmtNode *)node->stmt;
	ValueObject *val = NULL;
	while ((c = getchar()) && !feof(stdin)) {
		/** \note The specification is unclear as to the exact semantics
		  *       of input.  Here, we read up until the first newline or
		  *       EOF but do not store it. */
		if (c == EOF || c == '\r' || c == '\n') break;
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
		temp[cur] = c;
		cur++;
	}
	temp[cur] = '\0';
	val = createStringValueObject(temp);
	if (!val) {
		free(temp);
		return NULL;
	}
	if (!updateScopeValue(scope, stmt->target, val)) {
		deleteValueObject(val);
		return NULL;
	}
	return createReturnObject(RT_DEFAULT, NULL);
}

/** Interprets an assignment statement.
  *
  * \pre \a node was created by createStmtNode(StmtType type, void *stmt) where
  *      \a type is ST_ASSIGNMENT and \a stmt was created by createAssignmentStmtNode(IdentifierNode *, ExprNode *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return A pointer to a ReturnObject structure with the default return value.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretCastStmtNode(StmtNode *, ScopeObject *)
  * \see interpretPrintStmtNode(StmtNode *, ScopeObject *)
  * \see interpretInputStmtNode(StmtNode *, ScopeObject *)
  * \see interpretDeclarationStmtNode(StmtNode *, ScopeObject *)
  * \see interpretIfThenElseStmtNode(StmtNode *, ScopeObject *)
  * \see interpretSwitchStmtNode(StmtNode *, ScopeObject *)
  * \see interpretBreakStmtNode(StmtNode *, ScopeObject *)
  * \see interpretReturnStmtNode(StmtNode *, ScopeObject *)
  * \see interpretLoopStmtNode(StmtNode *, ScopeObject *)
  * \see interpretFuncDefStmtNode(StmtNode *, ScopeObject *)
  * \see interpretExprStmtNode(StmtNode *, ScopeObject *) */
ReturnObject *interpretAssignmentStmtNode(StmtNode *node,     /**< [in] A pointer to a StmtNode structure containing the AssignmentStmtNode structure to interpret. */
                                          ScopeObject *scope) /**< [in,out] A pointer to the ScopeObject structure to evaluate \a node under. */
{
	AssignmentStmtNode *stmt = (AssignmentStmtNode *)node->stmt;
	ValueObject *val = interpretExprNode(stmt->expr, scope);
	if (!val) return NULL;
	if (!updateScopeValue(scope, stmt->target, val)) {
		deleteValueObject(val);
		return NULL;
	}
	return createReturnObject(RT_DEFAULT, NULL);
}

/** Interprets a declaration statement.
  *
  * \pre \a node was created by createStmtNode(StmtType type, void *stmt) where
  *      \a type is ST_DECLARATION and \a stmt was created by createDeclarationStmtNode(IdentifierNode *, IdentifierNode *, ExprNode *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return A pointer to a ReturnObject structure with the default return value.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretCastStmtNode(StmtNode *, ScopeObject *)
  * \see interpretPrintStmtNode(StmtNode *, ScopeObject *)
  * \see interpretInputStmtNode(StmtNode *, ScopeObject *)
  * \see interpretAssignmentStmtNode(StmtNode *, ScopeObject *)
  * \see interpretIfThenElseStmtNode(StmtNode *, ScopeObject *)
  * \see interpretSwitchStmtNode(StmtNode *, ScopeObject *)
  * \see interpretBreakStmtNode(StmtNode *, ScopeObject *)
  * \see interpretReturnStmtNode(StmtNode *, ScopeObject *)
  * \see interpretLoopStmtNode(StmtNode *, ScopeObject *)
  * \see interpretFuncDefStmtNode(StmtNode *, ScopeObject *)
  * \see interpretExprStmtNode(StmtNode *, ScopeObject *) */
ReturnObject *interpretDeclarationStmtNode(StmtNode *node,     /**< [in] A pointer to a StmtNode structure containing the DeclarationStmtNode structure to interpret. */
                                           ScopeObject *scope) /**< [in,out] A pointer to the ScopeObject structure to evaluate \a node under. */
{
	DeclarationStmtNode *stmt = (DeclarationStmtNode *)node->stmt;
	ValueObject *init = NULL;
	if (getLocalScopeValue(scope, stmt->target)) {
		IdentifierNode *id = (IdentifierNode *)(stmt->target);
		fprintf(stderr, "%s:%d: redefinition of existing variable at: %s\n", id->fname, id->line, id->image);
		return NULL;
	}
	if (stmt->expr)
		init = interpretExprNode(stmt->expr, scope);
	else
		init = createNilValueObject();
	if (!init) return NULL;
	if (!createScopeValue(scope, stmt->target)) {
		deleteValueObject(init);
		return NULL;
	}
	if (!updateScopeValue(scope, stmt->target, init)) {
		deleteValueObject(init);
		return NULL;
	}
	return createReturnObject(RT_DEFAULT, NULL);
}

/** Interprets an if/then/else statement.
  *
  * \pre \a node was created by createStmtNode(StmtType type, void *stmt) where
  *      \a type is ST_IFTHENELSE and \a stmt was created by createIfThenElseStmtNode(BlockNode *, BlockNode *, ExprNodeList *, BlockNodeList *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return A pointer to a ReturnObject structure with the default return value.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretCastStmtNode(StmtNode *, ScopeObject *)
  * \see interpretPrintStmtNode(StmtNode *, ScopeObject *)
  * \see interpretInputStmtNode(StmtNode *, ScopeObject *)
  * \see interpretAssignmentStmtNode(StmtNode *, ScopeObject *)
  * \see interpretDeclarationStmtNode(StmtNode *, ScopeObject *)
  * \see interpretSwitchStmtNode(StmtNode *, ScopeObject *)
  * \see interpretBreakStmtNode(StmtNode *, ScopeObject *)
  * \see interpretReturnStmtNode(StmtNode *, ScopeObject *)
  * \see interpretLoopStmtNode(StmtNode *, ScopeObject *)
  * \see interpretFuncDefStmtNode(StmtNode *, ScopeObject *)
  * \see interpretExprStmtNode(StmtNode *, ScopeObject *) */
ReturnObject *interpretIfThenElseStmtNode(StmtNode *node,     /**< [in] A pointer to a StmtNode structure containing the IfThenElseStmtNode structure to interpret. */
                                          ScopeObject *scope) /**< [in,out] A pointer to the ScopeObject structure to evaluate \a node under. */
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

/** Interprets a switch statement.
  *
  * \note The specification is unclear as to whether guards are implicitly cast
  *       to the type of the implicit variable.  This only matters in the case
  *       that mixed guard types are present and in this code, the action that
  *       is performed is the same as the comparison operator, that is, in order
  *       for a guard to match, both its type and value must match the implicit
  *       variable.
  *
  * \pre \a node was created by createStmtNode(StmtType type, void *stmt) where
  *      \a type is ST_SWITCH and \a stmt was created by createSwitchStmtNode(ExprNodeList *, BlockNodeList *, BlockNode *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return A pointer to a ReturnObject structure with the return value after
  *         interpreting \a node in the scope \a scope.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretCastStmtNode(StmtNode *, ScopeObject *)
  * \see interpretPrintStmtNode(StmtNode *, ScopeObject *)
  * \see interpretInputStmtNode(StmtNode *, ScopeObject *)
  * \see interpretAssignmentStmtNode(StmtNode *, ScopeObject *)
  * \see interpretDeclarationStmtNode(StmtNode *, ScopeObject *)
  * \see interpretIfThenElseStmtNode(StmtNode *, ScopeObject *)
  * \see interpretBreakStmtNode(StmtNode *, ScopeObject *)
  * \see interpretReturnStmtNode(StmtNode *, ScopeObject *)
  * \see interpretLoopStmtNode(StmtNode *, ScopeObject *)
  * \see interpretFuncDefStmtNode(StmtNode *, ScopeObject *)
  * \see interpretExprStmtNode(StmtNode *, ScopeObject *) */
ReturnObject *interpretSwitchStmtNode(StmtNode *node,     /**< [in] A pointer to the StmtNode structure containing the SwitchStmtNode structure to interpret. */
                                      ScopeObject *scope) /**< [in,out] A pointer to the ScopeObject structure to evaluate \a node under. */
{
	SwitchStmtNode *stmt = (SwitchStmtNode *)node->stmt;
	unsigned int n;
	/* Loop over each of the guards, checking if any match the implicit
	 * variable. */
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
					if (getFloat(use1) == getFloat(use2))
						done = 1;
					break;
				case VT_STRING:
					/** \note Strings with interpolation
					  *       should have already been
					  *       checked for. */
					if (!strcmp(getString(use1), getString(use2)))
						done = 1;
					break;
				default:
					fprintf(stderr, "Invalid type\n");
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
		/* Keep interpreting blocks starting at n until a break or
		 * return is encountered. */
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

/** Interprets a break statement.
  *
  * \pre \a node was created by createStmtNode(StmtType type, void *stmt) where
  *      \a type is ST_BREAK and \a stmt is NULL.
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \note \a node and \a scope are not used by this function but are still
  *       included in its prototype to allow this function to be stored in a
  *       jump table for fast execution.
  *
  * \return A pointer to a ReturnObject structure indicating a break occurred.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretCastStmtNode(StmtNode *, ScopeObject *)
  * \see interpretPrintStmtNode(StmtNode *, ScopeObject *)
  * \see interpretInputStmtNode(StmtNode *, ScopeObject *)
  * \see interpretAssignmentStmtNode(StmtNode *, ScopeObject *)
  * \see interpretDeclarationStmtNode(StmtNode *, ScopeObject *)
  * \see interpretIfThenElseStmtNode(StmtNode *, ScopeObject *)
  * \see interpretSwitchStmtNode(StmtNode *, ScopeObject *)
  * \see interpretReturnStmtNode(StmtNode *, ScopeObject *)
  * \see interpretLoopStmtNode(StmtNode *, ScopeObject *)
  * \see interpretFuncDefStmtNode(StmtNode *, ScopeObject *)
  * \see interpretExprStmtNode(StmtNode *, ScopeObject *) */
ReturnObject *interpretBreakStmtNode(StmtNode *node,     /**< Not used (see note). */
                                     ScopeObject *scope) /**< Not used (see note). */
{
	node = NULL;
	scope = NULL;
	return createReturnObject(RT_BREAK, NULL);
}

/** Interprets a return statement.
  *
  * \pre \a node was created by createStmtNode(StmtType type, void *stmt) where
  *      \a type is ST_RETURN and \a stmt was created by createReturnStmtNode(ExprNode *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return A pointer to a ReturnObject structure with the return value after
  *         interpreting \a node in the scope \a scope.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretCastStmtNode(StmtNode *, ScopeObject *)
  * \see interpretPrintStmtNode(StmtNode *, ScopeObject *)
  * \see interpretInputStmtNode(StmtNode *, ScopeObject *)
  * \see interpretAssignmentStmtNode(StmtNode *, ScopeObject *)
  * \see interpretDeclarationStmtNode(StmtNode *, ScopeObject *)
  * \see interpretIfThenElseStmtNode(StmtNode *, ScopeObject *)
  * \see interpretSwitchStmtNode(StmtNode *, ScopeObject *)
  * \see interpretBreakStmtNode(StmtNode *, ScopeObject *)
  * \see interpretLoopStmtNode(StmtNode *, ScopeObject *)
  * \see interpretFuncDefStmtNode(StmtNode *, ScopeObject *)
  * \see interpretExprStmtNode(StmtNode *, ScopeObject *) */
ReturnObject *interpretReturnStmtNode(StmtNode *node,     /**< [in] A pointer to the StmtNode structure containing the ReturnStmtNode structure to interpret. */
                                      ScopeObject *scope) /**< [in] A pointer to the ScopeObject structure to evaluate \a node under. */
{
	/* Evaluate and return the expression. */
	ReturnStmtNode *stmt = (ReturnStmtNode *)node->stmt;
	ValueObject *value = interpretExprNode(stmt->value, scope);
	if (!value) return NULL;
	return createReturnObject(RT_RETURN, value);
}

/** Interprets a loop statement.
  *
  * \pre \a node was created by createStmtNode(StmtType type, void *stmt) where
  *      \a type is ST_LOOP and \a stmt was created by createLoopStmtNode(IdentifierNode *, IdentifierNode *, ExprNode *, ExprNode *, BlockNode *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return A pointer to a ReturnObject structure with the return value after
  *         interpreting \a node in the scope \a scope.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretCastStmtNode(StmtNode *, ScopeObject *)
  * \see interpretPrintStmtNode(StmtNode *, ScopeObject *)
  * \see interpretInputStmtNode(StmtNode *, ScopeObject *)
  * \see interpretAssignmentStmtNode(StmtNode *, ScopeObject *)
  * \see interpretDeclarationStmtNode(StmtNode *, ScopeObject *)
  * \see interpretIfThenElseStmtNode(StmtNode *, ScopeObject *)
  * \see interpretSwitchStmtNode(StmtNode *, ScopeObject *)
  * \see interpretBreakStmtNode(StmtNode *, ScopeObject *)
  * \see interpretReturnStmtNode(StmtNode *, ScopeObject *)
  * \see interpretFuncDefStmtNode(StmtNode *, ScopeObject *)
  * \see interpretExprStmtNode(StmtNode *, ScopeObject *) */
ReturnObject *interpretLoopStmtNode(StmtNode *node,     /**< [in] A pointer to the StmtNode structure containing the LoopStmtNode structure to interpret. */
                                    ScopeObject *scope) /**< [in,out] A pointer to the ScopeObject structure to evaluate \a node under. */
{
	LoopStmtNode *stmt = (LoopStmtNode *)node->stmt;
	ScopeObject *outer = createScopeObject(scope);
	ValueObject *var = NULL;
	if (!outer) return NULL;
	/* Create a temporary loop variable if required */
	if (stmt->var) {
		var = createScopeValue(outer, stmt->var);
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
			/* A little efficiency hack: if we know the operation
			 * to perform, don't bother evaluating the ExprNode
			 * structure, just go ahead and do it to the loop
			 * variable. */
			if (stmt->update->type == ET_OP) {
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
				if (!updateScopeValue(outer, stmt->var, update)) {
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

/** Interprets a function definition statement.
  *
  * \pre \a node was created by createStmtNode(StmtType type, void *stmt) where
  *      \a type is ST_SWITCH and \a stmt was created by createSwitchStmtNode(IdentifierNode *, IdentifierNode *, IdentifierNodeList *, BlockNode *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \note \a node and \a scope are not used by this function but are still
  *       included in its prototype to allow this function to be stored in a
  *       jump table for fast execution.
  *
  * \return A pointer to a ReturnObject structure with the default return value.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretCastStmtNode(StmtNode *, ScopeObject *)
  * \see interpretPrintStmtNode(StmtNode *, ScopeObject *)
  * \see interpretInputStmtNode(StmtNode *, ScopeObject *)
  * \see interpretAssignmentStmtNode(StmtNode *, ScopeObject *)
  * \see interpretDeclarationStmtNode(StmtNode *, ScopeObject *)
  * \see interpretIfThenElseStmtNode(StmtNode *, ScopeObject *)
  * \see interpretSwitchStmtNode(StmtNode *, ScopeObject *)
  * \see interpretBreakStmtNode(StmtNode *, ScopeObject *)
  * \see interpretReturnStmtNode(StmtNode *, ScopeObject *)
  * \see interpretLoopStmtNode(StmtNode *, ScopeObject *)
  * \see interpretExprStmtNode(StmtNode *, ScopeObject *) */
ReturnObject *interpretFuncDefStmtNode(StmtNode *node,     /**< Not used (see note). */
                                       ScopeObject *scope) /**< Not used (see note). */
{
	/* Skip over this; we have already parsed and stored it */
	node = NULL;
	scope = NULL;
	return createReturnObject(RT_DEFAULT, NULL);
}

/** Interprets an expression statement.
  *
  * \pre \a node was created by createStmtNode(StmtType type, void *stmt) where
  *      \a type is ST_EXPR and \a stmt was created by createExprNode(ExprType, void *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return A pointer to a ReturnObject structure with the default return value.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretCastStmtNode(StmtNode *, ScopeObject *)
  * \see interpretPrintStmtNode(StmtNode *, ScopeObject *)
  * \see interpretInputStmtNode(StmtNode *, ScopeObject *)
  * \see interpretAssignmentStmtNode(StmtNode *, ScopeObject *)
  * \see interpretDeclarationStmtNode(StmtNode *, ScopeObject *)
  * \see interpretIfThenElseStmtNode(StmtNode *, ScopeObject *)
  * \see interpretSwitchStmtNode(StmtNode *, ScopeObject *)
  * \see interpretBreakStmtNode(StmtNode *, ScopeObject *)
  * \see interpretReturnStmtNode(StmtNode *, ScopeObject *)
  * \see interpretLoopStmtNode(StmtNode *, ScopeObject *)
  * \see interpretFuncDefStmtNode(StmtNode *, ScopeObject *) */
ReturnObject *interpretExprStmtNode(StmtNode *node,     /**< [in] A pointer to a StmtNode structure containing the ExprNode structure to interpret. */
                                    ScopeObject *scope) /**< [in,out] A pointer to the ScopeObject structure to evaluate \a node under. */
{
	/* Set the implicit variable to the result of the expression */
	ExprNode *expr = (ExprNode *)node->stmt;
	deleteValueObject(scope->impvar);
	scope->impvar = interpretExprNode(expr, scope);
	if (!scope->impvar) return NULL;
	return createReturnObject(RT_DEFAULT, NULL);
}

/* A jump table for statements.  The index of a function in the table is given
 * by its its index in the enumerated StmtType type. */
static ReturnObject *(*StmtJumpTable[12])(StmtNode *, ScopeObject *) = {
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
	interpretFuncDefStmtNode,
	interpretExprStmtNode };

/** Interprets the contents of a StmtNode structure.
  *
  * \pre \a node was created by parseStmtNode(Token ***, FunctionTable *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return A pointer to a ReturnObject structure with the return state of the
  *         interpreted \a node.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretExprNode(ExprNode *, ScopeObject *)
  * \see interpretStmtNodeList(StmtNodeList *, ScopeObject *)
  * \see interpretBlockNode(BlockNode *, ScopeObject *)
  * \see interpretMainNode(MainNode *) */
ReturnObject *interpretStmtNode(StmtNode *node,     /**< [in] A pointer to a StmtNode structure to interpret. */
                                ScopeObject *scope) /**< [in,out] A pointer to a ScopeObject structure to evaluate \a node under. */
{
	return StmtJumpTable[node->type](node, scope);
}

/** Interprets the contents of a StmtNodeList structure.
  *
  * \pre \a list was created by createStmtNodeList(void) and contains contents
  *      added by addStmtNode(StmtNodeList *, StmtNode *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return A pointer to a ReturnObject structure with the return state of the
  *         interpreted \a list.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretExprNode(ExprNode *, ScopeObject *)
  * \see interpretStmtNode(StmtNode *, ScopeObject *)
  * \see interpretBlockNode(BlockNode *, ScopeObject *)
  * \see interpretMainNode(MainNode *) */
ReturnObject *interpretStmtNodeList(StmtNodeList *list, /**< [in] A pointer to the StmtNodeList structure to interpret. */
                                    ScopeObject *scope) /**< [in,out] A pointer to the ScopeObject structure to evaluate \a list under. */
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

/** Interprets the contents of a BlockNode structure.
  *
  * \pre \a node was created by parseBlockNode(Token ***, FunctionTable *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return A pointer to a ReturnObject structure with the return state of the
  *         interpreted \a node.
  *
  * \retval NULL An error occurred during interpretation.
  *
  * \see interpretExprNode(ExprNode *, ScopeObject *)
  * \see interpretStmtNode(StmtNode *, ScopeObject *)
  * \see interpretStmtNodeList(StmtNodeList *, ScopeObject *)
  * \see interpretMainNode(MainNode *) */
ReturnObject *interpretBlockNode(BlockNode *node,    /**< [in] A pointer to a BlockNode structure to interpret. */
                                 ScopeObject *scope) /**< [in,out] A pointer to a ScopeObject structure to evaluate \a node under. */
{
	ReturnObject *ret = NULL;
	ScopeObject *inner = createScopeObject(scope);
	if (!inner) return NULL;
	ret = interpretStmtNodeList(node->stmts, inner);
	deleteScopeObject(inner);
	return ret;
}

/** Interprets the contents of a MainNode structure.
  *
  * \pre \a node was created by parseMainNode(Token **, FunctionTable *).
  * \pre \a scope was created by createScopeObject(ScopeObject *) and contains
  *      contents added by createScopeValue(ScopeObject *, IdentifierNode *) and
  *      contents updated by updateScopeValue(ScopeObject *, IdentifierNode *, ValueObject *).
  *
  * \return The return status of the interpreted MainNode structure.
  *
  * \retval 0 \a main was interpreted without any errors.
  * \retval 1 An error occurred while interpreting \a main.
  *
  * \see interpretExprNode(ExprNode *, ScopeObject *)
  * \see interpretStmtNode(StmtNode *, ScopeObject *)
  * \see interpretStmtNodeList(StmtNodeList *, ScopeObject *)
  * \see interpretBlockNode(BlockNode *, ScopeObject *) */
int interpretMainNode(MainNode *main) /**< [in] A pointer to the MainNode structure to interpret. */
{
	ReturnObject *ret = NULL;
	if (!main) return 1;
	ret = interpretBlockNode(main->block, NULL);
       	if (!ret) return 1;
	deleteReturnObject(ret);
	return 0;
}
