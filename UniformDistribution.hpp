/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef UNIFORMDISTRIBUTION_HPP
#define UNIFORMDISTRIBUTION_HPP

#include "Distribution.hpp"


class UniformDistribution : public Distribution<unsigned char, unsigned int>
{
public:
	std::pair<unsigned int, unsigned int>      getRange(unsigned char _v)
	{
		return {_v*256*256*256, 256*256*256-((_v==255)?1:0)};
	}
	std::pair<
		unsigned char,
		std::pair<unsigned int, unsigned int>> getValue(unsigned int _v)
	{
		unsigned char value = _v/(256*256*256);

		return {value, {value*256*256*256, 256*256*256-((value==255)?1:0)}};
	}
};

#endif // UNIFORMDISTRIBUTION_HPP
