//write VMThreadCreate
//transform the tick callback into a simple scheduler


#include "Machine.h"
#include <queue>
#include <list>

class ThreadStore;

extern "C"{

//Don't remove or comment out	==================================================================
//typedef void (*TVMMainEntry)(int, char*[]); //These are definitions for VirtualMachineUtils.c
TVMMainEntry VMLoadModule(const char* module); //They are implemented using Professor Nitta's provided source code in VirtualMachineUtils.c
void VMUnloadModule(void);
//TVMStatus VMFilePrint(int filedescriptor, const char *format, ...);
//===============================================================================================

void safeEntry(void *param);
void timerInterrupt(void *calldata);	//accepts a VM_THREAD_PRIORITY
void machineFileWriteCallback(void *data, int result);
void machineFileOpenCallback(void* calledParam, int returnedFileDescriptor);
void machineFileCloseCallback(void* calledParam, int result);
void machineFileReadCallback(void* calledParam, int result);
void machineFileSeekCallback(void* calledParam, int result);
void idle(void* parm1);
void dummyReturn(void* p);
}

#ifndef TCB_H
#define TCB_H

class TCB{

	public:
		TCB(TVMThreadEntry entry, void *param, TVMMemorySize memsize, TVMThreadPriority prio, TVMThreadIDRef tid);
		~TCB();
		TVMThreadPriority getPriority();
		TVMThreadID getThreadID();
		void sleep(TVMTick tick);
		TVMThreadState getState();
		void setState(TVMThreadState state);
		TVMTick getTicksToSleep();
		void decrementTicksToSleep();
		void setFileWriteResult(int result);
		int getFileWriteResult();
		void setFileOpenResult(int result);
		int getFileOpenResult();
		void setFileCloseResult(int result);
		int getFileCloseResult();
		void setFileReadResult(int result);
		int getFileReadResult();
		void setFileSeekResult(int result);
		int getFileSeekResult();
		bool isWaitingOnIO();
		void setIsWaitingOnIO(bool waiting);
		TVMThreadEntry getEntryFunction();
		SMachineContextRef contextRef;
		void releaseMutexes();

	private:
		
		TVMThreadEntry safeEntryFunction;
		int fileSeekResult;
		int fileWriteResult;
		int fileReadResult;
		int fileOpenResult;
		int fileCloseResult;
		int** safeEntryParam;
		TVMThreadID threadID;
		TVMThreadPriority priority;
		TVMThreadState state;
		TVMThreadEntry entryFunction;
		void* entryParam;
		void* stackBaseAddress;
		TVMMemorySize stackSize;
		TVMTick ticksToSleep;						//how many ticks to sleep the thread before awaking it
		bool waitingOnIO;
		//a mutex, when I get to that point
};

#endif

//Add Mutex class here
//	HAS TCB* to current owner
//	HAS value
//	ThreadStore - HAS list of mutexes? Otherwise may need a mutexStore (not doing that)
//
//For ThreadStore class - may need a switchToThreadByID method. Should be a simple change from current switching function,
//just change the call signature and add some logic to schedule()
//Test the new switcing logic in hello.so first 


#ifndef THREADSTORE_H
#define THREADSTORE_H

class ThreadStore{
	
	public:
		static ThreadStore *getInstance();
		~ThreadStore();
		void insert(TCB* tcb);
		int getNumThreads();
		void createIdleThread();
		void createMainThread();
		TCB* getCurrentThread();
		void sleepCurrentThread(TVMTick tick);
		void schedule(TVMThreadPriority priority);
		void timerEvent();
		TCB* findThreadByID(TVMThreadID ID);
		void activateDeadThread(TCB* deadThread);
		int voidParam;
		bool isThreadPrioInRange(TCB*);	//debug, remove
		void switchToNewThread(TCB* thread);
		void waitCurrentThreadOnIO();
		void terminate(TCB* thread);
		void terminateCurrentThread();
		void deleteDeadThread(TCB* thread);

	protected:
		ThreadStore();
//		void swapOutCurrentThread(TVMThreadID index);
		static ThreadStore *DUniqueInstance;
		TVMThreadID numThreads;
		TCB* idleThread;
		TCB* currentThread;
		TVMThreadID idleThreadID;
		std::list<TCB*>* readyLists[4];
		std::list<TCB*> *waitList;
		std::list<TCB*> *deadList;
};

#endif
