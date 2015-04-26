//Working code for calling file.so and hello.so
#include "VirtualMachine.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SMALL_BUFFER_SIZE       256

TVMMainEntry VMLoadModule(const char* module);
void VMUnloadModule(void);
TVMStatus VMFilePrint(int filedescriptor, const char *format, ...);

TVMStatus VMStart(int tickms, int machinetickms, int argc, char *argv[]){
    TVMMainEntry funcPtr;
    funcPtr = VMLoadModule(argv[0]);
    if (funcPtr == NULL){
        fprintf(stderr,"Error loading failed %s\n",dlerror());
        return VM_STATUS_FAILURE;
    }
    else{
        funcPtr(argc, argv);
        return VM_STATUS_SUCCESS;
    }
}

TVMStatus VMFileOpen(const char *filename, int flags, int mode, int *filedescriptor){
    int temp;
    temp = open(filename, flags, mode);
    filedescriptor = &temp;
    if (filename || filedescriptor == NULL)
        return VM_STATUS_ERROR_INVALID_PARAMETER;
    else if (filedescriptor < 0)
        return VM_STATUS_FAILURE;
    else
        return VM_STATUS_SUCCESS;
}
TVMStatus VMFileClose(int filedescriptor){
    if (!(close(filedescriptor)))
        return VM_STATUS_FAILURE;
    else
        return VM_STATUS_SUCCESS;
}
TVMStatus VMFileRead(int filedescriptor, void *data, int *length){
    size_t temp;
    temp = abs(*length);
    
    if (data || length == NULL)
        return VM_STATUS_ERROR_INVALID_PARAMETER;
    else if (read(filedescriptor, data, temp) < 0)
        return VM_STATUS_FAILURE;
    else
        return VM_STATUS_SUCCESS;

}
TVMStatus VMFileWrite(int filedescriptor, void *data, int *length){
    size_t temp;
    temp = abs(*length);
    if (data || length != NULL){
        write(filedescriptor, data, temp);
        return VM_STATUS_SUCCESS;
    }
    else if (data || length == NULL)
        return VM_STATUS_ERROR_INVALID_PARAMETER;
    else
        return VM_STATUS_FAILURE;
}
TVMStatus VMFileSeek(int filedescriptor, int offset, int whence, int *newoffset){
    int temp;
    
    if (lseek(filedescriptor, offset*SMALL_BUFFER_SIZE, whence) <0)
        return VM_STATUS_FAILURE;
    else
    {
        temp = SEEK_CUR;
        newoffset = &temp;
        return VM_STATUS_SUCCESS;

    }
}

