#ifndef OPEN_FILE
#ifdef WINDOWS
    #define OPEN_FILE(fp, filename, mode) _wfopen_s(&fp, filename, mode)
#else
    #define OPEN_FILE(file, fp, mode) (file = fopen(fp, mode))
#endif
#endif
