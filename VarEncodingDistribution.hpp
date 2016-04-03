/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef VARENCODINGDISTRIBUTION_HPP
#define VARENCODINGDISTRIBUTION_HPP

#include <vector>
#include <tuple>
#include <utility>

class ArithmeticEncoder;


class VarEncodingDistribution
{
	ArithmeticEncoder& encoder;

	std::vector<std::pair<int, double>> curvePoints;


	static double cubicInterpolate(double y0, double y1, double y2, double y3, double mu);


public:
	VarEncodingDistribution(ArithmeticEncoder& _encoder) :
		encoder(_encoder)
	{}

	std::pair<double, double> getDist(unsigned char _val);

	void encode(int _val, unsigned char _v);

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
