#include "ThreadStore.h" //implements a data structure and associated functions to store threads, delete threads, prioritize threads, etc
#include "VirtualMachine.h"
#include "Machine.h"

//should remove this one after the project is done
#include <iostream>
#include <cstdio>



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
	}
	//End Vincent's File Functions
	*/

	//Dan's Threading Functions
	
	TVMStatus VMThreadState(TVMThreadID thread, TVMThreadStateRef state){
		//retrieves the state of the thread specified by parameter thread, and places that state into the location specified by parameter state
		//does NOT RETURN the state of the thread. parameter state IS AN ADDRESS. Assigns the thread state BY REFERENCE, eg *state = threadstate
		
		//if the thread state is successfully retrieved from the VM and stored in the address specified by state,
				return VM_STATUS_SUCCESS;
		//if the thread specified by parameter thread does not exist,
		//		return VM_STATUS_ERROR_INVALID_ID;
		//if the parameter stateref is NULL,
		//		return VM_STATUS_ERROR_INVALID_PARAMETER;
	}
	
	TVMStatus VMThreadID(TVMThreadIDRef threadref){
		//puts the thread identifier of the currently running thread into the location specified by threadref
		//does NOT RETURN the thread identifier! Sets the thread identifier BY REFERENCE, eg *threadref = threadid. threadref is an ADDRESS.
		
		//if the thread identifier is successfully retrieved from the virtual machine and stored in the location specified by threadref,
				return VM_STATUS_SUCCESS;
		//if the thread specified by the thread identifier thread does not exist,
			//		return VM_STATUS_ERROR_INVALID_ID;
						//thread is not specified in the function call, so again, I'm assuming I can get it from the virtual machine
						//may ask on piazza about this
			//if the parameter threadref is NULL,
				//	return VM_STATUS_ERROR_INVALID_PARAMETER;
	}
	
	TVMStatus VMThreadTerminate(TVMThreadID thread){
		//terminates the dead thread specified by parameter thread in the virtual machine
		//thread must be in the DEAD state for this function to succeed
		//after termination, the thread enters the state VM_THREAD_STATE_DEAD
		//the thread must release any mutexes it holds
		//termination of a thread can trigger the scheduling of another thread, account for this!
			//
		
		//if thread is terminated successfully,
				return VM_STATUS_SUCCESS;
		//if thread id specified by thread does not exist,
			//		return VM_STATUS_ERROR_INVALID_ID;
		//if thread does exist but is not in the dead state,
			//		return VM_STATUS_ERROR_INVALID_STATE;
	}
	
	TVMStatus VMThreadActivate(TVMThreadID thread){
		//activates the dead thread specified by parameter thread in the virtual machine
		//thread MUST be in the DEAD state for this function to return correctly
		//after activation, the thread enters the ready state VM_THREAD_STATE_READY
		//thread then begins execution at the entry function specified
			//(presumably, entry function is specified in the thread, since entry isn't a parameter for this function)
			//may need to ask on piazza
		
		//if the thread is successfully activated in the virtual machine,
				return VM_STATUS_SUCCESS;
		//If the thread specified by the thread identifier thread does not exist,
				//return VM_STATUS_ERROR_INVALID_ID;
		//If the thread does exist but is not in the dead state,
		//		return VM_STATUS_ERROR_INVALID_STATE;
	}
	
	TVMStatus VMThreadDelete(TVMThreadID thread){
		//deletes the DEAD thread specified by parameter thread from the virtual machine
		//the thread specified by TVMThreadID MUST BE DEAD in order to be deleted
		
		//if the thread is deleted successfully,
		return VM_STATUS_SUCCESS;
		//if the thread specified by teh thread identifier thread does not exist,
				//return VM_STATUS_ERROR_INVALID_ID;
		//if the thread does exist, but it is not in the dead state,
				//return VM_STATUS_ERROR_INVALID_STATE;
	}
		
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
	}
	
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