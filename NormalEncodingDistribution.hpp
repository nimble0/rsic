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
	static const DoubleRange RANGE_RESERVED = 256*256;
	static const DoubleRange VAR_RANGE_MAX = std::numeric_limits<ArithmeticEncoder::Range>::max() - RANGE_RESERVED;
	static const DoubleRange START_RESERVED = VAR_RANGE_MAX + 1;

	boost::math::normal dist;

	double start, end;
	double scale;

public:
    NormalEncodingDistribution(double _mu, double _sigma) :
		dist(_mu, _sigma),
		start{boost::math::cdf(this->dist, -0.5)},
		end{boost::math::cdf(this->dist, 255.5)},
		scale{end-start}
	{}

	std::pair<Range, DoubleRange> getRange(unsigned char _v) const
	{
		double cfStart = (boost::math::cdf(this->dist, _v-0.5) - this->start)/this->scale;
		double cfEnd = (boost::math::cdf(this->dist, _v+0.5) - this->start)/this->scale;

		std::pair<Range, DoubleRange> range
		{
			std::ceil(cfStart * START_RESERVED),
			std::min(
				+START_RESERVED,
				static_cast<DoubleRange>(std::ceil(cfEnd * START_RESERVED)))
		};

		if(range.second - range.first < ArithmeticEncoder::MIN_RANGE_SIZE)
			return
			{
				START_RESERVED + _v*ArithmeticEncoder::MIN_RANGE_SIZE,
				START_RESERVED + (_v+1)*ArithmeticEncoder::MIN_RANGE_SIZE,
			};
		else
			return range;
	}

	std::pair<
		unsigned char,
		std::pair<Range, DoubleRange>> getValue(Range _r) const
	{
		if(_r < START_RESERVED)
		{
			unsigned char v = static_cast<unsigned char>(
				boost::math::quantile(this->dist,
					this->start + (static_cast<double>(_r)/START_RESERVED)*this->scale)
				+0.5);

			return { v, this->getRange(v) };
		}
		else
		{
			unsigned char v = ( _r - START_RESERVED)/ArithmeticEncoder::MIN_RANGE_SIZE;

			return
			{
				v,
				{
					START_RESERVED + v*ArithmeticEncoder::MIN_RANGE_SIZE,
					START_RESERVED + (v+1)*ArithmeticEncoder::MIN_RANGE_SIZE,
				}
			};
		}
	}
};

#endif // NORMALENCODINGDISTRIBUTION_HPP
