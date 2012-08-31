#ifndef _beMacros_h_
#define _beMacros_h_

#include "BlakesEngine_PCH.h"

#define NULL 0
#define beUNUSED(x) void(sizeof(x))

// Need to use 2 macros do use __LINE__ as a string
#define STRINGIFY(x)     #x
#define STRINGIFY_PT2(x) STRINGIFY(x)
#define beLOCATION       __FILE__": Line("STRINGIFY_PT2(__LINE__)")"

#undef STRINGIFY_PT2
#undef STRINGIFY

#define BE_NEW new
#define BE_SAFE_DELETE(x) delete(x); x = NULL;

#define BE_MALLOC malloc
#define BE_SAFE_FREE(x) free(x); x = NULL;

#define BE_MEMCPY memcpy

#endif // _beMacros_h_
