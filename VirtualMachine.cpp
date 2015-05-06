#include "VirtualMachine.h"
#include "UtilityFunctions.h"
#include "Machine.h"
#include <stdio.h>

<<<<<<< HEAD
TVMThreadPriority defaultSchedulerPriority = VM_THREAD_PRIORITY_HIGH;
TCB* vmStartThread;
=======
//should remove this one after the project is done
#include <iostream>
#include <cstdio>
>>>>>>> f7388c19a0e6d60f159c076f43d0e5d1cce5ec8f

extern "C" {


<<<<<<< HEAD
//=== Mutex ==================================================================


TVMStatus VMMutexCreate(TVMMutexIDRef mutexref){
	TMachineSignalState sigState;	
	MachineSuspendSignals(&sigState);
	ThreadStore* tStore = ThreadStore::getInstance();
	
	if(mutexref == NULL){
		MachineResumeSignals(&sigState);
  	return VM_STATUS_ERROR_INVALID_PARAMETER;
	}

	Mutex* mutex = new Mutex();
	tStore->insert(mutex);	//insert is overloaded to work with mutexes and TCB*s
	*mutexref = mutex->getID();
	
	MachineResumeSignals(&sigState);
	return VM_STATUS_SUCCESS;
}

TVMStatus VMMutexDelete(TVMMutexID mutexID){
	TMachineSignalState sigState;			
	MachineSuspendSignals(&sigState);
	ThreadStore* tStore = ThreadStore::getInstance();
	Mutex* mutex = tStore->findMutexByID(mutexID);	

	if(mutex == NULL){	//if the mutex does not exist
		MachineResumeSignals(&sigState);
  	return VM_STATUS_ERROR_INVALID_ID;
	}

	if(mutex->isDeleted() == true){			//mutex is marked for deletion
		MachineResumeSignals(&sigState);
  	return VM_STATUS_ERROR_INVALID_ID;
	}

	if(mutex->getOwner() != NULL){					//if the mutex is held, do not delete it
		MachineResumeSignals(&sigState);
  	return VM_STATUS_ERROR_INVALID_STATE;
	}

	//else, mark for deletion
	mutex->deleteFromVM();
	MachineResumeSignals(&sigState);
	return VM_STATUS_SUCCESS;
}

TVMStatus VMMutexQuery(TVMMutexID mutexID, TVMThreadIDRef ownerref){
	TMachineSignalState sigState;			
	MachineSuspendSignals(&sigState);
	ThreadStore* tStore = ThreadStore::getInstance();
	Mutex* mutex = tStore->findMutexByID(mutexID);
	
	if(ownerref == NULL){
		MachineResumeSignals(&sigState);
  	return VM_STATUS_ERROR_INVALID_PARAMETER;
=======
extern "C"{
	
	using namespace std;

	//Definitions
	typedef void (*TVMMainEntry)(int, char*[]); //These are definitions for VirtualMachineUtils.c
	TVMMainEntry VMLoadModule(const char* module); //They are implemented using Professor Nitta's provided source code in VirtualMachineUtils.c
	void VMUnloadModule(void);
	TVMStatus VMFilePrint(int filedescriptor, const char *format, ...);
	//End Definitions

	void dummyCallback(void *data, int result){
		
		int* mydata = (int*)data;
		*mydata = result;
		
		printf("\nin the callback");
		printf("\nmydata is: %d", *(int*)data);
		return;
	}
	
	//Global Variables
//	ThreadStore* threadStore;
	//End Global Variables
	
	//Vincent's File Functions
	/*
	TVMStatus VMFileOpen(const char *filename, int flags, int mode, int *filedescriptor){
    FILE* file;
    struct stat *perm;
    filedescriptor = stat(filename, perm);
    if (filename || filedescriptor == NULL)
        return VM_STATUS_ERROR_INVALID_PARAMETER;
    else if (!(file = fopen(filename, "r")))
        return VM_STATUS_FAILURE;
    else if ((perm->mode & flags) != 1)
        return VM_STATUS_FAILURE;
    else
        return VM_STATUS_SUCCESS;
	}
	
	TVMStatus VMFileClose(int filedescriptor){
    fclose(filedescriptor);
	}
	
	/*Still need to implement this one
	TVMStatus VMFileRead(int filedescriptor, void *data, int *length){

	}*/
	
	TVMStatus VMFileWrite(int fileDescriptor, void *data, int *length){
    
		int callbackdata = -1337;
		TMachineFileCallback callback = &dummyCallback;
		
		if ((data == NULL) || (length == NULL))
        return VM_STATUS_ERROR_INVALID_PARAMETER;
			
		MachineFileWrite(fileDescriptor, data, *length, callback, (void*)(&callbackdata));
		
		while(callbackdata != -1337);
		
		printf("\nResult is: %d", callbackdata);
		
//		cout << "in file write!" << endl;
		
	
//    if (write(fileDescriptor, data, (size_t)(*length)) == -1)
//        return VM_STATUS_FAILURE;
 //   else
        return VM_STATUS_SUCCESS;
	}

	/*Still need to implement this one
	TVMStatus VMFileSeek(int filedescriptor, int offset, int whence, int *newoffset){
    
	}
	*/
/*
	TVMStatus VMFilePrint(int filedescriptor, const char *format, ...){
    va_list ParamList;
    char *OutputBuffer;
    char SmallBuffer[SMALL_BUFFER_SIZE];
    int SizeRequired;
    TVMStatus ReturnValue;
    
    va_start(ParamList, format);
    OutputBuffer = SmallBuffer;
    
    SizeRequired = vsnprintf(OutputBuffer, SMALL_BUFFER_SIZE, format, ParamList);
    if(SizeRequired < SMALL_BUFFER_SIZE){
        ReturnValue = VMFileWrite(filedescriptor, OutputBuffer, &SizeRequired);
        return ReturnValue;
    }
    OutputBuffer = (char *)malloc(sizeof(char) *(SizeRequired + 1));
    SizeRequired = vsnprintf(OutputBuffer, SizeRequired, format, ParamList);
    ReturnValue = VMFileWrite(filedescriptor, OutputBuffer, &SizeRequired);
    free(OutputBuffer);
    return ReturnValue;
>>>>>>> f7388c19a0e6d60f159c076f43d0e5d1cce5ec8f
	}
	//End Vincent's File Functions
	*/

<<<<<<< HEAD
	if(mutex == NULL){
		MachineResumeSignals(&sigState);
  	return VM_STATUS_ERROR_INVALID_ID;
	}
=======
	//Dan's Threading Functions
>>>>>>> f7388c19a0e6d60f159c076f43d0e5d1cce5ec8f
	
	if(mutex->isDeleted() == true){
		MachineResumeSignals(&sigState);
  	return VM_STATUS_ERROR_INVALID_ID;
	}

	if(mutex->getOwner() == NULL){		//if the mutex is UNLOCKED, error
		MachineResumeSignals(&sigState);
  	return VM_THREAD_ID_INVALID;
	}

	//if the mutex exists, is not deleted, is not free, and if we have somewhere to put the result...	
	*ownerref = mutex->getOwner()->getThreadID();	//return the owner
	MachineResumeSignals(&sigState);
	return VM_STATUS_SUCCESS;
}

TVMStatus VMMutexAcquire(TVMMutexID mutexID, TVMTick timeout){
	TMachineSignalState sigState;
	MachineSuspendSignals(&sigState);
	ThreadStore* tStore = ThreadStore::getInstance();
	TCB* currentThread = tStore->getCurrentThread();
	Mutex* mutex = tStore->findMutexByID(mutexID);
	
	if(mutex == NULL){	//if mutex doesnt exist, error
		MachineResumeSignals(&sigState);
  	return VM_STATUS_ERROR_INVALID_ID;
	}

	if(mutex->isDeleted() == true){	//if mutex deleted, error
		MachineResumeSignals(&sigState);
  	return VM_STATUS_ERROR_INVALID_ID;
	}

	mutex->lock(currentThread, timeout);	//may block here until timeout expires

	if(mutex->getOwner() != currentThread){
		MachineResumeSignals(&sigState);
		return VM_STATUS_FAILURE;
	}

	MachineResumeSignals(&sigState);
	return VM_STATUS_SUCCESS;
}

TVMStatus VMMutexRelease(TVMMutexID mutexID){
	TMachineSignalState sigState;
	MachineSuspendSignals(&sigState);
	ThreadStore* tStore = ThreadStore::getInstance();
	TCB* currentThread = tStore->getCurrentThread();
	Mutex* mutex = tStore->findMutexByID(mutexID);
	
	if(mutex == NULL){
		MachineResumeSignals(&sigState);
  	return VM_STATUS_ERROR_INVALID_ID;
	}
<<<<<<< HEAD

	if(mutex->getOwner() != currentThread){
		return VM_STATUS_ERROR_INVALID_STATE;
=======
		
	TVMStatus VMThreadCreate(TVMThreadEntry entry, void *param, TVMMemorySize memsize, TVMThreadPriority prio, TVMThreadIDRef tid){
		//creates a thread in the virtual machine, thread begins in the dead state VM_THREAD_STATE_DEAD
		//entry specifies the function of the thread
		//param specifies the parameter that is passed to the function
		//memsize specifies the size of the thread's stack
		//prio specifies the thread's priority
		//tid specifies at which location to put the thread identifier
		
		//also creates a thread control block and pushes it into the appropriate queue in ThreadStore
		
		
		cout << "Entered VMThreadCreate" <<endl;
		
		//if the thread was successfully created,
		return VM_STATUS_SUCCESS;
		//if entry or tid is NULL, return
		//return VM_STATUS_ERROR_INVALID_PARAMETER;
>>>>>>> f7388c19a0e6d60f159c076f43d0e5d1cce5ec8f
	}

	mutex->release();
	MachineResumeSignals(&sigState);
	return VM_STATUS_SUCCESS;
}

//============================================================================

//=== Files and IO ==================================================================

TVMStatus VMFileSeek(int filedescriptor, int offset, int whence, int *newoffset){
	TMachineSignalState sigState;			
	MachineSuspendSignals(&sigState);
	ThreadStore* tStore = ThreadStore::getInstance();
	TCB* currentThread = tStore->getCurrentThread();
	
<<<<<<< HEAD
	if(newoffset == NULL){
		MachineResumeSignals(&sigState);
  	return VM_STATUS_ERROR_INVALID_PARAMETER;
	}
	
	MachineFileSeek(filedescriptor, offset, whence, &machineFileIOCallback, (void*)currentThread);
	tStore->waitCurrentThreadOnIO();//resume execution here after IO completes
	*newoffset = currentThread->getFileIOResult();	
	
	if(currentThread->getFileIOResult() < 0){	//if call to open() failed
		MachineResumeSignals(&sigState);
		return VM_STATUS_FAILURE;		//return failure state
	}
	
	MachineResumeSignals(&sigState);
	return VM_STATUS_SUCCESS;
}

TVMStatus VMFileRead(int filedescriptor, void *data, int *length){
	TMachineSignalState sigState;			
	MachineSuspendSignals(&sigState);	//suspend signals so we cannot be pre-empted while scheduling a new thread
	ThreadStore* tStore = ThreadStore::getInstance();
	TCB* currentThread = tStore->getCurrentThread();
  
	if ((data == NULL) || (length == NULL)){
		MachineResumeSignals (&sigState);
  	return VM_STATUS_ERROR_INVALID_PARAMETER;
	}
	
	MachineFileRead(filedescriptor, data, *length, &machineFileIOCallback, (void*)currentThread);
	tStore->waitCurrentThreadOnIO();								//resume execution here after IO completes
	*length = currentThread->getFileIOResult();
	
	if(currentThread->getFileIOResult() < 0){			//if call to open() failed
		MachineResumeSignals (&sigState);
		return VM_STATUS_FAILURE;											//return failure state
	}

	MachineResumeSignals (&sigState);
	return VM_STATUS_SUCCESS;
}

TVMStatus VMFileClose(int filedescriptor){
	TMachineSignalState sigState;			
	MachineSuspendSignals(&sigState);	//suspend signals so we cannot be pre-empted while scheduling a new thread
	ThreadStore* tStore = ThreadStore::getInstance();
	TCB* currentThread = tStore->getCurrentThread();

	MachineFileClose(filedescriptor, machineFileIOCallback, (void*)currentThread);
	tStore->waitCurrentThreadOnIO();//resume execution here after IO completes
	
	if(currentThread->getFileIOResult() < 0){
		MachineResumeSignals (&sigState);
    return VM_STATUS_FAILURE;
	}
	
	MachineResumeSignals (&sigState);
	return VM_STATUS_SUCCESS;
}

TVMStatus VMFileOpen(const char *filename, int flags, int mode, int *filedescriptor){
	TMachineSignalState sigState;			
	MachineSuspendSignals(&sigState);	//suspend signals so we cannot be pre-empted while scheduling a new thread
	ThreadStore* tStore = ThreadStore::getInstance();
	TCB* currentThread = tStore->getCurrentThread();
  
	if ((filename == NULL) || (filedescriptor == NULL)){
		MachineResumeSignals (&sigState);
  	return VM_STATUS_ERROR_INVALID_PARAMETER;
	}
	
	MachineFileOpen(filename, flags, mode, &machineFileIOCallback, (void*)currentThread);
	tStore->waitCurrentThreadOnIO();//resume execution here after IO completes
	*filedescriptor = currentThread->getFileIOResult();	//return result by reference
	
	if(currentThread->getFileIOResult() < 0){	//if call to open() failed
		MachineResumeSignals (&sigState);
		return VM_STATUS_FAILURE;		//return failure state
	}
	
	MachineResumeSignals (&sigState);
	return VM_STATUS_SUCCESS;
}

TVMStatus VMFileWrite(int fileDescriptor, void* data, int *length){
	TMachineSignalState sigState;			
	MachineSuspendSignals(&sigState);	//suspend signals so we cannot be pre-empted while scheduling a new thread
	ThreadStore* tStore = ThreadStore::getInstance();
	TCB* currentThread = tStore->getCurrentThread();
	
	if((data == NULL) || (length == NULL)){
		MachineResumeSignals (&sigState);
		return VM_STATUS_ERROR_INVALID_PARAMETER;
	}
	
		MachineFileWrite(fileDescriptor, data, *length, &machineFileIOCallback, (void*)currentThread);
	//This works because idle() has a call to MachineEnableSignals in it. Otherwise would need MachineEnableSignals() here	
	tStore->waitCurrentThreadOnIO();	//switch to a new thread here, don't do a wait loop
	*length = currentThread->getFileIOResult();

	if(currentThread->getFileIOResult() < 0){
		MachineResumeSignals (&sigState);
		return VM_STATUS_FAILURE;
	}
	MachineResumeSignals (&sigState);
	return VM_STATUS_SUCCESS;
}

//==================================================================================



//=== Threads ======================================================================
//

TVMStatus VMThreadDelete(TVMThreadID thread){
	TMachineSignalState sigState;			
	MachineSuspendSignals(&sigState);	//suspend signals so we cannot be pre-empted while scheduling a new thread
	ThreadStore* tStore = ThreadStore::getInstance();
	TCB* currentThread = tStore->findThreadByID(thread);	//returns NULL on not finding thread

	if(currentThread == NULL){								//if thread was not found, error
		MachineResumeSignals (&sigState);
		return VM_STATUS_ERROR_INVALID_ID;
	}
	
	if(currentThread->getState() != VM_THREAD_STATE_DEAD){
		MachineResumeSignals (&sigState);
		return VM_STATUS_ERROR_INVALID_STATE;
	}

	tStore->deleteDeadThread(currentThread);
	MachineResumeSignals(&sigState);
	return VM_STATUS_SUCCESS;
}

TVMStatus VMThreadTerminate(TVMThreadID thread){
	TMachineSignalState sigState;			
	MachineSuspendSignals(&sigState);
	ThreadStore* tStore = ThreadStore::getInstance();
	TCB* threadToTerminate = tStore->findThreadByID(thread);	//returns NULL on not finding thread


	if(threadToTerminate == NULL){								//if thread was not found, error
		MachineResumeSignals(&sigState);
		return VM_STATUS_ERROR_INVALID_ID;
	}
	
	if(threadToTerminate->getState() == VM_THREAD_STATE_DEAD){
		MachineResumeSignals(&sigState);
		return VM_STATUS_ERROR_INVALID_STATE;
	}
	else{
		tStore->terminate(threadToTerminate);
		MachineResumeSignals(&sigState);
		return VM_STATUS_SUCCESS;
	}
}

TVMStatus VMThreadState(TVMThreadID thread, TVMThreadStateRef stateRef){
	TMachineSignalState sigState;			
	MachineSuspendSignals(&sigState);

	if(stateRef == NULL){											//if stateRef is null, error
		MachineResumeSignals(&sigState);
		return VM_STATUS_ERROR_INVALID_PARAMETER;
	}

	ThreadStore* tStore = ThreadStore::getInstance();
	TCB* currentThread = tStore->findThreadByID(thread);	//returns NULL on not finding thread

	if(currentThread == NULL){								//if thread was not found, error
		MachineResumeSignals(&sigState);
		return VM_STATUS_ERROR_INVALID_ID;
	}
	*stateRef = currentThread->getState();		//otherwise, success
	MachineResumeSignals(&sigState);
	return VM_STATUS_SUCCESS;
}

TVMStatus VMThreadActivate(TVMThreadID thread){
	TMachineSignalState sigState;			
	MachineSuspendSignals(&sigState);
	ThreadStore* tStore = ThreadStore::getInstance();
	TCB* threadToActivate = tStore->findThreadByID(thread);

	if(threadToActivate == NULL){					//thread ID does not exist
		MachineResumeSignals(&sigState);
		return VM_STATUS_ERROR_INVALID_ID;	//error status
	}
	
	if(threadToActivate->getState() != VM_THREAD_STATE_DEAD){	//thread exists but in wrong state
		MachineResumeSignals(&sigState);
		return VM_STATUS_ERROR_INVALID_STATE;										//error status
	}
	tStore->activateDeadThread(threadToActivate);	//activate given thread from dead list	
	MachineResumeSignals(&sigState);
	return VM_STATUS_SUCCESS;							//only other alternative is success
}

TVMStatus VMThreadID(TVMThreadIDRef threadRef){
//returns thread id of currently running thread into location specified by threadref
	TMachineSignalState sigState;			
	MachineSuspendSignals(&sigState);
	
	if(threadRef == NULL){
			MachineResumeSignals(&sigState);
			return VM_STATUS_ERROR_INVALID_PARAMETER;
		}

		//need to SET the active thread before calling this!
		TCB* currentThread = ThreadStore::getInstance()->getCurrentThread();
		TVMThreadID currentThreadID = currentThread->getThreadID();
		*threadRef = currentThreadID;
		MachineResumeSignals(&sigState);
		return VM_STATUS_SUCCESS;			//thread ID is successfully retrieved, stored in the location specified by threadref,
	}

TVMStatus VMThreadCreate(TVMThreadEntry entry, void *param, TVMMemorySize memsize, TVMThreadPriority prio, TVMThreadIDRef tid){

		TMachineSignalState sigState;
		MachineSuspendSignals(&sigState);
		
		if((entry == NULL) || (tid == NULL)){
			MachineResumeSignals(&sigState);
			return VM_STATUS_ERROR_INVALID_PARAMETER;
		}

		ThreadStore *tStore = ThreadStore::getInstance(); 					//get the threadstore
		TCB* tcb = new TCB(entry, param, memsize, prio, tid);				//create TCB 
		
		tStore->insert(tcb);																				//pushes TCB into appropriate queue in ThreadStore
		MachineResumeSignals(&sigState);
		return VM_STATUS_SUCCESS;
	}

TVMStatus VMThreadSleep(TVMTick tick){
//Puts running thread to sleep for tick ticks/quantums
	TMachineSignalState sigState;			
	MachineSuspendSignals(&sigState);
	ThreadStore *tStore = ThreadStore::getInstance();

	if(tick == VM_TIMEOUT_INFINITE){
		MachineResumeSignals(&sigState);
		return VM_STATUS_ERROR_INVALID_PARAMETER;
	}

	if(tick == VM_TIMEOUT_IMMEDIATE){
		tStore->sleepCurrentThread(0);	//sets the current thread to sleep and schedules a new one in its place
	}
	else{
		tStore->sleepCurrentThread(tick);
	}//both cases sleep the running process and immediately call the scheduler to swap a new process in
	
	MachineResumeSignals(&sigState);
	return VM_STATUS_SUCCESS;
}

TVMStatus VMStart(int tickms, int machinetickms, int argc, char* argv[]){

	//tickms is the time in milliseconds for the alarm, this will be the quantum. 
	//machineticksms is the tick time of the machine, how long it will sleep in between actions.
	
	const char* module = (const char*) argv[0];	//get the module from the command-line args
	MachineInitialize(machinetickms);	

	ThreadStore* tStore = ThreadStore::getInstance();
	tStore->createMainThread();	//mainThread is ID 1
	tStore->createIdleThread();	//idleThread is ID 2

	//request alarm every tickms USECONDS. alarm callback is schedule(high priority)
	MachineEnableSignals();	//enable signals	
	MachineRequestAlarm(1000*tickms, (TMachineAlarmCallback)&timerInterrupt, (void*)&defaultSchedulerPriority);	
	TVMMainEntry VMMain = VMLoadModule(module);							//load the module, save the VMMain function reference
	
	if(VMMain != NULL){													//if VMMain is a valid reference,
		VMMain(argc, argv);												//run the loaded module,
		VMUnloadModule();													//then unload the module
		delete tStore;														//delete unnecessary container
		MachineTerminate();
		return VM_STATUS_SUCCESS;
	}
	else{																				//if VMMain is null, the module failed to load
		VMUnloadModule();													//unload the module and return error status
		MachineTerminate();
		return VM_STATUS_FAILURE;
	}
}
}
=======
	TVMTick alarmCounter;
	TVMStatus VMThreadSleep(TVMTick tick){		//TVMTick is just an unsigned int
	
		alarmCounter = tick;
	
	//Puts the currently running thread to sleep for tick ticks
	//if tick is the same as VM_TIMEOUT_IMMEDIATE, 
		//the current process immediately goes to sleep
		//the next ready process of EQUAL PRIORITY (NOT greater OR lesser) wakes up and begins executing,
		//the process that just woke up and started executing gets the remainder of the processing time that the previous thread just gave up
		
		//what I actually want to call here is change the thread's state to the WAIT state
		
		//need to set the global alarm counter to TICK ticks
		//keep the alarm counter in the threadstore?
		
		//create a new Scheduler class to hold the alarm counter
		
		SMachineContext currentMachineContext;			
		//MachineContextSwitch(currentThread->getCurrentMachineContext, threadStore->nextThread->nextContext);
		
		cout << "going to sleep" << endl;	//debug
		//set the callback function pointer here
		//		MachineRequestAlarm((useconds_t) tick, callback, &tick);		//request an alarm here, for useconds_t after calling the function
//		for(mytick; mytick > 0;){
//			cout << "This should keep appearing until the alarm goes off" << endl;
//		}//do nothing while the time to sleep is greater than zero
		cout << "woke back up" << endl;	//debug
		
		//As per nitta's instructions on Piazza:  set up an alarm using MachineRequestAlarm to sleep and wake the thread
		
	//if the currently running thread successfully goes to sleep,
			return VM_STATUS_SUCCESS;
	//if the sleep duration tick is the same as VM_TIMEOUT_INFINITE,
	//		return VM_STATUS_ERROR_INVALID_PARAMETER;
	}
	//End Dan's Threading Functions

	
	//Utility Functions
	void decrementAlarmCounter(void* data){
		//resets the sleep time to zero
		//threadStore->setAlarmCounter(threadStore->getAlarmCounter() - 1);
		
		*((TVMTick*)data) -= 1;
		printf("\ndecremented alarm counter");
		return;
	}
	//End Utility Functions
	
	TVMStatus VMStart(int tickms, int machinetickms, int argc, char* argv[]){
		//tickms is the time in milliseconds for the alarm, this will be the quantum. 
		//machineticksms is the tick time of the machine, how long it will sleep in between actions.

		//initialize the threadstore
		//to do: make it actually a singleton	
		//threadStore = new ThreadStore();
		
		MachineInitialize(machinetickms);	//Initialize the machine
		TMachineAlarmCallback callback = &decrementAlarmCounter;		//useconds_t is an unsigned integer
		MachineRequestAlarm(tickms*1000, callback, &alarmCounter);				//request an alarm here, for ticms MICROSECONDS after calling the function 
		MachineEnableSignals();	//enable signals	
		TVMMainEntry VMMain;	//declare the variable to hold the function pointer to the loaded module's main() function	
		const char* module = (const char*) argv[0];	//get the module from the command-line args
		VMMain = VMLoadModule(module);							//load the module using VMLoad, save the VMMain function reference
		
		if(VMMain != NULL){													//if VMMain is a valid reference,
			VMMain(argc, argv);												//run the loaded module,
			VMUnloadModule();													//then unload the module
			printf("\nfinished vmfilewrite");
			return VM_STATUS_SUCCESS;
		}
		else{																				//if VMMain is null, then the module failed to load
			VMUnloadModule();													//unload the module and return error status
//			cout << "VMMain failed" << endl;
			return VM_STATUS_FAILURE;			
		}
	}
}
>>>>>>> f7388c19a0e6d60f159c076f43d0e5d1cce5ec8f
