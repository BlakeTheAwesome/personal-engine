#pragma once

#if defined _DEBUG && !defined DEBUG
#define DEBUG
#endif

//#define _USE_MATH_DEFINES
//#include "BlakesEngine/Core/beConcepts.h"
#include "BlakesEngine/Core/beRanges.h"
#include <cmath>  

#include"BlakesEngine/External/fmt/format.h"
#include"BlakesEngine/Core/beGSL.h"
#include"BlakesEngine/Core/beSpan.h"
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
#include "BlakesEngine/Core/optional_arg.h"
#include "BlakesEngine/DataStructures/beArray.h"
#include "BlakesEngine/DataStructures/beVector.h"
#include "BlakesEngine/Core/beContainerHelpers.h"
#include "BlakesEngine/Math/beMath.h"
#include "BlakesEngine/Math/beMathFormat.h"
//#include "BlakesEngine/Core/beTypeTests.h"

