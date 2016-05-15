/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef VARDISTRIBUTION_HPP
#define VARDISTRIBUTION_HPP

#include <vector>
#include <tuple>
#include <utility>
#include <ostream>
#include <boost/multi_array.hpp>

class ArithmeticEncoder;
class ArithmeticDecoder;


class VarDistribution
{
	std::vector<std::pair<int, double>> curvePoints;

	void addPoint(int _i, double _sigma);


public:
	VarDistribution() {}
	template <class TContainer>
	VarDistribution(const TContainer& _counts);

	std::pair<double, double> getDist(unsigned char _val) const;

	void encodeDist(std::ostream& _out) const;
	void decodeDist(std::istream& _in);

	class Calculator
	{
		VarDistribution& dist;

		boost::multi_array<int,2> counts;

	public:
		Calculator(VarDistribution& _dist) :
			dist(_dist),
			counts(boost::extents[256][256])
		{}

		void add(int _val, unsigned char _v);

		void calculate();
	};
};


namespace
{

template <class TContainer>
double variance(const TContainer& _counts)
{
	int n = 0;
	double sumX = 0;
	double sumXX = 0;

	for(int x = 0; x < _counts.size(); ++x)
	{
		n += _counts[x];
		sumX += x*_counts[x];
		sumXX += x*x*_counts[x];
	}

	return (sumXX - sumX*sumX/n)/n;
}

}


template <class TContainer>
VarDistribution::VarDistribution(const TContainer& _counts)
{
	const int CURVE_SEGMENT_SIZE = 1024;

	std::vector<std::pair<int, double>> distVars(256);
	for(int varI = 0; varI < _counts.size(); ++varI)
	{
		const auto& subCounts = _counts[varI];

		int subTotal = std::accumulate(subCounts.begin(), subCounts.end(), 0);

		distVars[varI] = {subTotal, std::sqrt(variance(subCounts))};
	}

	int total = 0;
	int weightedI = 0;
	double weightedSigma = 0;
	for(int varI = 0; varI < distVars.size(); ++varI)
	{
		if(distVars[varI].first > 0)
		{
			total += distVars[varI].first;
			weightedI += distVars[varI].first * varI;
			weightedSigma += distVars[varI].first * distVars[varI].second;

			if(total >= CURVE_SEGMENT_SIZE)
			{
				this->addPoint((weightedI+total/2)/total, weightedSigma/total);

				total = 0;
				weightedI = 0;
				weightedSigma = 0;
			}
		}
	}

	if(total > 0)
		this->addPoint((weightedI+total/2)/total, weightedSigma/total);
}

#endif // VARDISTRIBUTION_HPP
