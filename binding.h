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

ValueObject *getArg(struct scopeobject *, char *);
void loadLibrary(ScopeObject *, IdentifierNode *);
void loadBinding(ScopeObject *, char *, const char *, struct returnobject *(*)(struct scopeobject *));

#endif /* __BINDING_H__ */
