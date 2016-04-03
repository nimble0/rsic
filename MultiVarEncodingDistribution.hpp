/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef MULTIVARENCODINGDISTRIBUTION_HPP
#define MULTIVARENCODINGDISTRIBUTION_HPP

#include <vector>
#include <utility>
#include "VarEncodingDistribution.hpp"

class ArithmeticEncoder;


class MultiVarEncodingDistribution
{
	ArithmeticEncoder& encoder;

	std::vector<VarEncodingDistribution> varDists;


	static std::pair<double,double> combineNormalDistributions(const std::vector<std::pair<double,double>>& _dists);

	void encode(const std::vector<std::pair<double, double>>& _dists, unsigned char _v);

public:
	MultiVarEncodingDistribution(ArithmeticEncoder& _encoder, std::size_t _nVars) :
		encoder(_encoder),
		varDists(_nVars, VarEncodingDistribution(encoder))
	{}

	template<class TContainer>
	void encode(const TContainer& _vars, unsigned char _v)
	{
		std::vector<std::pair<double, double>> combineDists;

		std::size_t i = 0;
		for(const auto& var : _vars)
		{
			if(var.first)
				combineDists.push_back(this->varDists[i].getDist(var.second));

			++i;
		}

		this->encode(combineDists, _v);
	}

	class Calculator
	{
		MultiVarEncodingDistribution& dist;

		std::vector<VarEncodingDistribution::Calculator> distCalculators;

	public:
		Calculator(MultiVarEncodingDistribution& _dist) :
			dist(_dist)
		{
			this->distCalculators.reserve(this->dist.varDists.size());
			for(VarEncodingDistribution& _dist : this->dist.varDists)
				this->distCalculators.emplace_back(_dist);
		}

		void add(int _var, int _varVal, unsigned char _v)
		{
			this->distCalculators[_var].add(_varVal, _v);
		}

		void calculate()
		{
			for(VarEncodingDistribution::Calculator& calculator : this->distCalculators)
				calculator.calculate();
		}
	};
};

#endif // MULTIVARENCODINGDISTRIBUTION_HPP
