#pragma once

// DLL export symbol: COMMANDS_EXPORT_DLL
// API: COMMANDS_SHARED and COMMANDS_LOCAL

#if defined _WIN32 || defined __CYGWIN__
    #ifdef COMMANDS_EXPORT_DLL
        #define COMMANDS_SHARED __declspec(dllexport) 
    #else
        #define COMMANDS_SHARED __declspec(dllimport)
    #endif
    #define COMMANDS_LOCAL
#else
    #if __GNUC__ >= 4
        #define COMMANDS_SHARED __attribute__ ((visibility ("default")))
        #define COMMANDS_LOCAL __attribute__ ((visibility ("hidden")))
    #else 
        #define COMMANDS_SHARED
        #define COMMANDS_LOCAL
    #endif
#endif