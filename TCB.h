#ifndef TCB_H
#define TCB_H

#include <iostream>
//#include <string>
#include <cstdlib>
#include <queue>

using namespace std;

class TCB{
	
	public:
		TCB();
		virtual ~TCB();
	
	private:	//these are the variables given in the function signature of VMThreadCreate()
		TVMThreadEntry entry;
		void* param;
		TVMMemorySize memsize;
		TVMThreadPriority prio;
		TVMThreadIDRef tid;

		
};

#endif //TCB_H