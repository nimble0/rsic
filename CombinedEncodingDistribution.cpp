/*
 * Copyright 2016 Erik Crevel
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
