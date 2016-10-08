/*
 * Copyright 2016 Erik Crevel
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
