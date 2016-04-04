/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#include "VarEncodingDistribution.hpp"

#include "ArithmeticEncoder.hpp"
#include "LaplaceEncodingDistribution.hpp"


double VarEncodingDistribution::cubicInterpolate(double y0, double y1, double y2, double y3, double mu)
{
   double a0,a1,a2,a3,mu2;

   mu2 = mu*mu;
   a0 = y3 - y2 - y0 + y1;
   a1 = y0 - y1 - a0;
   a2 = y2 - y0;
   a3 = y1;

   return(a0*mu*mu2+a1*mu2+a2*mu+a3);
}

std::pair<double, double> VarEncodingDistribution::getDist(unsigned char _val) const
{
	int i = 0;
	for(; i < this->curvePoints.size() && this->curvePoints[i].first < _val; ++i);

	std::pair<int, double> y0 = this->curvePoints[std::max(i-2, 0)];
	std::pair<int, double> y1 = this->curvePoints[std::max(i-1, 0)];
	std::pair<int, double> y2 = this->curvePoints[std::min(i, static_cast<int>(this->curvePoints.size())-1)];
	std::pair<int, double> y3 = this->curvePoints[std::min(i+1, static_cast<int>(this->curvePoints.size())-1)];

	double mu = static_cast<double>(_val-y1.first)/(y2.first - y1.first);

	return {_val, cubicInterpolate(y0.second, y1.second, y2.second, y3.second, mu)};
}

void VarEncodingDistribution::encode(ArithmeticEncoder& _encoder, int _val, unsigned char _v)
{
	std::pair<double, double> dist = this->getDist(_val);

	LaplaceEncodingDistribution encodingDist(dist.first, dist.second);
	encodingDist.encode(_encoder, _v);
}

void VarEncodingDistribution::Calculator::add(int _val, unsigned char _v)
{
	int p = _v;
	int pp = p*p;

	auto& varTotals = this->totals[_val];
	++std::get<0>(varTotals);
	std::get<1>(varTotals) += p;
	std::get<2>(varTotals) += pp;
}

void VarEncodingDistribution::Calculator::calculate()
{
	const int CURVE_SEGMENT_SIZE = 256;

	this->dist.curvePoints.emplace_back();

	int n = 0;
	for(auto& total : this->totals)
		n += std::get<0>(total);

	const int segmentSize = std::min(n, CURVE_SEGMENT_SIZE);

	int i = 0;
	int segmentN = 0;
	int sumWeighedI = 0;
	double sumSigma = 0;

	for(auto& total : this->totals)
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

				this->dist.curvePoints.push_back({averageI, averageSigma});

				segmentN = 0;
				sumWeighedI = 0;
				sumSigma = 0;
			}
		}

		++i;
	}

	this->dist.curvePoints.front() = {0, this->dist.curvePoints[1].second};
	this->dist.curvePoints.push_back({i, this->dist.curvePoints.back().second});
}
