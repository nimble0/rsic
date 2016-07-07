/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef NORMALENCODINGDISTRIBUTION_HPP
#define NORMALENCODINGDISTRIBUTION_HPP

#include "EncodingDistribution.hpp"

#include <boost/math/distributions/normal.hpp>


class NormalEncodingDistribution : public EncodingDistribution
{
	int encodeRangeStart, encodeRangeSize;

	boost::math::normal dist;

	double start, end;
	double scale;

public:
    NormalEncodingDistribution(std::pair<int, int> _encodeRange, double _mu, double _sigma);

	std::pair<Range, Range> getRange(int _v) const override;
	std::pair<int, std::pair<Range, Range>> getValue(Range _r) const override;
};

#endif // NORMALENCODINGDISTRIBUTION_HPP
