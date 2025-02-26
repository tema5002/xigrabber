#ifndef CREATE_DIRECTORY
#ifdef WINDOWS
    #include <direct.h>
    #define CREATE_DIRECTORY(dir) _wmkdir(dir)
#else
    #include <sys/stat.h>
    #define CREATE_DIRECTORY(dir) mkdir(dir, 0755) // rwxr-xr-x
#endif
#endif
