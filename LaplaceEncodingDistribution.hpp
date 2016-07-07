/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef LAPLACEENCODINGDISTRIBUTION_HPP
#define LAPLACEENCODINGDISTRIBUTION_HPP

#include "EncodingDistribution.hpp"

#include <cassert>
#include <cmath>


class LaplaceEncodingDistribution : public EncodingDistribution
{
	int encodeRangeStart, encodeRangeSize;

	double mu;
	double b;

	long double start;
	long double end;
	long double scale;

	long double cdf(long double _x) const;
	long double inverseCdf(long double _cdf) const;

public:
	LaplaceEncodingDistribution(std::pair<int, int> _encodeRange, double _mu, double _sigma);

	std::pair<Range, Range> getRange(int _v) const override;
	std::pair<int, std::pair<Range, Range>> getValue(Range _r) const override;
};

#endif // LAPLACEENCODINGDISTRIBUTION_HPP
