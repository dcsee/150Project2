#ifndef THREADSTORE_H
#define THREADSTORE_H

#include "TCB.h"

#include <iostream>
//#include <string>
#include <cstdlib>
#include <queue>

using namespace std;

class ThreadStore{
	
	public:
		ThreadStore();
		static ThreadStore& getThreadStore();
		virtual ~ThreadStore();
		void sayHi();
		void addThread(TCB n);
	
	private:
		TCB* idleThread;
		ThreadStore(const &tStore);
		queue<TCB>* lowPriorityQueue;
//		queue midPriorityQueue;
//		queue highPriorityQueue;

};

#endif //THREADSTORE_H