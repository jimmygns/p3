/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */

#include <string.h>
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "symtable.h"

IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    value = val;
}
void IntConstant::Check() {
    this->type = Type::intType;
}
void IntConstant::PrintChildren(int indentLevel) { 
    printf("%d", value);
}

FloatConstant::FloatConstant(yyltype loc, double val) : Expr(loc) {
    value = val;
}
void FloatConstant::Check() {
    this->type = Type::floatType;
}
void FloatConstant::PrintChildren(int indentLevel) { 
    printf("%g", value);
}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
}
void BoolConstant::Check() {
    this->type = Type::boolType;
}
void BoolConstant::PrintChildren(int indentLevel) { 
    printf("%s", value ? "true" : "false");
}

VarExpr::VarExpr(yyltype loc, Identifier *ident) : Expr(loc) {
    Assert(ident != NULL);
    this->id = ident;
}
void VarExpr::Check() {
    Symbol *sym = Node::symtab->find(this->GetIdentifier()->GetName());
    if(!sym){
        ReportError::IdentifierNotDeclared(this->GetIdentifier(), LookingForVariable);
    }
}
void VarExpr::PrintChildren(int indentLevel) {
    id->Print(indentLevel+1);
}

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
}

void Operator::PrintChildren(int indentLevel) {
    printf("%s",tokenString);
}

bool Operator::IsOp(const char *op) const {
    return strcmp(tokenString, op) == 0;
}

CompoundExpr::CompoundExpr(Expr *l, Operator *o, Expr *r) 
  : Expr(Join(l->GetLocation(), r->GetLocation())) {
    Assert(l != NULL && o != NULL && r != NULL);
    (op=o)->SetParent(this);
    (left=l)->SetParent(this); 
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Operator *o, Expr *r) 
  : Expr(Join(o->GetLocation(), r->GetLocation())) {
    Assert(o != NULL && r != NULL);
    left = NULL; 
    (op=o)->SetParent(this);
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Expr *l, Operator *o) 
  : Expr(Join(l->GetLocation(), o->GetLocation())) {
    Assert(l != NULL && o != NULL);
    (left=l)->SetParent(this);
    (op=o)->SetParent(this);
}


void CompoundExpr::PrintChildren(int indentLevel) {
   if (left) left->Print(indentLevel+1);
   op->Print(indentLevel+1);
   if (right) right->Print(indentLevel+1);
}

Type* ArithmeticExpr::CheckExpr(){
    bool is_unary = false;
    Type* l_type = NULL;
    Type* r_type = NULL;
    if(left){
        l_type = left->CheckExpr();
        r_type = right->CheckExpr();
        if(l_type->isError() || r_type->isError()){
            return Type::errorType;
        }

        if (!l_type->IsEquivalentTo(r_type)){
            ReportError::IncompatibleOperands(op, l_type, r_type);
            return Type::errorType;
        }
    }
    else{
        is_unary = true;
        r_type = right->CheckExpr();
        if(r_type->isError()){
            return Type::errorType;
        }
    }

    if (r_type->IsNumeric()||r_type->IsMatrix()||r_type->IsVector())
    {
        return r_type;
    }
    else{
        if(is_unary){
            ReportError::IncompatibleOperand(op, r_type);
        }
        else{
            ReportError::IncompatibleOperands(op, l_type, r_type);
        }
        
        return Type::errorType;
    }
}

void ArithmeticExpr::Check(){
    this->CheckExpr();
}

Type *RelationalExpr::CheckExpr(){
    Type *l_type = left->CheckExpr();
    Type *r_type = right->CheckExpr();

    if(l_type->isError() || r_type->isError()){
        return Type::errorType;
    }

    if (!l_type->IsEquivalentTo(r_type)){
        ReportError::IncompatibleOperands(op, l_type, r_type);
        return Type::errorType;
    }
    if (r_type->IsNumeric())
    {
        return Type::boolType;
    }
    else{
        ReportError::IncompatibleOperands(op, l_type, r_type);
        return Type::errorType;
    }

}

void RelationalExpr::Check(){
    this->CheckExpr();
}

Type *EqualityExpr::CheckExpr(){
    Type *l_type = left->CheckExpr();
    Type *r_type = right->CheckExpr();

    if(l_type->isError() || r_type->isError()){
        return Type::errorType;
    }

    if (!l_type->IsEquivalentTo(r_type)){
        ReportError::IncompatibleOperands(op, l_type, r_type);
        return Type::errorType;
    }
    
    return Type::boolType;
    
}

void EqualityExpr::Check(){
    this->CheckExpr();
}

Type *LogicalExpr::CheckExpr(){
    bool is_unary = false;
    Type* l_type = NULL;
    Type* r_type = NULL;
    if(left){
        l_type = left->CheckExpr();
        r_type = right->CheckExpr();
        if(l_type->isError() || r_type->isError()){
            return Type::errorType;
        }

        if (!l_type->IsEquivalentTo(r_type)){
            ReportError::IncompatibleOperands(op, l_type, r_type);
            return Type::errorType;
        }
    }
    else{
        is_unary = true;
        r_type = right->CheckExpr();
        if(r_type->isError()){
            return Type::errorType;
        }
    }

    if (r_type->IsConvertibleTo(Type::boolType))
    {
        return Type::boolType;
    }
    else{
        if(is_unary){
            ReportError::IncompatibleOperand(op, r_type);
        }
        else{
            ReportError::IncompatibleOperands(op, l_type, r_type);
        }
        
        return Type::errorType;
    }
}

void LogicalExpr::Check(){
    this->CheckExpr();
}

Type *AssignExpr::CheckExpr(){
    Type *l_type = left->CheckExpr();
    Type *r_type = right->CheckExpr();

    if(l_type->isError() || r_type->isError()){
        return Type::errorType;
    }

    if (!l_type->IsEquivalentTo(r_type)){
        ReportError::IncompatibleOperands(op, l_type, r_type);
        return Type::errorType;
    }
    
    return r_type;
}

void AssignExpr::Check(){
    this->CheckExpr();
}

Type *PostfixExpr::CheckExpr(){
    r_type = right->CheckExpr();
    if(r_type->isError()){
        return Type::errorType;
    }
    if(r_type->IsNumeric()||r_type->IsMatrix()||r_type->IsVector()){
        return r_type;
    }
    else{
        ReportError::IncompatibleOperand(op, r_type);
        return Type::errorType;
    }

}

void PostfixExpr::Check(){
    this->CheckExpr();
}
   
ConditionalExpr::ConditionalExpr(Expr *c, Expr *t, Expr *f)
  : Expr(Join(c->GetLocation(), f->GetLocation())) {
    Assert(c != NULL && t != NULL && f != NULL);
    (cond=c)->SetParent(this);
    (trueExpr=t)->SetParent(this);
    (falseExpr=f)->SetParent(this);
}
void ConditionalExpr::Check() {
    Type *cond_type = cond->CheckExpr();
    trueExpr->Check();
    falseExpr->Check();

    if(!cond_type->IsConvertibleTo(Type::boolType)){
        ReportError::TestNotBoolean(cond);
    }
}


void ConditionalExpr::PrintChildren(int indentLevel) {
    cond->Print(indentLevel+1, "(cond) ");
    trueExpr->Print(indentLevel+1, "(true) ");
    falseExpr->Print(indentLevel+1, "(false) ");
}
ArrayAccess::ArrayAccess(yyltype loc, Expr *b, Expr *s) : LValue(loc) {
    (base=b)->SetParent(this); 
    (subscript=s)->SetParent(this);
}
Type *ArrayAccess::CheckExpr() {
    VarExpr * b = dynamic_cast<VarExpr*> base;
    if(!b){
        ReportError::NotAnArray(b->GetIdentifier());
        return Type::errorType;
    }
    ArrayType *b_type = dynamic_cast<ArrayType*>(base->CheckExpr());
    //subscript->Check();

    if(!b_type){
        ReportError::NotAnArray(b->GetIdentifier());
        return Type::errorType;
    }
    return b_type->GetElemType();
}
void ArrayAccess::PrintChildren(int indentLevel) {
    base->Print(indentLevel+1);
    subscript->Print(indentLevel+1, "(subscript) ");
}
     
FieldAccess::FieldAccess(Expr *b, Identifier *f) 
  : LValue(b? Join(b->GetLocation(), f->GetLocation()) : *f->GetLocation()) {
    Assert(f != NULL); // b can be be NULL (just means no explicit base)
    base = b; 
    if (base) base->SetParent(this); 
    (field=f)->SetParent(this);
}
void FieldAccess::Check() {
    //TODO
}
void FieldAccess::PrintChildren(int indentLevel) {
    if (base) base->Print(indentLevel+1);
    field->Print(indentLevel+1);
}

Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
    Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
    base = b;
    if (base) base->SetParent(this);
    (field=f)->SetParent(this);
    (actuals=a)->SetParentAll(this);
}
void Call::Check() {
    //TODO
    //LessFormals, ExtraFormals, FormalsTypeMismatch, NotAFunction, 
}
void Call::PrintChildren(int indentLevel) {
   if (base) base->Print(indentLevel+1);
   if (field) field->Print(indentLevel+1);
   if (actuals) actuals->PrintAll(indentLevel+1, "(actuals) ");
}

