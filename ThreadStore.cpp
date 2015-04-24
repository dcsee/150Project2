#include "ThreadStore.h"
#include <iostream>

//ThreadStore::getThreadStore(){
//	return store;
//}

ThreadStore::ThreadStore(){
	lowPriorityQueue = new queue<TCB>;

	//Here, create a new thread and TCB for the idle thread. Assign it priority 0
	//	idleThread = new TCB();
}

ThreadStore::~ThreadStore(){
	
}

void ThreadStore::addThread(TCB n){
	lowPriorityQueue->push(n);
}

int ThreadStore::getAlarmCounter(){
	return alarmCounter;
}

void ThreadStore::setAlarmCounter(int ticks){
	
	alarmCounter = ticks;
	
}