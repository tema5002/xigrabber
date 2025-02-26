#ifndef DIRECTORY_EXISTS
#ifdef WINDOWS
    #include <io.h>
    #define DIRECTORY_EXISTS(dir) (_waccess(dir, 0) == 0)
#else
    #include <unistd.h>
    #define DIRECTORY_EXISTS(dir) (access(dir, F_OK) == 0)
#endif
#endif
