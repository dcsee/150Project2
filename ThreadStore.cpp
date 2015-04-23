#include "ThreadStore.h"
#include <iostream>

ThreadStore::ThreadStore(){
	lowPriorityQueue = new queue<int>;
}

ThreadStore::~ThreadStore(){
	
}

void ThreadStore::addNumber(int n){
	
	lowPriorityQueue->push(n);
	
}

void ThreadStore::sayHi(){
	int n = lowPriorityQueue->front();
	lowPriorityQueue->pop();
	cout << endl << n << endl;
}