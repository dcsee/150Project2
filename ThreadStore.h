#ifndef THREADSTORE_H
#define THREADSTORE_H

#include <iostream>
//#include <string>
#include <cstdlib>
#include <queue>

using namespace std;

class ThreadStore{
	
	public:
		ThreadStore();
		virtual ~ThreadStore();
		void sayHi();
		void addNumber(int n);
	
	private:
		queue<int>* lowPriorityQueue;
//		queue midPriorityQueue;
//		queue highPriorityQueue;

		
};

#endif //THREADSTORE_H