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

	std::pair<Range, Range> getRange(unsigned char _v)
	{
		return {_v*256*256*256, 256*256*256-((_v==255)?1:0)};
	}
};

#endif // UNIFORMENCODINGDISTRIBUTION_HPP
