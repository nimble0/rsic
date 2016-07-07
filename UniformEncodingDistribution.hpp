/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef UNIFORMENCODINGDISTRIBUTION_HPP
#define UNIFORMENCODINGDISTRIBUTION_HPP

#include "EncodingDistribution.hpp"


class UniformEncodingDistribution : public EncodingDistribution
{
	int encodeRangeStart, encodeRangeSize;

public:
	UniformEncodingDistribution(std::pair<int, int> _encodeRange) :
		encodeRangeStart{_encodeRange.first},
		encodeRangeSize{_encodeRange.second - _encodeRange.first}
	{}

	std::pair<Range, Range> getRange(int _v) const override
	{
		return
		{
			(RANGE_MAX*static_cast<DoubleRange>(_v - this->encodeRangeStart))/this->encodeRangeSize,
			(RANGE_MAX*static_cast<DoubleRange>(_v - this->encodeRangeStart + 1))/this->encodeRangeSize
		};
	}

	std::pair<int, std::pair<Range, Range>> getValue(Range _r) const override
	{
		int v = this->encodeRangeStart
			+ (this->encodeRangeSize*static_cast<DoubleRange>(_r))/RANGE_MAX;

		return {v, this->getRange(v)};
	}
};

#endif // UNIFORMENCODINGDISTRIBUTION_HPP
