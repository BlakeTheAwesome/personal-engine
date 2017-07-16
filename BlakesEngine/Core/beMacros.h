#pragma once

#include <cstring> // memcpy
#include <malloc.h> // aligned_alloc

#define BE_UNUSED(x) void(sizeof(x))

// Need to use 2 macros do use __LINE__ as a string
#define _STRINGIFY(x)     #x
#define _STRINGIFY_PT2(x) _STRINGIFY(x)
#define LOCATION       __FILE__" (" _STRINGIFY_PT2(__LINE__) ")"

#define CONCAT_IMPL( x, y ) x##y
#define MACRO_CONCAT( x, y ) CONCAT_IMPL( x, y )

#define BE_NEW new
#define BE_DELETE(x) delete(x)
#define BE_SAFE_DELETE(x) delete(x); x = nullptr;

#define BE_MALLOC malloc
#define BE_MALLOC_ALIGNED(alignment, size) _aligned_malloc(size, alignment) // aligned_alloc not found?
#define BE_FREE(x) free(x)
#define BE_SAFE_FREE(x) if (x){free(x); x = nullptr;};
#define BE_FREE_ALIGNED(x) if (x){_aligned_free(x); x = nullptr;};

#define BE_SAFE_RELEASE(x) if (x){ x->Release(); x = nullptr;};
#define BE_SAFE_DESTROY(x) if (x){ x->Destroy(); x = nullptr;};

#define BE_MEMCPY memcpy

#define BE_ARRAY_DIMENSION(x) (sizeof(x) / (sizeof(x[0])))

template <int wow> struct ttWow;

