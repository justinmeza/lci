/**
 * Structures and functions for binding to external libraries.
 *
 * \file   binding.h
 *
 * \author Justin J. Meza
 *
 * \date   2013
 */

#ifndef __BINDING_H__
#define __BINDING_H__

#include <stdio.h>

#include "interpreter.h"

/**
 * Stores a binding to an external library call to export.
 */
typedef struct {
	IdentifierNode *library;    /**< The library this binding belongs to. */
	FuncDefStmtNode *interface; /**< The interface that exports the binding. */
} Binding;

ValueObject *getArg(struct scopeobject *, char *);
void loadLibrary(ScopeObject *, IdentifierNode *);
void loadBinding(ScopeObject *, char *, const char *, struct returnobject *(*)(struct scopeobject *));

#endif /* __BINDING_H__ */
