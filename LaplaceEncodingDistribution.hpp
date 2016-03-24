/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef LAPLACEENCODINGDISTRIBUTION_HPP
#define LAPLACEENCODINGDISTRIBUTION_HPP

#include "EncodingDistribution.hpp"

#include <cmath>
#include <boost/concept_check.hpp>


class LaplaceEncodingDistribution : public EncodingDistribution<unsigned char>
{
	static const Range RANGE_RESERVED = 256*256;
	static const Range VAR_RANGE_MAX = ArithmeticEncoder::RANGE_MAX - RANGE_RESERVED;

	double mu;
	double b;
	double start;
	double end;
	double scale;

	double cdf(double _x)
	{
		if(_x < this->mu)
			return std::exp((_x-this->mu)/this->b)/2;
		else
			return 1-std::exp((this->mu-_x)/this->b)/2;
	}

public:
    LaplaceEncodingDistribution(ArithmeticEncoder& _encoder, double _mu, double _sigma) :
		EncodingDistribution<unsigned char>(_encoder),

		mu{_mu},
		b{_sigma/std::sqrt(2)},
		start{this->cdf(-0.5)},
		end{this->cdf(255.5)},
		scale{end-start}
	{}

	std::pair<Range, Range> getRange(unsigned char _v)
	{
		double cfStart = this->cdf(_v-0.5);
		double cfSize = this->cdf(_v+0.5) - cfStart;

		cfStart -= this->start;

		cfStart /= this->scale;
		cfSize /= this->scale;

		std::pair<Range, Range> range
		{
			cfStart * VAR_RANGE_MAX,
			cfSize * VAR_RANGE_MAX
		};

		if(range.second < 256)
			return
			{
				VAR_RANGE_MAX + 1 + _v*256,
				256
			};
		else
			return range;
	}
};

#endif // LAPLACEENCODINGDISTRIBUTION_HPP
