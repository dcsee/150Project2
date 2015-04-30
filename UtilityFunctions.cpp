#include "VirtualMachine.h"
#include "Machine.h"
#include "UtilityFunctions.h"
#include <stdint.h>
#include <stdio.h>

//=== TCB =======================================================

extern "C" {
	TCB::~TCB(){
/*		delete (uint8_t*)(stackBaseAddress);	//free memory called by new
		safeEntryParam[0] = NULL;
		safeEntryParam[1] = NULL;
		delete safeEntryParam;
		contextRef = NULL;
		delete contextRef;*/
	}
	
	TCB::TCB(TVMThreadEntry entry, void *param, TVMMemorySize memsize, TVMThreadPriority prio, TVMThreadIDRef tid){
		//Initialize all variables in thread control block
		//These initializations are common to all threads:

		//assign IDs in ascending order, return the ID to the calling function by reference
		threadID = ThreadStore::getInstance()->getNumThreads() + 1;
		*tid = threadID;

		TVMThreadEntry safeEntryFunction 	= &safeEntry;				//this is the skeleton function
		safeEntryParam 										= new int*[2];
		safeEntryParam[0]									= (int *)entry;			//skeleton function's params - [0] is thread entry func, [1] is param for thread entry func
		safeEntryParam[1]									= (int *)param;
	
		priority 				 	= prio;
		entryFunction 		= entry;
		entryParam 				= param;			//cast param to int*, assign
		stackSize 			 	= memsize;
		ticksToSleep		 	= 0;
		fileWriteResult		= 0;
		fileReadResult		= 0;
		fileOpenResult		= 0;
		fileCloseResult		= 0;
		fileSeekResult		= 0;
		contextRef 				= new SMachineContext[1];
		waitingOnIO 		= false;
		//mutex = mutex; //a mutex, when I get to that point
		
		if(entry == (TVMThreadEntry)&dummyReturn){				//dummyReturn indicates creation of vmStart thread, which already has a context 
			//vmStart is running
			state 					 = VM_THREAD_STATE_RUNNING;			//vmStart is obviously running
			stackBaseAddress = new uint8_t[1];							//may need to change this later
		}
		else if(entry == (TVMThreadEntry)&idle){					//initializing idle thread
			state = VM_THREAD_STATE_READY;
			stackBaseAddress 	= new uint8_t[memsize];				//allocate stack
			//assign context to new machine context, e.g. context = Machine->newContext(params...);
			MachineContextCreate(contextRef, &idle, NULL, stackBaseAddress, stackSize);
		}
		else{	
			state 					 	= VM_THREAD_STATE_DEAD;				//All other threads begin in dead state
			stackBaseAddress 	= new uint8_t[memsize];				//allocate stack
			//assign context to new machine context, e.g. context = Machine->newContext(params...);
			MachineContextCreate(contextRef, safeEntryFunction, safeEntryParam, stackBaseAddress, stackSize);
		}
	}
}

void TCB::setFileSeekResult(int result){
	fileSeekResult = result;
}

int TCB::getFileSeekResult(){
	return fileSeekResult;
}

void TCB::setFileReadResult(int result){
	fileReadResult = result;
}

int TCB::getFileReadResult(){
	return fileReadResult;
}

void TCB::setFileCloseResult(int result){
	fileCloseResult = result;
}

int TCB::getFileCloseResult(){
	return fileCloseResult;
}

void TCB::setFileOpenResult(int result){
	fileOpenResult = result;
}

int TCB::getFileOpenResult(){
	return fileOpenResult;
}

void TCB::setIsWaitingOnIO(bool waiting){
	waitingOnIO = waiting;
}

void TCB::setFileWriteResult(int result){
	fileWriteResult = result;
}

int TCB::getFileWriteResult(){
	return fileWriteResult;
}

bool TCB::isWaitingOnIO(){
	return waitingOnIO;
}

void TCB::releaseMutexes(){
	printf("TCB::releaseMutexes()\n");
}

TVMThreadID TCB::getThreadID(){
	return threadID;
}

TVMThreadPriority TCB::getPriority(){
	return priority;
}

TVMThreadState TCB::getState(){
	return state;
}

TVMThreadEntry TCB::getEntryFunction(){
	return entryFunction;
}

void TCB::setState(TVMThreadState s){
	state = s;
}

void TCB::sleep(TVMTick tick){
	state = VM_THREAD_STATE_WAITING;	//puts the thread into the waiting state for tick ticks
	ticksToSleep = tick;	//sets how long the thread should sleep
}

TVMTick TCB::getTicksToSleep(){
	return ticksToSleep;
}

void TCB::decrementTicksToSleep(){
	ticksToSleep = ticksToSleep - 1;
}


//=================================================================





//=== ThreadStore ==================================================

ThreadStore *ThreadStore::DUniqueInstance = NULL;

void ThreadStore::sleepCurrentThread(TVMTick tick){

	currentThread->sleep(tick);	//set current thread's ticksToSleep and puts it in waiting state

	if(tick == 0)															//if sleep was called with timeout immediate
		schedule(currentThread->getPriority());	//attempt to schedule new thread of current priority
	else
		schedule(VM_THREAD_PRIORITY_HIGH);			//attempt to schedule high priority thread
}

void ThreadStore::waitCurrentThreadOnIO(){

	printf("waitCurrentThreadOnIO(): \n");
	currentThread->setIsWaitingOnIO(true);	//set current thread's ticksToSleep
	currentThread->setState(VM_THREAD_STATE_WAITING);
	schedule(VM_THREAD_PRIORITY_HIGH);
	//sleep calls schedule
	return;
}

void ThreadStore::deleteDeadThread(TCB* thread){

	for(std::list<TCB*>::iterator TCBItr = deadList->begin(); TCBItr != deadList->end(); TCBItr++){
		//check deadList
	
		if((*TCBItr)->getThreadID() == thread->getThreadID()){	//if found id matches requested id
			//printf("\ndeleteDeadThread(): found match in deadList: %d\n", (*TCBItr)->getThreadID());
			deadList->erase(TCBItr);
			delete thread;
			break;
		}
	}
}

TCB* ThreadStore::findThreadByID(TVMThreadID id){
	//searches every container for a thread matching id

	for(TVMThreadPriority priority = 0x01; priority < 0x04; priority++){	//check all ready lists except idle list
		
		for(std::list<TCB*>::iterator TCBItr = readyLists[priority]->begin(); TCBItr != readyLists[priority]->end(); TCBItr++){
			//check every item in readyList[priority]
		
			if((*TCBItr)->getThreadID() == id){	//if found id matches requested id
				//printf("\nfindThreadByID(): found match: %d\n", (*TCBItr)->getThreadID());
				return *TCBItr;	//return TCB*
			}
		}
	}
	
	for(std::list<TCB*>::iterator TCBItr = waitList->begin(); TCBItr != waitList->end(); TCBItr++){ //check wait list
	
		if((*TCBItr)->getThreadID() == id){	//if found id matches requested id
			//printf("\nfindThreadByID(): found match in waitList: %d\n", (*TCBItr)->getThreadID());
			return *TCBItr;	//return TCB*
		}
	}
	
	for(std::list<TCB*>::iterator TCBItr = deadList->begin(); TCBItr != deadList->end(); TCBItr++){
		//check deadList
	
		if((*TCBItr)->getThreadID() == id){	//if found id matches requested id
		//	printf("\nfindThreadByID(): found match in deadList: %d\n", (*TCBItr)->getThreadID());
			return *TCBItr;	//return TCB*
		}
	}

	return NULL; //if code gets here, ID was not found
}

void ThreadStore::activateDeadThread(TCB* deadThread){

	for(std::list<TCB*>::iterator TCBItr = deadList->begin(); TCBItr != deadList->end(); TCBItr++){
		//find thread in check deadList
	
		if(*TCBItr == deadThread){	//if found TCB matches given TCB
			//printf("\nactivateDeadThread(): activating TID %d\n", (*TCBItr)->getThreadID());

			TVMThreadPriority priority = (*TCBItr)->getPriority();	//get priority of found thread
			(*TCBItr)->setState(VM_THREAD_STATE_READY);							//set thread state to READY
			readyLists[priority]->push_back(*TCBItr);								//add to back of ready list - make this front?
		}
	}
}

void ThreadStore::terminate(TCB* thread){

	if(thread->getThreadID() != 0x02){
		//printf("terminate(): terminating TID %d\n", currentThread->getThreadID());
		thread->setState(VM_THREAD_STATE_DEAD);	//mark thread as dead
		//alternative - delete the TCB here
		thread->releaseMutexes();
		deadList->push_back(thread);							//push to dead list
		schedule(VM_THREAD_PRIORITY_HIGH);
	}
	else{
		//printf("terminate(): Attempting to terminate idle thread\n");
	}
	return;
}

void ThreadStore::terminateCurrentThread(){
	terminate(currentThread);
}


void ThreadStore::switchToNewThread(TCB* nextThread){

	TMachineSignalState sigState;			
	MachineSuspendSignals(&sigState);	//suspend signals so we cannot be pre-empted while scheduling a new thread

	nextThread->setState(VM_THREAD_STATE_RUNNING);		//set next thread to running

	//if currentThread dead or idle thread, DON'T push it to wait list!	
	if((currentThread->getEntryFunction() != &idle) && (currentThread->getState() != VM_THREAD_STATE_DEAD)){
		currentThread->setState(VM_THREAD_STATE_WAITING);	//set current thread to waiting
	//	printf("switchToNewThread(): live thread going back to wait: TID %d\n", currentThread->getThreadID());
		waitList->push_back(currentThread);	//add current thread - one to be swapped out - to wait list
	}

	SMachineContextRef oldContext = currentThread->contextRef;	//old context is current thread's
	SMachineContextRef newContext = nextThread->contextRef;			//new context is next thread's
	//printf("swapOutCurrentThread(): switching to TID %d\n", nextThread->getThreadID());	//recall var currentThread has been changed!
	currentThread = nextThread;																	//set current active thread to be next thread
	MachineResumeSignals(&sigState);														//resume signals immediately before switching context
	MachineContextSwitch(oldContext, newContext);
	return;
}

void ThreadStore::schedule(TVMThreadPriority priority){ //priority = thread priority to schedule first. default is HIGH.

	//Checks if there are any threads ready to run in any ready lists
	//If no threads, runs idle thread until timerEvent is called at next timeslice

	bool areAllListsEmpty = (readyLists[0x01]->empty() & readyLists[0x02]->empty() & readyLists[0x03]->empty());

	if((areAllListsEmpty == true) && (currentThread->getState() == VM_THREAD_STATE_RUNNING)){
	//	printf("ThreadStore::schedule(): all lists empty and thread is running, no swap! TID %d\n", currentThread->getThreadID());
		return;
	}

	if(readyLists[priority]->empty() == false){	//attempt to swap in thread of given priority
		//printf("\nready list of prio = priority is not empty!\n");
		if(priority != 0x0){												//if list is not special idle thread queue
			readyLists[priority]->pop_front();				//pop old pointer from the list
		}
		switchToNewThread(readyLists[priority]->front());	//switch to new thread of priority priority
	}
	else{																								//no threads of given priority exist
		for(TVMThreadPriority i = 0x03; i >= 0x00; i--){	//Check highest prio and work down
			if(i == priority){										//skip priority level that was already checked
				continue;
			}
			if(readyLists[i]->empty() == false){ 		//Queue is not empty
				TCB* newThread = readyLists[i]->front();
				if(i != 0x0){												//if list is not special idle thread queue
					readyLists[i]->pop_front();				//pop old pointer from the list
				}
				//printf("schedule(): Swapping current thread with readyList: %d\n", i);
				switchToNewThread(newThread);						//schedule first thread in list
				return;
			}
		}
	}
}

void ThreadStore::timerEvent(){
	//checks wait list for any threads that have finished sleeping
	//moves threads that are done sleeping from wait list into appropriate ready list
	//ThreadStore::schedule() is then called, which swaps out the current thread for a thread from a ready list

	if(waitList->empty()){
//		printf("timerEvent(): Waitlist is empty, scheduling high\n");
	}
	
	for(std::list<TCB*>::iterator i = waitList->begin(); i != waitList->end(); i++){	//iterate over entire wait list
//		printf("timerEvent(): WaitList not empty, found TID %d\n", (*i)->getThreadID());
		if((*i)->isWaitingOnIO() == true){	//if the thread is waiting on IO, ignore it
			printf("timerEvent(): TID %d: ticks to sleep: %d: waiting on I/O\n", (*i)->getThreadID(), (*i)->getTicksToSleep());
		}
		else if((*i)->getTicksToSleep() > 0){	//if the thread still has ticks to sleep, decrement them by 1
			printf("timerEvent(): TID %d: ticks to sleep: %d\n", (*i)->getThreadID(), (*i)->getTicksToSleep());
			(*i)->decrementTicksToSleep();
		}
		else if((*i)->getTicksToSleep() == 0){	//if thread has no ticks to sleep, put it in ready list
			(*i)->setState(VM_THREAD_STATE_READY);
			if(isThreadPrioInRange(*i) == false){			//range checking
				printf("\nerorr in ThreadStore::timerEvent - thread prio out of range!\n");
			}
			else{
				TCB* foundThread = *i;
				readyLists[(*i)->getPriority()]->push_back(*i);			//insert thread into ready list
				waitList->erase(i);															//remove thread from wait list
				i--;
//				printf("timerEvent(): pushed TID: %d to readyList: %d\n and erased from waitList\n", readyLists[foundThread->getPriority()]->back()->getThreadID(), foundThread->getPriority());
			}
		}
	}

	if((readyLists[0x01]->empty() && readyLists[0x02]->empty() && readyLists[0x03]->empty())){
		if(waitList->empty() && (currentThread->getEntryFunction() == &idle)){
			//if all ready lists are empty, the wait list is empty, and the idle thread is running, call MachineTerminate() here
			printf("timerEvent(): readyLists empty, waitList empty, TID %d, ending program\n", currentThread->getThreadID());
			VMUnloadModule();
			MachineTerminate();
			return;
		}
		if(waitList->empty() && (currentThread->getEntryFunction() != &idle)){
//			printf("timerEvent(): readyLists empty, waitList empty, TID %d, calling schedule\n", currentThread->getThreadID());
			//if all ready lists are empty, the wait list is empty, but the idle thread is not running, call schedule()
			schedule(VM_THREAD_PRIORITY_HIGH);
		}
		if((waitList->empty() == false) && (currentThread->getEntryFunction() == &idle)){
//			printf("timerEvent(): readyLists empty, waitList NOT empty, TID %d, returning to idle\n", currentThread->getThreadID());
			//if all ready lists are empty, the wait list is not empty, and the idle thread is running, return; to idle
			return;
		}
		if((waitList->empty() == false) && (currentThread->getEntryFunction() != &idle)){
//			printf("timerEvent(): readyLists empty, waitList NOT empty, TID %d, calling schedule\n", currentThread->getThreadID());
			//if all ready lists are emtpy, the wait list is not empty, and the idle thread is not running, call schedule()
			schedule(VM_THREAD_PRIORITY_HIGH);
		}	
	}
	else{
		//if not all ready lists are empty, call schedule()
		schedule(VM_THREAD_PRIORITY_HIGH);
	}
	schedule(VM_THREAD_PRIORITY_HIGH);	//this is called in case of an uncaught case
	
//	MachineResumeSignals(&sigState);		//resume signals once scheduling ops are done
}

ThreadStore::ThreadStore(){
	
	for(TVMThreadPriority i = 0x0; i < 0x04; i++){
		//priority lists: 0x0 is idle, 0x1 is low, 0x2 is mid, 0x3 is high
		readyLists[i] = new std::list<TCB*>();	
	}

	waitList = new std::list<TCB*>();			//the list of threads in waiting state
	deadList = new std::list<TCB*>();			//the list of threads in dead state
	numThreads = 0;
	voidParam = 0;
	idleThreadID = 0;		//initialize the idle thread's id to zero, it will be overwritten
}

ThreadStore *ThreadStore::getInstance(){
    if(NULL == DUniqueInstance){
        DUniqueInstance = new ThreadStore();
    }
    return DUniqueInstance;
}

TCB* ThreadStore::getCurrentThread(){
	return currentThread;
}

bool ThreadStore::isThreadPrioInRange(TCB* tcb){

	//if the thread's priority is less than 1 or greater than 3
	if((tcb->getPriority() < (TVMThreadPriority)0x01) || (tcb->getPriority() > (TVMThreadPriority)0x03)){
		return false;	//false = we are out of range = bad
	}
	else
		return true;	//we are not out of range so we are in range
}

void ThreadStore::insert(TCB* tcb){	
//insert a newly created thread into the correct container
//currently, only the dead case should be called
//
	TVMThreadPriority priority = tcb->getPriority();
	
	switch(tcb->getState()){
		
		case VM_THREAD_STATE_DEAD:
			deadList->push_back(tcb);
			break;
		case VM_THREAD_STATE_WAITING:
			waitList->push_back(tcb);
		case VM_THREAD_STATE_READY:
			if(!isThreadPrioInRange(tcb)){
					printf("\nInvalid Thread Priority - ignoring thread\n");
				return;
			}//if the priority is not a valid priority, don't try to insert it!
			readyLists[priority]->push_back(tcb);			//otherwise go ahead and add it
			//lists[0x01] is low, queues[0x02] is mid, queues[0x03] is high
			break;
		default:
			break;
	}
	numThreads++;	//increment number of threads
}

void ThreadStore::createMainThread(){

	//initialize parameters for creating vmStart thread
	TVMThreadEntry vmStartEntry = (TVMThreadEntry)&dummyReturn;
	int vmStartParam = 0;
	TVMMemorySize vmStartMemsize = 0;	//random (not random, arbitrary)
	TVMThreadPriority vmStartPrio = VM_THREAD_PRIORITY_NORMAL;
	TVMThreadID vmStartTID = 0xFFFFFFFF;	//will be overwritten with 1
	TCB* vmStartThread = new TCB(vmStartEntry, (void*)&vmStartParam, vmStartMemsize, vmStartPrio, (TVMThreadIDRef)&vmStartTID);
	insert(vmStartThread);
	currentThread = vmStartThread;
}

void ThreadStore::createIdleThread(){

	if(readyLists[0x0]->empty() == true){	//if no idle thread exists
		TVMMemorySize m = 0x100000;
		TVMThreadPriority p = 0x00;
		//idle thread has its own special list
		readyLists[0x0]->push_back(new TCB((TVMThreadEntry)&idle, (void *)(voidParam), m, p, &idleThreadID));
	}
	else
		printf("\nerror - idle thread already exists!\n");
	numThreads++;
}

int ThreadStore::getNumThreads(){
	return numThreads;
}

ThreadStore::~ThreadStore(){
/*
	for(TVMThreadPriority i = 0x00; i < 0x04; i++){	//iterate over all ready lists
		while(readyLists[i]->empty() == false){				//while the list is not empty
			TCB* element = readyLists[i]->front();			//get the head of the list
			readyLists[i]->pop_front();									//pop off head of list
			delete element;															//delete head of list
		}
		delete readyLists[i];													//delete empty container
//		printf("successfully deleted readyList %d\n", i);
	}

	while(waitList->empty() == false){
		TCB* element = waitList->front();	//access head of list
		waitList->pop_front();						//erase head of list
		delete element;										//delete head element
	}
	delete waitList;

//	printf("successfully deleted waitList\n");

	while(deadList->empty() == false){
		TCB* element = deadList->front();	//access head of list
		deadList->pop_front();						//erase head of list
		printf("successfully deleted deadList elem\n");
		delete element;										//delete head element
	}
	delete deadList;
	delete currentThread;*/
}


//=================================================================



//=== Utilities ==================================================

void safeEntry(void *functionAndParams){

	ThreadStore *tStore = ThreadStore::getInstance();
	printf("safeEntry(): entering thread TID %d\n", tStore->getCurrentThread()->getThreadID());
	TVMThreadEntry entry 	= (TVMThreadEntry)(((int**)functionAndParams)[0]);
	void *params 					= (void *)(((int**)functionAndParams)[1]);
	entry(params);				//this starts the thread
	printf("safeEntry(): returned, terminating TID %d\n", tStore->getCurrentThread()->getThreadID());
	tStore->terminateCurrentThread();
	tStore->schedule(VM_THREAD_PRIORITY_HIGH);
	//this is called if user does not terminate on their own
	printf("safeEntry(): returning, should not get here\n");
}

void idle(void* parm1){	//infinite loop for the idle thread
	while(1);
}

void timerInterrupt(void *calldata){
	//takes a TVMThreadPriorityRef
	
	ThreadStore::getInstance()->timerEvent();	//scheduling logic is in the threadstore because that's where all the TCBs are
//	printf("timerInterrupt(): back in the interrupt, returning\n");

	return;
}

void machineFileSeekCallback(void* calledParam, int result){

	ThreadStore* tStore = ThreadStore::getInstance();
	printf("machineFileSeekCallback(): TID %d\n", tStore->getCurrentThread()->getThreadID());
	((TCB*)calledParam)->setFileSeekResult(result);	//return file descriptor to original location by reference. will be -1 if an error occurs
	((TCB*)calledParam)->setIsWaitingOnIO(false);
	return;
}

void machineFileReadCallback(void* calledParam, int result){

	ThreadStore* tStore = ThreadStore::getInstance();
	printf("machineFileReadCallback(): TID %d\n", tStore->getCurrentThread()->getThreadID());
	((TCB*)calledParam)->setFileReadResult(result);	//return file descriptor to original location by reference. will be -1 if an error occurs
	((TCB*)calledParam)->setIsWaitingOnIO(false);
	return;
}

void machineFileCloseCallback(void* calledParam, int result){
	
	ThreadStore* tStore = ThreadStore::getInstance();
	printf("machineFileCloseCallback(): TID %d\n", tStore->getCurrentThread()->getThreadID());
	((TCB*)calledParam)->setFileCloseResult(result);	//return file descriptor to original location by reference. will be -1 if an error occurs
	((TCB*)calledParam)->setIsWaitingOnIO(false);
	return;	
}

void machineFileOpenCallback(void* calledParam, int returnedFileDescriptor){

	ThreadStore* tStore = ThreadStore::getInstance();
	printf("machineFileOpenCallback(): TID %d\n", tStore->getCurrentThread()->getThreadID());
	((TCB*)calledParam)->setFileOpenResult(returnedFileDescriptor);	//return file descriptor to original location by reference. will be -1 if an error occurs
	((TCB*)calledParam)->setIsWaitingOnIO(false);
	return;
}

void machineFileWriteCallback(void *data, int result){
	//here, we can be in ANY thread!
	ThreadStore* tStore = ThreadStore::getInstance();
	printf("machineFileWriteCallback(): TID %d\n", tStore->getCurrentThread()->getThreadID());
	((TCB*)data)->setFileWriteResult(result);	//set IO thread's result
	((TCB*)data)->setIsWaitingOnIO(false);										//set thread to wake up at next timeslice; however, no guarantee it will run right away!
	//here, thread returns out of callback and resumes whatever it was doing
	return;
}

void dummyReturn(void* p){
	return;
}
//==================================================================