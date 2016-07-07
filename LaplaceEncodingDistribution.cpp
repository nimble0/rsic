/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#include "LaplaceEncodingDistribution.hpp"


long double LaplaceEncodingDistribution::cdf(long double _x) const
{
	if(_x < this->mu)
		return std::exp((_x-this->mu)/this->b)/2;
	else
		return 1-std::exp((this->mu-_x)/this->b)/2;
}

long double LaplaceEncodingDistribution::inverseCdf(long double _cdf) const
{
	assert(_cdf >= 0 && _cdf <= 1);

	if(_cdf <= 0.5)
		return std::log(2*_cdf)*this->b + this->mu;
	else
		return this->mu - std::log(2 - 2*_cdf)*this->b;
}

LaplaceEncodingDistribution::LaplaceEncodingDistribution
(std::pair<int, int> _encodeRange, double _mu, double _sigma) :
	encodeRangeStart{_encodeRange.first},
	encodeRangeSize{_encodeRange.second - _encodeRange.first},

	mu{_mu},
	b{_sigma/std::sqrt(2)},

	start{this->cdf(_encodeRange.first - 0.5)},
	end{this->cdf(_encodeRange.second - 0.5)},
	scale{end-start}
{}

std::pair<ArithmeticEncoder::Range, ArithmeticEncoder::Range>
LaplaceEncodingDistribution::getRange(int _v) const
{
	long double cfStart = (this->cdf(_v-0.5) - this->start)/this->scale;
	long double cfEnd = (this->cdf(_v+0.5) - this->start)/this->scale;

	std::pair<Range, Range> range
	{
		std::ceil(cfStart * ArithmeticEncoder::RANGE_MAX),
		std::ceil(std::min(1.0L, cfEnd) * ArithmeticEncoder::RANGE_MAX)
	};

	return range;
}

std::pair<int, std::pair<ArithmeticEncoder::Range, ArithmeticEncoder::Range>>
LaplaceEncodingDistribution::getValue(ArithmeticEncoder::Range _r) const
{
	int v = static_cast<int>(
		this->inverseCdf(
			this->start + (static_cast<long double>(_r)/ArithmeticEncoder::RANGE_MAX)*this->scale)
		+0.5);

	std::pair<int, std::pair<Range, Range>> result = { v, this->getRange(v) };

	if(_r < result.second.first)
		result = { v-1, this->getRange(v-1) };

	return result;
}
