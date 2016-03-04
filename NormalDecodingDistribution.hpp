/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef NORMALDECODINGDISTRIBUTION_HPP
#define NORMALDECODINGDISTRIBUTION_HPP

#include "DecodingDistribution.hpp"

#include <cmath>
#include <boost/math/distributions/normal.hpp>


class NormalDecodingDistribution : public DecodingDistribution<unsigned char>
{
	boost::math::normal dist;

	double start, end;
	double scale;

	static const Range RESERVED = 65536;
	static const Range VAR_RANGE_SIZE = ArithmeticDecoder::RANGE_MAX - RESERVED;

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

public:
    NormalDecodingDistribution(ArithmeticDecoder& _decoder, double _mu, double _sigma) :
		DecodingDistribution<unsigned char>(_decoder),

		dist(_mu, _sigma),
		start{boost::math::cdf(this->dist,0)},
		end{1-boost::math::cdf(this->dist,256)},
		scale{1-start-end}
	{}

	std::pair<
		unsigned char,
		std::pair<Range, Range>> getValue(Range _r)
	{
		unsigned char v = static_cast<unsigned char>(boost::math::quantile(this->dist,
			this->start + ((static_cast<double>(_r)+1)/ArithmeticDecoder::RANGE_MAX)*this->scale));

		return { v, this->getRange(v) };
	}
};

#endif // NORMALDECODINGDISTRIBUTION_HPP
