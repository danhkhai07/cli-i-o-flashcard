#pragma once

// DLL export symbol: CARD_EXPORT_DLL
// API: CARD_SHARED and CARD_LOCAL

#if defined _WIN32 || defined __CYGWIN__
    #ifdef CARD_EXPORT_DLL
        #define CARD_SHARED __declspec(dllexport) 
    #else
        #define CARD_SHARED __declspec(dllimport)
    #endif
    #define CARD_LOCAL
#else
    #if __GNUC__ >= 4
        #define CARD_SHARED __attribute__ ((visibility ("default")))
        #define CARD_LOCAL __attribute__ ((visibility ("hidden")))
    #else 
        #define CARD_SHARED
        #define CARD_LOCAL
    #endif
#endif