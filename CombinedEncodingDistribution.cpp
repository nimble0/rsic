/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#include "CombinedEncodingDistribution.hpp"


std::pair<ArithmeticEncoder::Range, ArithmeticEncoder::Range>
CombinedEncodingDistribution::getRange(int _v) const
{
	std::pair<WorkingRange, WorkingRange> range;
	Range scale = 0;

	for(const auto& dist : this->dists)
	{
		auto distRange = dist.second->getRange(_v);

		range.first += dist.first * distRange.first;
		range.second += dist.first * distRange.second;

		scale += dist.first;
	}

	return { range.first/scale, range.second/scale };
}

std::pair<int, std::pair<ArithmeticEncoder::Range, ArithmeticEncoder::Range>>
CombinedEncodingDistribution::getValue(Range _v) const
{
	std::pair<int, Range> low = { 0, 0 };
	std::pair<int, Range> high = { 255, +RANGE_MAX };

	while(true)
	{
		int mid = (low.first + high.first)/2;
		auto range = this->getRange(mid);

		if(_v < range.first)
		{
			std::pair<int, Range> newHigh = { mid-1, range.first-1 };

			assert(high != newHigh);

			high = newHigh;
		}
		else if(_v >= range.second)
		{
			std::pair<int, Range> newLow = { mid+1, range.second };

			assert(low != newLow);

			low = newLow;
		}
		else
			return { mid, range };
	}
}
