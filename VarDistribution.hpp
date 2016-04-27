/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef VARDISTRIBUTION_HPP
#define VARDISTRIBUTION_HPP

#include <vector>
#include <tuple>
#include <utility>
#include <ostream>

class ArithmeticEncoder;
class ArithmeticDecoder;


class VarDistribution
{
	std::vector<std::pair<int, double>> curvePoints;


public:
	std::pair<double, double> getDist(unsigned char _val) const;

	void encodeDist(std::ostream& _out) const;
	void decodeDist(std::istream& _in);

	class Calculator
	{
		VarDistribution& dist;

		std::vector<std::tuple<int, int, int>> totals;

	public:
		Calculator(VarDistribution& _dist) :
			dist(_dist),
			totals(256)
		{}

		void add(int _val, unsigned char _v);

		void calculate();
	};
};

#endif // VARDISTRIBUTION_HPP
