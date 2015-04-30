#include "VirtualMachine.h"
#include "UtilityFunctions.h"
#include "Machine.h"
#include <stdio.h>

TVMThreadPriority defaultSchedulerPriority = VM_THREAD_PRIORITY_HIGH;
TCB* vmStartThread;

extern "C" {


//=== Files and IO ==================================================================

TVMStatus VMFileSeek(int filedescriptor, int offset, int whence, int *newoffset){

	ThreadStore* tStore = ThreadStore::getInstance();
	TCB* currentThread = tStore->getCurrentThread();
	TMachineFileCallback callBack = &machineFileSeekCallback;
	
	MachineFileSeek(filedescriptor, offset, whence, callBack, (void*)currentThread);
	MachineEnableSignals();
	tStore->waitCurrentThreadOnIO();//resume execution here after IO completes
	
	if(currentThread->getFileSeekResult() < 0){	//if call to open() failed
		return VM_STATUS_FAILURE;		//return failure state
	}
	
	return VM_STATUS_SUCCESS;

}

TVMStatus VMFileRead(int filedescriptor, void *data, int *length){

	ThreadStore* tStore = ThreadStore::getInstance();
	TCB* currentThread = tStore->getCurrentThread();
	TMachineFileCallback callBack = &machineFileReadCallback;
  
	if ((data == NULL) || (length == NULL)){
  	return VM_STATUS_ERROR_INVALID_PARAMETER;
	}
	
	MachineFileRead(filedescriptor, data, *length, callBack, (void*)currentThread);
	MachineEnableSignals();
	tStore->waitCurrentThreadOnIO();//resume execution here after IO completes
	
	if(currentThread->getFileReadResult() < 0){	//if call to open() failed
		return VM_STATUS_FAILURE;		//return failure state
	}
	
	return VM_STATUS_SUCCESS;
}

TVMStatus VMFileClose(int filedescriptor){

	ThreadStore* tStore = ThreadStore::getInstance();
	TCB* currentThread = tStore->getCurrentThread();
	TMachineFileCallback callBack = &machineFileCloseCallback;

	MachineFileClose(filedescriptor, callBack, (void*)currentThread);
	tStore->waitCurrentThreadOnIO();//resume execution here after IO completes
	MachineEnableSignals();
	
  if (currentThread->getFileCloseResult() < 0)
    return VM_STATUS_FAILURE;
	else
		return VM_STATUS_SUCCESS;
}

TVMStatus VMFileOpen(const char *filename, int flags, int mode, int *filedescriptor){
	
	ThreadStore* tStore = ThreadStore::getInstance();
	TCB* currentThread = tStore->getCurrentThread();
	TMachineFileCallback callBack = &machineFileOpenCallback;
  
	if ((filename == NULL) || (filedescriptor == NULL)){
  	return VM_STATUS_ERROR_INVALID_PARAMETER;
	}
	
	MachineFileOpen(filename, flags, mode, callBack, (void*)currentThread);
	MachineEnableSignals();
	tStore->waitCurrentThreadOnIO();//resume execution here after IO completes
	*filedescriptor = currentThread->getFileOpenResult();	//return result by reference
	
	if(currentThread->getFileOpenResult() < 0){	//if call to open() failed
		return VM_STATUS_FAILURE;		//return failure state
	}
	
	return VM_STATUS_SUCCESS;
}

TVMStatus VMFileWrite(int fileDescriptor, void* data, int *length){

	ThreadStore* tStore = ThreadStore::getInstance();
	TCB* currentThread = tStore->getCurrentThread();
	TMachineFileCallback callBack = &machineFileWriteCallback;
	
	if((data == NULL) || (length == NULL)){
		printf("VMFileWrite(): error length and data are null\n");
		return VM_STATUS_ERROR_INVALID_PARAMETER;
	}
	
	MachineFileWrite(fileDescriptor, data, *length, callBack, (void*)currentThread);
	MachineEnableSignals();						//enable signals. Do this after any file i/o	
	tStore->waitCurrentThreadOnIO();	//switch to a new thread here, don't do a wait loop
	
	if(currentThread->getFileWriteResult() < 0){
		//indicates an error has occurred with MachineFileWrite
		return VM_STATUS_FAILURE;
	}
//	printf("\nVMFileWrite(): complete\n");
	return VM_STATUS_SUCCESS;
}

//==================================================================================



//=== Threads ======================================================================
//

TVMStatus VMThreadDelete(TVMThreadID thread){

	TCB* currentThread = NULL;
	ThreadStore* tStore = ThreadStore::getInstance();
	currentThread = tStore->findThreadByID(thread);	//returns NULL on not finding thread

	if(currentThread == NULL){								//if thread was not found, error
		return VM_STATUS_ERROR_INVALID_ID;
	}
	
	if(currentThread->getState() != VM_THREAD_STATE_DEAD){
		return VM_STATUS_ERROR_INVALID_STATE;
	}

	tStore->deleteDeadThread(currentThread);
	return VM_STATUS_SUCCESS;

}

TVMStatus VMThreadTerminate(TVMThreadID thread){

	TCB* currentThread = NULL;
	ThreadStore* tStore = ThreadStore::getInstance();
	currentThread = tStore->findThreadByID(thread);	//returns NULL on not finding thread

	if(currentThread == NULL){								//if thread was not found, error
		return VM_STATUS_ERROR_INVALID_ID;
	}
	
	if(currentThread->getState() == VM_THREAD_STATE_DEAD){
		return VM_STATUS_ERROR_INVALID_STATE;
	}
	else{
		tStore->terminate(currentThread);
	}
	return VM_STATUS_SUCCESS;
}

TVMStatus VMThreadState(TVMThreadID thread, TVMThreadStateRef stateRef){

	if(stateRef == NULL){											//if stateRef is null, error
		return VM_STATUS_ERROR_INVALID_PARAMETER;
	}

	TCB* currentThread = NULL;
	ThreadStore* tStore = ThreadStore::getInstance();
	currentThread = tStore->findThreadByID(thread);	//returns NULL on not finding thread

	if(currentThread == NULL){								//if thread was not found, error
		return VM_STATUS_ERROR_INVALID_ID;
	}
	
	*stateRef = currentThread->getState();		//otherwise, success
	return VM_STATUS_SUCCESS;
}

TVMStatus VMThreadActivate(TVMThreadID thread){
	//activates dead thread by ID, which then enters ready state
	
	TCB* threadToActivate = ThreadStore::getInstance()->findThreadByID(thread);

	if(threadToActivate == NULL)					//thread ID does not exist
		return VM_STATUS_ERROR_INVALID_ID;	//error status
	
	if(threadToActivate->getState() != VM_THREAD_STATE_DEAD)	//thread exists but in wrong state
		return VM_STATUS_ERROR_INVALID_STATE;										//error status

	printf("\nVMThreadActivate(): activating TID %d", threadToActivate->getThreadID());
	ThreadStore::getInstance()->activateDeadThread(threadToActivate);	//activate given thread from dead list	

	return VM_STATUS_SUCCESS;							//only other alternative is success
}

TVMStatus VMThreadID(TVMThreadIDRef threadRef){

		//returns thread id of currently running thread into location specified by threadref
		//Sets thread id BY REFERENCE, threadref is ADDRESS.
	
		if(threadRef == NULL){
			return VM_STATUS_ERROR_INVALID_PARAMETER;
		}

		//need to SET the active thread before calling this!
		TCB* currentThread = ThreadStore::getInstance()->getCurrentThread();
		TVMThreadID currentThreadID = currentThread->getThreadID();
		*threadRef = currentThreadID;
		return VM_STATUS_SUCCESS;			//thread ID is successfully retrieved, stored in the location specified by threadref,
	}

TVMStatus VMThreadCreate(TVMThreadEntry entry, void *param, TVMMemorySize memsize, TVMThreadPriority prio, TVMThreadIDRef tid){
		
		//creates thread beginning in VM_THREAD_STATE_DEAD
		//entry: entry function of thread, param: parameter for entry function
		//memsize: size of thread's stack, prio: thread's priority, tid: address of variable that holds thread id

		TMachineSignalState signalState;
		MachineSuspendSignals((TMachineSignalStateRef)&signalState);

		printf("\nEntered VMThreadCreate!!!\n\n");
		
		if((entry == NULL) || (tid == NULL)){
			return VM_STATUS_ERROR_INVALID_PARAMETER;
		}

		ThreadStore *tStore = ThreadStore::getInstance(); 					//get the threadstore
		TCB* tcb = new TCB(entry, param, memsize, prio, tid);				//create TCB 
		tStore->insert(tcb);																				//pushes TCB into appropriate queue in ThreadStore
		MachineResumeSignals((TMachineSignalStateRef)&signalState);	//resume signals

		return VM_STATUS_SUCCESS;
	}



TVMStatus VMThreadSleep(TVMTick tick){
	//Puts running thread to sleep for tick ticks/quantums

	if(tick == VM_TIMEOUT_INFINITE){
		return VM_STATUS_ERROR_INVALID_PARAMETER;
	}
	
	ThreadStore *tStore = ThreadStore::getInstance();

	if(tick == VM_TIMEOUT_IMMEDIATE){

		printf("VMThreadSleep(): sleep immediately!\n");
		tStore->sleepCurrentThread(0);	//sets the current thread to sleep and schedules a new one in its place
	}
	else{
		printf("vmThreadSleep(): sleep for %d!\n", tick);
		tStore->sleepCurrentThread(tick);
	}//both cases sleep the running process and immediately call the scheduler to swap a new process in
	
		return VM_STATUS_SUCCESS;
	}

TVMStatus VMStart(int tickms, int machinetickms, int argc, char* argv[]){

	//tickms is the time in milliseconds for the alarm, this will be the quantum. 
	//machineticksms is the tick time of the machine, how long it will sleep in between actions.
	
	const char* module = (const char*) argv[0];	//get the module from the command-line args
	MachineInitialize(machinetickms);	

	ThreadStore* tStore = ThreadStore::getInstance();
	tStore->createMainThread();	//mainThread is ID 1
	printf("\nCreated Main Thread with TID: %d\n", tStore->getCurrentThread()->getThreadID());
	tStore->createIdleThread();	//idleThread is ID 2

	//request alarm every tickms USECONDS. alarm callback is schedule(high priority)
	MachineEnableSignals();	//enable signals	
	MachineRequestAlarm(1000*tickms, (TMachineAlarmCallback)&timerInterrupt, (void*)&defaultSchedulerPriority);	
	TVMMainEntry VMMain = VMLoadModule(module);							//load the module, save the VMMain function reference
	
//	testThreadCreate();
	
	if(VMMain != NULL){													//if VMMain is a valid reference,
		VMMain(argc, argv);												//run the loaded module,
		VMUnloadModule();													//then unload the module
		delete tStore;														//delete unnecessary container
		MachineTerminate();
		printf("\nsuccess!");
		return VM_STATUS_SUCCESS;
	}
	else{																				//if VMMain is null, the module failed to load
		VMUnloadModule();													//unload the module and return error status
		MachineTerminate();
		printf("\nVM failed");
		return VM_STATUS_FAILURE;
	}
//	return VM_STATUS_SUCCESS;
}

}
