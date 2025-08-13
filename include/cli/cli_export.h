#pragma once

// DLL export symbol: CLI_EXPORT_DLL
// API: CLI_SHARED and CLI_LOCAL

#if defined _WIN32 || defined __CYGWIN__
    #ifdef CLI_EXPORT_DLL
        #define CLI_SHARED __declspec(dllexport) 
    #else
        #define CLI_SHARED __declspec(dllimport)
    #endif
    #define CLI_LOCAL
#else
    #if __GNUC__ >= 4
        #define CLI_SHARED __attribute__ ((visibility ("default")))
        #define CLI_LOCAL __attribute__ ((visibility ("hidden")))
    #else 
        #define CLI_SHARED
        #define CLI_LOCAL
    #endif
#endif