#include "TCB.h"
#include <iostream>

TCB::TCB(){

}

TCB::~TCB(){
	
}

void TCB::setPriority(TVMThreadPriority p){
	priority = p;
}

TVMThreadPriority TCB::getPriority(){
	return priority;
}

SMachineContextRef* TCB::exposeContext(){
//returns the address of the machine's context, this lets
//MachineContextSwitch() save the machine context into context

	return &context;

}