/*
 * Symbol table implementation
 *
 */
#include "symtable.h"

using namespace std;

ScopedTable::ScopedTable(){
}

ScopedTable::~ScopedTable(){
	this->symbols.clear();
}

void ScopedTable::addReturnType(Type *t){
	this->type = t;
}

void ScopedTable::insert(Symbol &sym){

	this->symbols.insert(pair<char*, Symbol>(sym.name, sym));

}

void ScopedTable::remove(Symbol &sym){
	
	this->symbols.erase(sym.name);
}

Symbol *ScopedTable::find(const char *name){
	SymbolIterator it = this->symbols.find(name);
	if (it == this->symbols.end()){
		return NULL;
	}

	return &(this->symbols.find(name)->second);
}

SymbolTable::SymbolTable(){

}

SymbolTable::~SymbolTable(){
	for(vector<ScopedTable*>::iterator it = this->tables.begin(); it != this->tables.end(); ++it){
		delete *it;
	}
}

void SymbolTable::setReturnType(Type *t){
	this->tables.back()->addReturnType(t);
}


Type *SymbolTable::getType(){
	return this->tables.back()->getType();
}



void SymbolTable::push(){
	this->tables.push_back(new ScopedTable());
}

void SymbolTable::pop(){

	this->tables.pop_back();
}

void SymbolTable::insert(Symbol &sym){
	ScopedTable *st = this->tables.back();
	st->insert(sym);
}

void SymbolTable::remove(Symbol &sym){
	this->tables.back()->remove(sym);
}

Symbol *SymbolTable::find(const char *name){
	return this->tables.back()->find(name);

}

bool MyStack::insideLoop(){

	for(vector<Stmt*>::iterator it = this->stmtStack.begin(); it != this->stmtStack.end(); ++it){
		LoopStmt *l = dynamic_cast<LoopStmt *>(*it);
		if(l)
			return true;
	}
	return false;
}

bool MyStack::insideSwitch(){

	for(vector<Stmt*>::iterator it = this->stmtStack.begin(); it != this->stmtStack.end(); ++it){
		SwitchStmt *sw = dynamic_cast<SwitchStmt *>(*it);
		if(sw)
			return true;
	}
	return false;
}

