#pragma once

#define XIGRABBER_VERSION "XIGrabber v1.0.0    "
#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS
#endif

#ifdef WINDOWS
#define MAIN wmain
#else
#define MAIN main
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>

#include "macros/create_directory.h"
#include "macros/directory_exists.h"
#include "macros/help_message.h"
#include "macros/open_file.h"
#include "macros/strings.h"
#include "macros/uint32_le.h"
#include "macros/xi_macros.h"
