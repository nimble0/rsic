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
    NormalEncodingDistribution(double _mu, double _sigma) :
		dist(_mu, _sigma),
		start{boost::math::cdf(this->dist,-0.5)},
		end{boost::math::cdf(this->dist,255.5)},
		scale{end-start}
	{}

	std::pair<Range, Range> getRange(unsigned char _v)
	{
		double cfStart = boost::math::cdf(this->dist, _v-0.5);
		double cfSize = boost::math::cdf(this->dist, _v+0.5) - cfStart;

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

	std::pair<
		unsigned char,
		std::pair<Range, Range>> getValue(Range _r)
	{
		if(_r <= VAR_RANGE_MAX)
		{
			unsigned char v = static_cast<unsigned char>(boost::math::quantile(this->dist,
				this->start + ((static_cast<double>(_r)+1)/VAR_RANGE_MAX)*this->scale)+0.5);

			return { v, this->getRange(v) };
		}
		else
		{
			unsigned char v = ( _r - VAR_RANGE_MAX)/256;

			return
			{
				v,
				{
					VAR_RANGE_MAX + 1 + v*256,
					256
				}
			};
		}
	}
};

#endif // NORMALENCODINGDISTRIBUTION_HPP
