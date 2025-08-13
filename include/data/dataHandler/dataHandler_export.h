#pragma once

// DLL export symbol: DATAHANDLER_EXPORT_DLL
// API: DATAHANDLER_SHARED and DATAHANDLER_LOCAL

#if defined _WIN32 || defined __CYGWIN__
    #ifdef DATAHANDLER_EXPORT_DLL
        #define DATAHANDLER_SHARED __declspec(dllexport) 
    #else
        #define DATAHANDLER_SHARED __declspec(dllimport)
    #endif
    #define DATAHANDLER_LOCAL
#else
    #if __GNUC__ >= 4
        #define DATAHANDLER_SHARED __attribute__ ((visibility ("default")))
        #define DATAHANDLER_LOCAL __attribute__ ((visibility ("hidden")))
    #else 
        #define DATAHANDLER_SHARED
        #define DATAHANDLER_LOCAL
    #endif
#endif