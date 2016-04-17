/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef LAPLACEENCODINGDISTRIBUTION_HPP
#define LAPLACEENCODINGDISTRIBUTION_HPP

#include "EncodingDistribution.hpp"

#include <cassert>
#include <cmath>
#include <boost/concept_check.hpp>


class LaplaceEncodingDistribution : public EncodingDistribution<unsigned char>
{
	static const DoubleRange RANGE_RESERVED = 256*ArithmeticEncoder::MIN_RANGE_SIZE;
	static const DoubleRange VAR_RANGE_MAX = std::numeric_limits<ArithmeticEncoder::Range>::max() - RANGE_RESERVED;
	static const DoubleRange START_RESERVED = VAR_RANGE_MAX + 1;

	double mu;
	double b;
	double start;
	double end;
	double scale;

	double cdf(double _x) const
	{
		if(_x < this->mu)
			return std::exp((_x-this->mu)/this->b)/2;
		else
			return 1-std::exp((this->mu-_x)/this->b)/2;
	}

	double inverseCdf(double _cdf) const
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

	std::pair<Range, DoubleRange> getRange(unsigned char _v)
	{
		double cfStart = (this->cdf(_v-0.5) - this->start)/this->scale;
		double cfEnd = (this->cdf(_v+0.5) - this->start)/this->scale;

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
		std::pair<Range, DoubleRange>> getValue(Range _r)
	{

		if(_r < START_RESERVED)
		{
			unsigned char v = static_cast<unsigned char>(
				this->inverseCdf(
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

#endif // LAPLACEENCODINGDISTRIBUTION_HPP
