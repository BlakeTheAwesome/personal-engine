#pragma once
#include "beRandom.h"
#include "BlakesEngine/Core/beSpan.h"

namespace beRandomFunctions
{
	int WeightedRandom(beRandom* random, std::span<int> weights)
	{
		BE_ASSERT(weights.size() > 0);
		auto sum = std::accumulate(weights.begin(), weights.end(), 0);
		auto target = random->Next(0, sum);
		auto remaining = target;
		for (int i : RangeIter((int)weights.size()))
		{
			auto weight = weights[i];
			if (weight > 0)
			{
				remaining -= weight;
				if (remaining <= 0)
				{
					//LOG("Target:{} total:{}, percent:{:.2f}% Weights:{},{},{} Result:{}", (double)target, (double)sum, (double)target/(double)sum, weights[0], weights[1], weights[2], i);
					return i;
				}
			}
		}
		BE_ASSERT(false);
		return (int)(weights.size()-1);
	}
}
