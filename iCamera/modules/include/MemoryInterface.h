#ifndef H_MEMORY_FUNCTION_INTERFACE
#define H_MEMORY_FUNCTION_INTERFACE

#if defined( __cplusplus)
#define MEMORY_API  extern "C" 
#else
#define MEMORY_API  
#endif

//内存分配
#if defined( __cplusplus)
MEMORY_API void* MemoryMalloc(int size, const char *module, const char *file = __FILE__, const char *function = __FUNCTION__, int line = __LINE__);
#else
MEMORY_API void* MemoryMalloc(int size, const char *module, const char *file, const char *function, int line);
#endif

MEMORY_API void MemoryFree(const void *buffer);

//使用内存池
#ifdef USE_MEMORY_POOL
#define MEMORY_MALLOC(length, cfile, cfun, cline) MemoryMalloc(length, MODULE_NAME, cfile, cfun, cline)
#define MEMORY_FREE	MemoryFree
#else
#define MEMORY_MALLOC(length, cfile, cfun, cline) malloc(length)
#define MEMORY_FREE	free
#endif

#endif
