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
	static const Range RANGE_RESERVED = 256*256;
	static const Range VAR_RANGE_MAX = ArithmeticEncoder::RANGE_MAX - RANGE_RESERVED;

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

	std::pair<
		unsigned char,
		std::pair<Range, Range>> getValue(Range _r)
	{
		if(_r <= VAR_RANGE_MAX)
		{
			unsigned char v = static_cast<unsigned char>(this->inverseCdf(
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

#endif // LAPLACEENCODINGDISTRIBUTION_HPP
