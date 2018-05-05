#pragma once

#if defined _DEBUG && !defined DEBUG
#define DEBUG
#endif

#include"BlakesEngine/External/fmt/format.h"
#include <optional>

#include "BlakesEngine/Platform/beWindows.h"

//#include "BlakesEngine/Core/beMacros.h"
#include "BlakesEngine/Core/RangeIter.h"
#include "BlakesEngine/Core/Reconstruct.h"
#include "BlakesEngine/Core/ZeroMem.h"
#include "BlakesEngine/Core/bePimpl.h"
#include "BlakesEngine/Core/beTypes.h"
#include "BlakesEngine/Core/beNonCopiable.h"
#include "BlakesEngine/Core/bePrintf.h"
#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Core/beDeferred.h"

//#include "BlakesEngine/Core/beTypeTests.h"

