/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef UNIFORMENCODINGDISTRIBUTION_HPP
#define UNIFORMENCODINGDISTRIBUTION_HPP

#include "EncodingDistribution.hpp"


class UniformEncodingDistribution : public EncodingDistribution<unsigned char>
{
public:
	using EncodingDistribution::EncodingDistribution;

	std::pair<Range, DoubleRange> getRange(unsigned char _v) const
	{
		return {_v*256*256*256, (_v+1)*256*256*256};
	}

	std::pair<
		unsigned char,
		std::pair<Range, DoubleRange>> getValue(Range _r) const
	{
		unsigned char v = _r/(256*256*256);

		return {v, this->getRange(v)};
	}
};

#endif // UNIFORMENCODINGDISTRIBUTION_HPP
