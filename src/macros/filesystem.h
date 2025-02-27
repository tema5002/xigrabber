#pragma once

#ifdef WINDOWS
    #define CREATE_DIRECTORY(dir) _wmkdir(dir)
    #define DIRECTORY_EXISTS(dir) (_waccess(dir, 0) == 0)
    #define GET_FILENAME(path) PathFindFileName(path)
#else
    #define CREATE_DIRECTORY(dir) mkdir(dir, 0755) // rwxr-xr-x
    #define DIRECTORY_EXISTS(dir) (access(dir, F_OK) == 0)
    #define GET_FILENAME(path) basename((char*)path)
#endif

#ifdef WINDOWS
    #define FILE_HANDLE HANDLE
    #define OPEN_FILE_R(path) CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)
    #define OPEN_FILE_W(path) CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)
    #define CLOSE_FILE CloseHandle
#else
    #define FILE_HANDLE FILE*
    #define OPEN_FILE_R(path) fopen(path, "r")
    #define OPEN_FILE_W(path) fopen(path, "w")
    #define CLOSE_FILE fclose
#endif

#ifndef WINDOWS
#define INVALID_HANDLE_VALUE NULL
#endif

FILE_HANDLE safe_open_r(STRING s) {
    FILE_HANDLE f = OPEN_FILE_R(s);
    if (f == INVALID_HANDLE_VALUE)
        exit_error(FAILED_TO_OPEN_TEXT, s);
    return f;
}

FILE_HANDLE safe_open_w(STRING s) {
    FILE_HANDLE f = OPEN_FILE_W(s);
    if (f == INVALID_HANDLE_VALUE)
        exit_error(FAILED_TO_OPEN_TEXT, s);
    return f;
}

void safe_read(void* ptr, const size_t n, FILE_HANDLE file) {
    // ok i give up i have to do it in function now
#ifdef WINDOWS
    DWORD bytesRead;
    const BOOL result = ReadFile(
        file,
        ptr,
        (DWORD)n,
        &bytesRead,
        NULL
    );

    if (!result || bytesRead != n)
#else
    if (fread(ptr, 1, n, file) != n)
#endif
        exit_error(END_OF_FILE_TEXT);
}

void safe_write(const void* ptr, const size_t n, FILE_HANDLE file)  {
#ifdef WINDOWS
    DWORD bytesWritten;
    const BOOL result = WriteFile(
        file,
        ptr,
        (DWORD)n,
        &bytesWritten,
        NULL
    );
    if (!result || bytesWritten != n)
#else
    if (fwrite(ptr, 1, n, file) != n)
#endif
        exit_error(FILE_WRITE_FAIL_TEXT);
}

long get_filesize(FILE_HANDLE file) {
#ifdef WINDOWS
    LARGE_INTEGER filesize;
    GetFileSizeEx(file, &filesize);
    return filesize.QuadPart;
#else
    fseek(file, 0, SEEK_END);
    const long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);
    return filesize;
#endif
}