/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef VARENCODINGDISTRIBUTION_HPP
#define VARENCODINGDISTRIBUTION_HPP

#include <vector>
#include <tuple>
#include <utility>
#include <ostream>

class ArithmeticEncoder;


class VarEncodingDistribution
{
	std::vector<std::pair<int, double>> curvePoints;


	static double cubicInterpolate(double y0, double y1, double y2, double y3, double mu);


public:
	std::pair<double, double> getDist(unsigned char _val) const;

	void encode(ArithmeticEncoder& _encoder, int _val, unsigned char _v);

	class Calculator
	{
		VarEncodingDistribution& dist;

		std::vector<std::tuple<int, int, int>> totals;

	public:
		Calculator(VarEncodingDistribution& _dist) :
			dist(_dist),
			totals(256)
		{}

		void add(int _val, unsigned char _v);

		void calculate();
	};
};

#endif // VARENCODINGDISTRIBUTION_HPP
