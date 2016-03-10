/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef MULTIVARENCODINGDISTRIBUTION_HPP
#define MULTIVARENCODINGDISTRIBUTION_HPP

#include <vector>
#include <utility>
#include <boost/iterator/iterator_concepts.hpp>

class ArithmeticEncoder;


class MultiVarEncodingDistribution
{
	ArithmeticEncoder& encoder;

	std::vector<std::vector<std::pair<double, double>>> dists;


	static std::pair<double,double> combineNormalDistributions(const std::vector<std::pair<double,double>>& _dists);

	void encode(const std::vector<std::pair<double, double>>& _dists, unsigned char _v);

public:
	MultiVarEncodingDistribution(ArithmeticEncoder& _encoder) :
		encoder(_encoder)
	{}

	template<class TContainer>
	void encode(const TContainer& _vars, unsigned char _v)
	{
		std::vector<std::pair<double, double>> combineDists;

		std::size_t i = 0;
		for(const auto& var : _vars)
		{
			if(var.first)
				combineDists.push_back(this->dists[i][var.second]);

			++i;
		}

		this->encode(combineDists, _v);
	}


	class Calculator
	{
		MultiVarEncodingDistribution& dist;

		std::vector<std::vector<std::tuple<int,int,int>>> totals;

	public:
		Calculator(MultiVarEncodingDistribution& _dist, int _nVars) :
			dist(_dist),
			totals(_nVars, std::vector<std::tuple<int,int,int>>(256))
		{}

		void add(int _var, int _varVal, unsigned char _v);

		void calculate();
	};
};

#endif // MULTIVARENCODINGDISTRIBUTION_HPP
