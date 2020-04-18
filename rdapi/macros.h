#pragma once

#ifdef __cplusplus
    #define RD_API_EXTERN_C extern "C"
#else
    #define RD_API_EXTERN_C
#endif

#if defined _MSC_VER // Defined by Visual Studio
  #define RD_API_IMPORT RD_API_EXTERN_C __declspec(dllimport)
  #define RD_API_EXPORT RD_API_EXTERN_C __declspec(dllexport)
#else
  #if __GNUC__ >= 4 // Defined by GNU C Compiler. Also for C++
    #define RD_API_IMPORT RD_API_EXTERN_C __attribute__ ((visibility ("default")))
    #define RD_API_EXPORT RD_API_EXTERN_C __attribute__ ((visibility ("default")))
  #else
    #define RD_API_IMPORT RD_API_EXTERN_C
    #define RD_API_EXPORT RD_API_EXTERN_C
  #endif
#endif

#define DECLARE_HANDLE(name) typedef struct name { void* __unused; } name
