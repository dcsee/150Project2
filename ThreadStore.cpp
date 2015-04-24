#include "ThreadStore.h"
#include <iostream>

//ThreadStore::getThreadStore(){
//	return store;
//}

ThreadStore::ThreadStore(){
	lowPriorityQueue = new queue<TCB>;

	//creates a default "idle thread" with priority zero
	//	idleThread = new TCB();
}

ThreadStore::~ThreadStore(){
	
}

void ThreadStore::addThread(TCB n){
	lowPriorityQueue->push(n);
}

void ThreadStore::sayHi(){
}