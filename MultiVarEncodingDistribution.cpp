/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#include "MultiVarEncodingDistribution.hpp"

#include "ArithmeticEncoder.hpp"
#include "NormalEncodingDistribution.hpp"


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

void MultiVarEncodingDistribution::encode(const std::vector<std::pair<double, double>>& _dists, unsigned char _v)
{
	std::pair<double, double> combinedDist = combineNormalDistributions(_dists);

	if(combinedDist.second != 0)
	{
		NormalEncodingDistribution encodeDist(encoder, combinedDist.first, combinedDist.second);
		encodeDist.encode(_v);
	}
}

void MultiVarEncodingDistribution::Calculator::add(int _var, int _varVal, unsigned char _v)
{
	int p = _v;
	double pp = p*p;

	auto& varTotals = this->totals[_var][_varVal];
	++std::get<0>(varTotals);
	std::get<1>(varTotals) += p;
	std::get<2>(varTotals) += pp;
}

void MultiVarEncodingDistribution::Calculator::calculate()
{
	for(auto var : this->totals)
	{
		this->dist.dists.emplace_back();
		auto& varDists = this->dist.dists.back();

		for(auto& total : var)
		{
			if(std::get<0>(total) != 0)
			{
				double mu = static_cast<double>(std::get<1>(total))/std::get<0>(total);
				double sigma = std::sqrt(static_cast<double>(std::get<2>(total))/std::get<0>(total) - mu*mu);

				varDists.push_back({ mu, sigma });
			}
			else
				varDists.push_back({ 0, 0 });
		}
	}
}
