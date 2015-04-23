#include "ThreadStore.h" //implements a data structure and associated functions to store threads, delete threads, prioritize threads, etc
#include "VirtualMachine.h"
#include "Machine.h"

//should remove this one after the project is done
#include <iostream>



extern "C"{
	using namespace std;

//These are definitions for VirtualMachineUtils.c
//They are implemented using Professor Nitta's provided source code in VirtualMachineUtils.c
	typedef void (*TVMMainEntry)(int, char*[]);
	TVMMainEntry VMLoadModule(const char* module);
	void VMUnloadModule(void);
	TVMStatus VMFilePrint(int filedescriptor, const char *format, ...);
//end definitions
	
	//Dan's Threading Functions
	
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
		
	void clearAlarmCounter(void* data){ 
		//resets the sleep time to zero
		useconds_t* mydata = (useconds_t*)data;
		*mydata -= 1;	//cast data to a useconds_t pointer and reset it to zero
//		cout << "This should only appear once" << endl;
		cout << "decremented alarm counter" << endl;
		return;
	}
	
	TVMStatus VMThreadSleep(TVMTick tick){
		//TVMTick is just an unsigned int
		//MachineRequestAlarm call signature:
			//void MachineRequestAlarm(useconds_t usec, TMachineAlarmCallback callback, void *calldata);
			//useconds_t is also an unsigned integer
			
	
	//VMThreadSleep puts the currently running thread to sleep for a number of ticks specified by parameter tick
	//if tick is the same as VM_TIMEOUT_IMMEDIATE, 
		//the current process immediately goes to sleep
		//the next ready process of EQUAL PRIORITY (NOT greater OR lesser) wakes up and begins executing,
		//the process that just woke up and started executing gets the remainder of the processing time that the previous thread just gave up
		
		//what I actually want to call here is change the thread's state to the WAIT state
		
		cout << "going to sleep" << endl;	//debug
		//set the callback function pointer here
		//		MachineRequestAlarm((useconds_t) tick, callback, &tick);		//request an alarm here, for useconds_t after calling the function
		for(mytick; mytick > 0;){
			cout << "This should keep appearing until the alarm goes off" << endl;
		}//do nothing while the time to sleep is greater than zero
		cout << "woke back up" << endl;	//debug
		
		//As per nitta's instructions on Piazza:  set up an alarm using MachineRequestAlarm to sleep and wake the thread
		
	//if the currently running thread successfully goes to sleep,
			return VM_STATUS_SUCCESS;
	//if the sleep duration tick is the same as VM_TIMEOUT_INFINITE,
	//		return VM_STATUS_ERROR_INVALID_PARAMETER;
}
	
	TVMStatus VMStart(int tickms, int machinetickms, int argc, char* argv[]){

		//tickms is the time in milliseconds for the alarm, this will be the quantum. 
		//machineticksms is the tick time of the machine, how long it will sleep in between actions. This was necessary because ppoll doesn't exist on all systems.
	
		//The following 3 lines initialize the machine layer
		MachineInitialize(machinetickms);	//initialize the machine
		TMachineAlarmCallback callback = &decrementAlarmCounter;
		MachineRequestAlarm(tickms, callback, &mytick);		//request an alarm here, for useconds_t after calling the function
		
		//here, create a singleton ThreadStore to hold all the threads
	
		TVMMainEntry VMMain;	//declare the variable to hold the function pointer to the loaded module's main() function	
		const char* module = (const char*) argv[0];	//get the module from the command-line args
		VMMain = VMLoadModule(module);							//load the module using VMLoad, save the VMMain function reference

		if(VMMain != NULL){													//if VMMain is a valid reference,
			VMMain(argc, argv);												//run the loaded module,
			VMUnloadModule();													//then unload the module
			cout << "VMMain ran successfully" << endl;
//			ThreadStore* tstore = new ThreadStore();
//			tstore->addNumber(5);
//			tstore->sayHi();
//			delete tstore;
			return VM_STATUS_SUCCESS;
		}
		else{																				//if VMMain is null, then the module failed to load
			VMUnloadModule();													//unload the module and return error status
			cout << "VMMain failed" << endl;
			return VM_STATUS_FAILURE;			
		}
	}
	
	TVMStatus VMFileWrite(int fileDescriptor, void *data, int *length){
	
		//this is another dummy function to make the code compile
		return VM_STATUS_SUCCESS;
	
	}

}