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
	static const Range RANGE_RESERVED = 256*256;
	static const Range VAR_RANGE_MAX = ArithmeticDecoder::RANGE_MAX - RANGE_RESERVED;

	boost::math::normal dist;

	double start, end;
	double scale;

	std::pair<Range, Range> getRange(unsigned char _v)
	{
		double cfStart = boost::math::cdf(this->dist, _v-0.5);
		double cfSize = boost::math::cdf(this->dist, _v+0.5) - cfStart;

		cfStart -= this->start;

		cfStart /= this->scale;
		cfSize /= this->scale;

		return
		{
			cfStart * VAR_RANGE_MAX,
			cfSize * VAR_RANGE_MAX
		};
	}

public:
    NormalDecodingDistribution(ArithmeticDecoder& _decoder, double _mu, double _sigma) :
		DecodingDistribution<unsigned char>(_decoder),

		dist(_mu, _sigma),
		start{boost::math::cdf(this->dist,-0.5)},
		end{boost::math::cdf(this->dist,255.5)},
		scale{end-start}
	{}

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

#endif // NORMALDECODINGDISTRIBUTION_HPP
