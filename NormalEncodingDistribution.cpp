/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#include "NormalEncodingDistribution.hpp"

#include <cmath>


NormalEncodingDistribution::NormalEncodingDistribution
(std::pair<int, int> _encodeRange, double _mu, double _sigma) :
	encodeRangeStart{_encodeRange.first},
	encodeRangeSize{_encodeRange.second - _encodeRange.first},

	dist(_mu, _sigma),
	start{boost::math::cdf(this->dist, _encodeRange.first - 0.5)},
	end{boost::math::cdf(this->dist, _encodeRange.second - 0.5)},
	scale{end - start}
{}

std::pair<ArithmeticEncoder::Range, ArithmeticEncoder::Range>
NormalEncodingDistribution::getRange(int _v) const
{
	double cfStart = (boost::math::cdf(this->dist, _v-0.5) - this->start)/this->scale;
	double cfEnd = (boost::math::cdf(this->dist, _v+0.5) - this->start)/this->scale;

	std::pair<Range, Range> range
	{
		std::ceil(cfStart * ArithmeticEncoder::RANGE_MAX),
		std::ceil(std::min(1.0, cfEnd) * ArithmeticEncoder::RANGE_MAX)
	};

	return range;
}

std::pair<int, std::pair<ArithmeticEncoder::Range, ArithmeticEncoder::Range>>
NormalEncodingDistribution::getValue(ArithmeticEncoder::Range _r) const
{
	unsigned char v = static_cast<unsigned char>(
		boost::math::quantile(this->dist,
			this->start + (static_cast<double>(_r)/ArithmeticEncoder::RANGE_MAX)*this->scale)
		+0.5);

	return { v, this->getRange(v) };
}
