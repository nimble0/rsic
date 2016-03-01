/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef UNIFORMDECODINGDISTRIBUTION_HPP
#define UNIFORMDECODINGDISTRIBUTION_HPP

#include "DecodingDistribution.hpp"


class UniformDecodingDistribution : public DecodingDistribution<unsigned char>
{
public:
	using DecodingDistribution::DecodingDistribution;

	std::pair<
		unsigned char,
		std::pair<Range, Range>> getValue(Range _v)
	{
		unsigned char value = _v/(256*256*256);

		return {value, {value*256*256*256, 256*256*256-((value==255)?1:0)}};
	}
};

#endif // UNIFORMDECODINGDISTRIBUTION_HPP
