#include "binding.h"

struct returnobject;
struct scopeobject;
struct returnobject *fopenWrapper(struct scopeobject *scope)
{
    fprintf(stderr, "HERE\n");
}

void loadBinding(ScopeObject *scope)
{
    /* TODO: add error checking and handling */

    /* stdio */

    Binding *fopen = malloc(sizeof(Binding));

    fopen->library = createIdentifierNode(IT_DIRECT, (void *)copyString("STDIO"), NULL, NULL, 0);

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
    fopen->interface = createFuncDefStmtNode(NULL, name, args, body);
    ValueObject *val = createFunctionValueObject(fopen->interface);

    createScopeValue(scope, scope, name);
    updateScopeValue(scope, scope, name, val);

    return;

loadBindingAbort: /* Exception handling */

    return;
}
