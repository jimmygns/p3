/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
#include "symtable.h"        
         
Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this); 
}

VarDecl::VarDecl(Identifier *n, Type *t, Expr *e) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
    if (e) (assignTo=e)->SetParent(this);
    typeq = NULL;
}

VarDecl::VarDecl(Identifier *n, TypeQualifier *tq, Expr *e) : Decl(n) {
    Assert(n != NULL && tq != NULL);
    (typeq=tq)->SetParent(this);
    if (e) (assignTo=e)->SetParent(this);
    type = NULL;
}

VarDecl::VarDecl(Identifier *n, Type *t, TypeQualifier *tq, Expr *e) : Decl(n) {
    Assert(n != NULL && t != NULL && tq != NULL);
    (type=t)->SetParent(this);
    (typeq=tq)->SetParent(this);
    if (e) (assignTo=e)->SetParent(this);
}
  
void VarDecl::PrintChildren(int indentLevel) { 
   if (typeq) typeq->Print(indentLevel+1);
   if (type) type->Print(indentLevel+1);
   if (id) id->Print(indentLevel+1);
   if (assignTo) assignTo->Print(indentLevel+1, "(initializer) ");

}

void VarDecl::Check(){
    Symbol *sym = Node::symtab->find(this->GetIdentifier()->GetName());
    if(sym&&sym->someInfo==0){
        ReportError::DeclConflict(this, sym->decl);
        Node::symtab->remove(*sym);
    }
    if(this->assignTo){
        Type *actual_type = this->assignTo->CheckExpr();
        if(!actual_type->IsEquivalentTo(this->type)){
            ReportError::InvalidInitialization(this->GetIdentifier(), this->type, actual_type);
        }
    }
    sym = new Symbol(this->GetIdentifier()->GetName(), this, E_VarDecl);
    Node::symtab->insert(*sym);
    
}

FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    (returnType=r)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;
    returnTypeq = NULL;
}

FnDecl::FnDecl(Identifier *n, Type *r, TypeQualifier *rq, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r != NULL && rq != NULL&& d != NULL);
    (returnType=r)->SetParent(this);
    (returnTypeq=rq)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;
}

void FnDecl::SetFunctionBody(Stmt *b) { 
    (body=b)->SetParent(this);
}

void FnDecl::PrintChildren(int indentLevel) {
    if (returnType) returnType->Print(indentLevel+1, "(return type) ");
    if (id) id->Print(indentLevel+1);
    if (formals) formals->PrintAll(indentLevel+1, "(formals) ");
    if (body) body->Print(indentLevel+1, "(body) ");
}

void FnDecl::Check(){
    Symbol *sym = Node::symtab->find(this->GetIdentifier()->GetName());
    if(sym&&sym->someInfo==0){
        ReportError::DeclConflict(this, sym->decl);
        Node::symtab->remove(*sym);
    }
    
    sym = new Symbol(this->GetIdentifier()->GetName(), this, E_FunctionDecl);
    Node::symtab->insert(*sym);
    Node::symtab->push();
    Node::symtab->setReturnType(this->GetType());
    Node::isFnDecl = true;
    if ( this->GetFormals()->NumElements() > 0 ) {
        for ( int i = 0; i < this->GetFormals()->NumElements(); ++i ) {
            VarDecl *vd = this->GetFormals()->Nth(i);
            vd->Check();
        }
    }
    StmtBlock* body_stmt = dynamic_cast<StmtBlock *>(this->GetBody());

    

    body_stmt->Check();

    
    Type *returned = Node::symtab->getType(); 
    if(!returned->IsEquivalentTo(Type::voidType)){
        ReportError::ReturnMissing(this);

    }
    Node::symtab->pop();

    
    

}

