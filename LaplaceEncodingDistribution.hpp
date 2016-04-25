/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef LAPLACEENCODINGDISTRIBUTION_HPP
#define LAPLACEENCODINGDISTRIBUTION_HPP

#include "EncodingDistribution.hpp"

#include <cassert>
#include <cmath>


class LaplaceEncodingDistribution : public EncodingDistribution<unsigned char>
{
	static const DoubleRange RANGE_RESERVED = 256*256;
	static const DoubleRange VAR_RANGE_MAX = std::numeric_limits<ArithmeticEncoder::Range>::max() - RANGE_RESERVED;
	static const DoubleRange START_RESERVED = VAR_RANGE_MAX + 1;

	double mu;
	double b;
	long double start;
	long double end;
	long double scale;

	long double cdf(long double _x) const
	{
		if(_x < this->mu)
			return std::exp((_x-this->mu)/this->b)/2;
		else
			return 1-std::exp((this->mu-_x)/this->b)/2;
	}

	long double inverseCdf(long double _cdf) const
	{
		assert(_cdf >= 0 && _cdf <= 1);

		if(_cdf <= 0.5)
			return std::log(2*_cdf)*this->b + this->mu;
		else
			return this->mu - std::log(2 - 2*_cdf)*this->b;
	}

public:
    LaplaceEncodingDistribution(double _mu, double _sigma) :
		mu{_mu},
		b{_sigma/std::sqrt(2)},
		start{this->cdf(-0.5)},
		end{this->cdf(255.5)},
		scale{end-start}
	{}

	std::pair<Range, DoubleRange> getRange(unsigned char _v) const
	{
		long double cfStart = (this->cdf(_v-0.5) - this->start)/this->scale;
		long double cfEnd = (this->cdf(_v+0.5) - this->start)/this->scale;

		std::pair<Range, DoubleRange> range
		{
			std::ceil(cfStart * START_RESERVED),
			std::min(
				+START_RESERVED,
				static_cast<DoubleRange>(std::ceil(cfEnd * START_RESERVED)))
		};

		if(range.second - range.first < 256)
			return
			{
				START_RESERVED + _v*256,
				START_RESERVED + (_v+1)*256,
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
				this->inverseCdf(
					this->start + (static_cast<long double>(_r)/START_RESERVED)*this->scale)
				+0.5);

			return { v, this->getRange(v) };
		}
		else
		{
			unsigned char v = ( _r - START_RESERVED)/256;

			return
			{
				v,
				{
					START_RESERVED + v*256,
					START_RESERVED + (v+1)*256,
				}
			};
		}
	}
};

#endif // LAPLACEENCODINGDISTRIBUTION_HPP
