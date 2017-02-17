/*
 * Symbol table implementation
 *
 */
#include "symtable.h"

using namespace std;

ScopedTable::ScopedTable(){
}

void ScopedTable::insert(Symbol &sym){

	this->symbols.insert(pair<char*, Symbol>(sym.name, sym));

}

void ScopedTable::remove(Symbol &sym){
	
	this->symbols.erase(sym.name);
}

Symbol *ScopedTable::find(const char *name){

	return &(this->symbols.find(name)->second);
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

