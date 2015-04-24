#ifndef TCB_H
#define TCB_H

#include <iostream>
//#include <string>
#include <cstdlib>
#include <queue>

#include "VirtualMachine.h"

using namespace std;

class TCB{
	
	public:
		TCB();
		virtual ~TCB();
		
		SMachineContextRef* exposeContext();
		TVMThreadPriority getPriority();
		void setPriority();
		
	private:	//these are the variables given in the function signature of VMThreadCreate()
		TVMThreadEntry entry;	//the thread's entry function, called when the thread is initially run
		void* param;					//parameters for the thread's entry function
		TVMMemorySize memsize;	//the size of the thread's stack
		TVMThreadPriority priority;	//the thread's priority
		TVMThreadIDRef tid;			//the thread's ID number
		SMachineContextRef context;	//the thread's context

		
};

#endif //TCB_H