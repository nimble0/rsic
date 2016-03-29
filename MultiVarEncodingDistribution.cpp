/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#include "MultiVarEncodingDistribution.hpp"

#include <algorithm>

#include "ArithmeticEncoder.hpp"
#include "NormalEncodingDistribution.hpp"
#include "LaplaceEncodingDistribution.hpp"


double MultiVarEncodingDistribution::cubicInterpolate(double y0, double y1, double y2, double y3, double mu)
{
   double a0,a1,a2,a3,mu2;

   mu2 = mu*mu;
   a0 = y3 - y2 - y0 + y1;
   a1 = y0 - y1 - a0;
   a2 = y2 - y0;
   a3 = y1;

   return(a0*mu*mu2+a1*mu2+a2*mu+a3);
}

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

std::pair<double, double> MultiVarEncodingDistribution::varDist(int _var, unsigned char _val)
{
	const auto& curvePoints = this->varCurvePoints[_var];

	int i = 0;
	for(; i < curvePoints.size() && curvePoints[i].first < _val; ++i);

	std::pair<int, double> y0 = curvePoints[std::max(i-2, 0)];
	std::pair<int, double> y1 = curvePoints[std::max(i-1, 0)];
	std::pair<int, double> y2 = curvePoints[std::min(i, static_cast<int>(curvePoints.size())-1)];
	std::pair<int, double> y3 = curvePoints[std::min(i+1, static_cast<int>(curvePoints.size())-1)];

	double mu = static_cast<double>(_val-y1.first)/(y2.first - y1.first);

	return {_val, cubicInterpolate(y0.second, y1.second, y2.second, y3.second, mu)};
}

void MultiVarEncodingDistribution::encode(const std::vector<std::pair<double, double>>& _dists, unsigned char _v)
{
	std::pair<double, double> combinedDist = combineNormalDistributions(_dists);

	if(combinedDist.second != 0)
	{
		LaplaceEncodingDistribution encodeDist(encoder, combinedDist.first, combinedDist.second);
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
	const int CURVE_SEGMENT_SIZE = 256;


	int varI = 0;
	for(auto var : this->totals)
	{
		int n = 0;
		for(auto& total : var)
			n += std::get<0>(total);

		const int segmentSize = std::min(n, CURVE_SEGMENT_SIZE);

		this->dist.varCurvePoints.emplace_back();
		std::vector<std::pair<int, double>>& curvePoints = this->dist.varCurvePoints.back();
		curvePoints.emplace_back();

		int i = 0;
		int segmentN = 0;
		int sumWeighedI = 0;
		double sumSigma = 0;

		for(auto& total : var)
		{
			if(std::get<0>(total) != 0)
			{
				double mu = static_cast<double>(std::get<1>(total))/std::get<0>(total);
				double sigma = std::sqrt(static_cast<double>(std::get<2>(total))/std::get<0>(total) - mu*mu);

				segmentN += std::get<0>(total);
				sumWeighedI += i * std::get<0>(total);
				sumSigma += sigma * std::get<0>(total);

				if(segmentN >= segmentSize)
				{
					int averageI = sumWeighedI/segmentN;
					double averageSigma = sumSigma/segmentN;

					curvePoints.push_back({averageI, averageSigma});

					segmentN = 0;
					sumWeighedI = 0;
					sumSigma = 0;
				}
			}

			++i;
		}

		curvePoints.front() = {0, curvePoints[1].second};
		curvePoints.push_back({i, curvePoints.back().second});
	}
}
