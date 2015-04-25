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

TVMStatus VMFileOpen(const char *filename, int flags, int mode, int *filedescriptor){
    FILE* file;
    int temp;
    struct stat *perm;
    stat(filename, perm);
    open(filename, flags, mode);
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
    
    if (data || length == NULL)
        return VM_STATUS_ERROR_INVALID_PARAMETER;
    else if (!(write(filedescriptor, data, temp)))
        return VM_STATUS_FAILURE;
    else
        return VM_STATUS_SUCCESS;
}
TVMStatus VMFileSeek(int filedescriptor, int offset, int whence, int *newoffset){
    char buffer[SMALL_BUFFER_SIZE];
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

