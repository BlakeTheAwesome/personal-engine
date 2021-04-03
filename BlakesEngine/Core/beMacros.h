#pragma once

#include <cstring> // memcpy
#include <malloc.h> // aligned_alloc

template <typename ...Args> static inline void Unused(Args&& ...args) {}

// Need to use 2 macros do use __LINE__ as a string
#define _STRINGIFY(x)     #x
#define _STRINGIFY_PT2(x) _STRINGIFY(x)
#define LOCATION       __FILE__" (" _STRINGIFY_PT2(__LINE__) ")"

#define CONCAT_IMPL( x, y ) x##y
#define MACRO_CONCAT( x, y ) CONCAT_IMPL( x, y )

#define BE_MALLOC_ALIGNED(alignment, size) _aligned_malloc(size, alignment) // aligned_alloc not found?
#define BE_FREE_ALIGNED(x) if ((x)){_aligned_free((x)); (x) = nullptr;};

#define BE_SAFE_DELETE(x) delete((x)); (x) = nullptr;
#define BE_SAFE_FREE(x) if ((x)){free((x)); (x) = nullptr;};

#define BE_SAFE_RELEASE(x) if ((x)){ (x)->Release(); (x) = nullptr;};
#define BE_SAFE_DESTROY(x) if ((x)){ (x)->Destroy(); (x) = nullptr;};

template <int wow> struct ttWow;

