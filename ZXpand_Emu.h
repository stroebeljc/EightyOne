// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ZXPAND_DLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ZXPAND_DLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef ZXPAND_DLL_EXPORTS
#define ZXPAND_DLL_API __declspec(dllexport)
#else
#define ZXPAND_DLL_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
   ZXPAND_DLL_API void* ZXpand_Create(const char* imagename);
   ZXPAND_DLL_API void ZXpand_Destroy(void* instance);
   ZXPAND_DLL_API void ZXpand_Update(void* instance, int millis);
   ZXPAND_DLL_API void ZXpand_IO_Write(void* instance, int data, int addr);
   ZXPAND_DLL_API int ZXpand_IO_Read(void* instance, int addr);
#ifdef __cplusplus
};
#endif

typedef void*(*ZXP_CREATEFN)(const char*);
typedef void*(*ZXP_DESTROYFN)(void*);
typedef void(*ZXP_UPDATEFN)(void*,int);
typedef void(*ZXP_IOWRFN)(void*,int,int);
typedef int(*ZXP_IORDFN)(void*,int);
