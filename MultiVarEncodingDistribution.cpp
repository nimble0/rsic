/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#include "MultiVarEncodingDistribution.hpp"

#include <algorithm>

#include "LaplaceEncodingDistribution.hpp"


// o' = (o0*o1*o2)/(o0*o1 + o0*o2 + o1*o2)
// u' = (u0*o1*o2 + u1*o0*o2 + u2*o0*o1)/(o0*o1 + o0*o2 + o1*o2)
//    = o'*u0/o0 + o'*u1/o1 + o'*u2/o2;
std::pair<double,double> MultiVarEncodingDistribution::combineNormalDistributions(const std::vector<std::pair<double,double>>& _dists)
{
	std::pair<double,double> combinedDist;

	// a = o0*o1*o2*...*on
	// b = a/o0 + a/o1 + a/o2 + ... + a/on

	double a = 1;
	for(auto dist : _dists)
	{
		if(dist.second == 0)
			return dist;

		a *= dist.second;
	}

	double b = 0;
	for(auto dist : _dists)
	{
		combinedDist.first += dist.first*a/dist.second;
		b += a/dist.second;
	}

	combinedDist.first /= b;
	combinedDist.second = a/b;

	return combinedDist;
}

void MultiVarEncodingDistribution::encode(ArithmeticEncoder& _encoder, const std::vector<std::pair<double, double>>& _dists, unsigned char _v)
{
	std::pair<double, double> combinedDist = combineNormalDistributions(_dists);

	if(combinedDist.second != 0)
	{
		LaplaceEncodingDistribution encodeDist(combinedDist.first, combinedDist.second);
		encodeDist.encode(_encoder, _v);
	}
}
