#pragma once

#define XIGRABBER_VERSION "2.2.0"
#define XIGRABBER_VERSION_FULL "XIGrabber v" XIGRABBER_VERSION
#if defined(_WIN32) || defined(_WIN64)
    #define WINDOWS
#endif

#ifdef WINDOWS
    #define MAIN wmain
#else
    #define MAIN main
#endif

#ifdef WINDOWS
    #include <direct.h>
    #include <io.h>
    #include <windows.h>
    #include <shlwapi.h>
    #include <wchar.h>
#else
    #include <stdlib.h>
    #include <string.h>
    #include <strings.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <libgen.h>
#endif

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "macros/help_message.h"
#include "macros/uint32_le.h"
#include "macros/xi_macros.h"

#include "macros/strings.h"
#include "exit_error.h"
#include "macros/filesystem.h"