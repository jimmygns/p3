/*
 * Symbol table implementation
 *
 */
#include "symtable.h"

using namespace std;

ScopedTable::ScopedTable(){
	this->type = NULL;
}

ScopedTable::~ScopedTable(){
	this->symbols.clear();
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
	this->return_type = NULL;

}

SymbolTable::~SymbolTable(){
	for(vector<ScopedTable*>::iterator it = this->tables.begin(); it != this->tables.end(); ++it){
		delete *it;
	}
}

void SymbolTable::setReturnType(Type *t){
	this->return_type = t;
}


Type *SymbolTable::getType(){

	
	return this->return_type;
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
	Symbol *sym = this->tables.back()->find(name);
	if(sym){
		sym->someinfo=0;
	    return sym;
	}
	for(vector<ScopedTable*>::iterator it = this->tables.begin(); it != this->tables.end(); ++it){
	    sym = *it->find(name);
	    if(sym){
			sym->someinfo=1;
			return sym;
	    }
		
	}


	return NULL;

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

