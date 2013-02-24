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

void loadBinding(ScopeObject *); /* TODO: make this take an IdentifierNode library to load */

/**
 * TODO:
 *   - At startup, check which bindings are imported (using "CAN HAS" syntax)
 *   - Package those bindings as VT_FUNCs in BUKKITs
 *   - Add those bindings to the top-level scope (maybe create a new,
 *   higher-than-global scope for this)
 *   - To implement a new binding, specify the interface, which will contain a
 *   BlockNode that contains a StmtListNode with one statement: a
 *   BindingStmtNode that implements a native function that modifies directly
 *   the scope.
 *   - Binding creation/deletion helper functions.

IdentifierNode *scope = ??? what goes here again?;
IdentifierNode *name = createIdentifierNode("FileOpen");
IdentifierNodeList *args = ... add these ...
BlockNode *block = ... block with appropriate BindingStmtNode ...
FuncDefStmtNode *def = createFuncDefStmtNode();
Binding *binding = createBinding("STDIO", def);

*/

#endif /* __BINDING_H__ */
