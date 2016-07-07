/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef MULTIVARENCODINGDISTRIBUTION_HPP
#define MULTIVARENCODINGDISTRIBUTION_HPP

#include <vector>
#include <utility>
#include "VarDistribution.hpp"
#include "LaplaceEncodingDistribution.hpp"
#include "UniformEncodingDistribution.hpp"
#include "CombinedEncodingDistribution.hpp"

class ArithmeticEncoder;


class MultiVarEncodingDistribution
{
	std::vector<VarDistribution> varDists;


	static std::pair<double,double> combineNormalDistributions(const std::vector<std::pair<double,double>>& _dists);

public:
	MultiVarEncodingDistribution(std::size_t _nVars) :
		varDists(_nVars)
	{}

	template<class TContainer>
	void encode(ArithmeticEncoder& _encoder, const TContainer& _vars, unsigned char _v)
	{
		std::vector<std::pair<double, double>> combineDists;

		std::size_t i = 0;
		for(const auto& var : _vars)
		{
			if(var.first)
				combineDists.push_back(this->varDists[i].getDist(var.second));

			++i;
		}


		std::pair<double, double> combinedDist = combineNormalDistributions(combineDists);

		if(combinedDist.second != 0)
		{
			std::pair<int, int> encodeRange{0, 256};

			CombinedEncodingDistribution::Builder distBuilder;
			distBuilder.add(LaplaceEncodingDistribution(encodeRange, combinedDist.first, combinedDist.second), 65534);
			distBuilder.add(UniformEncodingDistribution(encodeRange), 1);

			distBuilder.get().encode(_encoder, _v);
		}
	}

	template<class TContainer>
	unsigned char decode(ArithmeticDecoder& _decoder, const TContainer& _vars)
	{
		std::vector<std::pair<double, double>> combineDists;

		std::size_t i = 0;
		for(const auto& var : _vars)
		{
			if(var.first)
				combineDists.push_back(this->varDists[i].getDist(var.second));

			++i;
		}


		std::pair<double, double> combinedDist = combineNormalDistributions(combineDists);

		if(combinedDist.second != 0)
		{
			std::pair<int, int> encodeRange{0, 256};

			CombinedEncodingDistribution::Builder distBuilder;
			distBuilder.add(LaplaceEncodingDistribution(encodeRange, combinedDist.first, combinedDist.second), 65534);
			distBuilder.add(UniformEncodingDistribution(encodeRange), 1);

			return distBuilder.get().decode(_decoder);
		}
		else
			return combinedDist.first;
	}

	void encodeDist(std::ostream& _out) const
	{
		for(const VarDistribution& dist : this->varDists)
			dist.encodeDist(_out);
	}

	void decodeDist(std::istream& _in)
	{
		for(VarDistribution& dist : this->varDists)
			dist.decodeDist(_in);
	}

	class Calculator
	{
		MultiVarEncodingDistribution& dist;

		std::vector<VarDistribution::Calculator> distCalculators;

	public:
		Calculator(MultiVarEncodingDistribution& _dist) :
			dist(_dist)
		{
			this->distCalculators.reserve(this->dist.varDists.size());
			for(VarDistribution& _dist : this->dist.varDists)
				this->distCalculators.emplace_back(_dist);
		}

		void add(int _var, int _varVal, unsigned char _v)
		{
			this->distCalculators[_var].add(_varVal, _v);
		}

		void calculate()
		{
			for(VarDistribution::Calculator& calculator : this->distCalculators)
				calculator.calculate();
		}
	};
};

#endif // MULTIVARENCODINGDISTRIBUTION_HPP
