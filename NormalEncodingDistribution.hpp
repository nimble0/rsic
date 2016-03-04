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
	boost::math::normal dist;

	double start, end;
	double scale;

public:
    NormalEncodingDistribution(ArithmeticEncoder& _encoder, double _mu, double _sigma) :
		EncodingDistribution<unsigned char>(_encoder),

		dist(_mu, _sigma),
		start{boost::math::cdf(this->dist,0)},
		end{1-boost::math::cdf(this->dist,256)},
		scale{1-start-end}
	{}

	std::pair<Range, Range> getRange(unsigned char _v)
	{
		double cfStart = boost::math::cdf(this->dist, _v);
		double cfSize = boost::math::cdf(this->dist, _v+1) - cfStart;

		cfStart -= this->start;

		cfStart /= this->scale;
		cfSize /= this->scale;

		return
		{
			cfStart * ArithmeticEncoder::RANGE_MAX,
			cfSize * ArithmeticEncoder::RANGE_MAX
		};
	}
};

#endif // NORMALENCODINGDISTRIBUTION_HPP
