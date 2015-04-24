#include "VirtualMachine.h"
#include "Machine.h"

#include <cstdio>


extern "C"{
	
typedef void (*TVMMainEntry)(int, char*[]); //These are definitions for VirtualMachineUtils.c
TVMMainEntry VMLoadModule(const char* module); //They are implemented using Professor Nitta's provided source code in VirtualMachineUtils.c
void VMUnloadModule(void);
TVMStatus VMFilePrint(int filedescriptor, const char *format, ...);

int alarmCounter;
volatile int globalflag;


void alarmCallback(void *calldata){
	printf("\nin alarm callback");
	alarmCounter -= 1;
	return;
}

void dummy(void *data, int result){
	
	*((int*)data) = 1;
	globalflag = 1;
	printf("\nIn the MachineFileWrite callback!");
	return;
}

TVMStatus VMFileWrite(int fileDescriptor, void* data, int *length){
	
	
	TMachineFileCallback callback = &dummy;
	volatile int calldata = 1337;
	volatile void *cdptr = &calldata;
	
	if((data == NULL) || (length == NULL))
		return VM_STATUS_ERROR_INVALID_PARAMETER;
	
//	MachineFileWrite(int fd, void *data, int length, TMachineFileCallback callback, void *calldata);
	MachineFileWrite(fileDescriptor, data, *length, callback, cdptr);
	
	while(calldata > 10);	//busy wait until MachineFileWrite is done

	printf("\nEnd of VMFileWrite");
	return VM_STATUS_SUCCESS;
}

TVMStatus VMStart(int tickms, int machinetickms, int argc, char* argv[]){
	//tickms is the time in milliseconds for the alarm, this will be the quantum. 
	//machineticksms is the tick time of the machine, how long it will sleep in between actions.
	
	globalflag = 0;
	
	printf("\nglobalflag: %d", globalflag);
	
	alarmCounter = 100;
	MachineInitialize(machinetickms);	//Initialize the machine
	TMachineAlarmCallback callback = &alarmCallback;		//useconds_t is an unsigned integer
	MachineRequestAlarm(1000*tickms, callback, &alarmCounter);				//request an alarm here, for ticms MICROSECONDS after calling the function 
	MachineEnableSignals();	//enable signals	
	TVMMainEntry VMMain;	//declare the variable to hold the function pointer to the loaded module's main() function	
	const char* module = (const char*) argv[0];	//get the module from the command-line args
	VMMain = VMLoadModule(module);							//load the module using VMLoad, save the VMMain function reference
	
	if(VMMain != NULL){													//if VMMain is a valid reference,
		VMMain(argc, argv);												//run the loaded module,
		VMUnloadModule();													//then unload the module
		printf("\nglobalflag: %d", globalflag);
		printf("\nVM succeeded\n");
		return VM_STATUS_SUCCESS;
	}
	else{																				//if VMMain is null, then the module failed to load
		VMUnloadModule();													//unload the module and return error status
		printf("\nVM failed");
		return VM_STATUS_FAILURE;			
	}
}

}