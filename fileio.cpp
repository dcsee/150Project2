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
TVMStatus VMFileRead(int filedescriptor, void *data, int *length){

}
TVMStatus VMFileWrite(int filedescriptor, void *data, int *length){
    if (data || length == NULL)
        return VM_STATUS_ERROR_INVALID_PARAMETER;
    else if (!(write(filedescriptor, data, length))
        return VM_STATUS_FAILURE;
    else
        return VM_STATUS_SUCCESS;
}
TVMStatus VMFileSeek(int filedescriptor, int offset, int whence, int *newoffset){
    
}
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
