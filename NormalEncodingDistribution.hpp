/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef NORMALENCODINGDISTRIBUTION_HPP
#define NORMALENCODINGDISTRIBUTION_HPP

#include "EncodingDistribution.hpp"

#include <cmath>
#include <boost/math/distributions/normal.hpp>


class NormalEncodingDistribution : public EncodingDistribution<unsigned char>
{
	static const Range RANGE_RESERVED = 256*256;
	static const Range VAR_RANGE_MAX = ArithmeticEncoder::RANGE_MAX - RANGE_RESERVED;

	boost::math::normal dist;

	double start, end;
	double scale;

public:
    NormalEncodingDistribution(ArithmeticEncoder& _encoder, double _mu, double _sigma) :
		EncodingDistribution<unsigned char>(_encoder),

		dist(_mu, _sigma),
		start{boost::math::cdf(this->dist,0)},
		end{boost::math::cdf(this->dist,256)},
		scale{end-start}
	{}

	std::pair<Range, Range> getRange(unsigned char _v)
	{
		double cfStart = boost::math::cdf(this->dist, _v);
		double cfSize = boost::math::cdf(this->dist, _v+1) - cfStart;

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

#endif // NORMALENCODINGDISTRIBUTION_HPP
