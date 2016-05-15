/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#include "VarDistribution.hpp"

#include <cmath>

#include "ArithmeticEncoder.hpp"
#include "ArithmeticDecoder.hpp"
#include "LaplaceEncodingDistribution.hpp"


template <class TInt, bool SIGN_BIT, int MANTISSA_BITS, int EXPONENT_BITS, bool ROUND = true>
TInt toCompactFp(double _fp)
{
	int exponent;
	double mantissa = std::abs(std::frexp(_fp, &exponent));

	TInt result;
	if(exponent > (static_cast<TInt>(1)<<EXPONENT_BITS) - 1)
		result = ~((~static_cast<TInt>(0))<<(EXPONENT_BITS + MANTISSA_BITS));
	else if(exponent < 0)
		result = 0;
	else
	{
		result = std::max(mantissa - 0.5, 0.0) * std::exp2(1+MANTISSA_BITS) + (ROUND?0.5:0);
		result |= static_cast<TInt>(exponent)<<MANTISSA_BITS;
	}

	result |= (SIGN_BIT && _fp < 0)<<(EXPONENT_BITS + MANTISSA_BITS);

	return result;
}

template <class TInt, bool SIGN_BIT, int MANTISSA_BITS, int EXPONENT_BITS>
double fromCompactFp(TInt _fp)
{
	static const TInt MANTISSA_MASK = ~(((~0)>>MANTISSA_BITS)<<MANTISSA_BITS);
	static const TInt EXPONENT_MASK = ~(((~0)>>EXPONENT_BITS)<<EXPONENT_BITS);

	TInt exponent = (_fp >> MANTISSA_BITS) & EXPONENT_MASK;
	TInt mantissa = (_fp & MANTISSA_MASK) | (1<<MANTISSA_BITS);

	double result = mantissa * std::exp2(exponent - MANTISSA_BITS - 1);

    if(SIGN_BIT && (1&(_fp>>(EXPONENT_BITS + MANTISSA_BITS))))
        result = -result;

	return result;
}

double cubicInterpolate(double y0, double y1, double y2, double y3, double mu)
{
   double a0,a1,a2,a3,mu2;

   mu2 = mu*mu;
   a0 = y3 - y2 - y0 + y1;
   a1 = y0 - y1 - a0;
   a2 = y2 - y0;
   a3 = y1;

   return(a0*mu*mu2+a1*mu2+a2*mu+a3);
}

double linearInterpolate(double y1, double y2, double mu)
{
   return y1*(1-mu) + y2*mu;
}


void VarDistribution::addPoint(int _i, double _sigma)
{
	_sigma = fromCompactFp<unsigned char, false, 4, 4>(
		toCompactFp<unsigned char, false, 4, 4>(_sigma*std::exp2(3))
			)*std::exp2(-3);

	this->curvePoints.push_back({_i, _sigma});
}

std::pair<double, double> VarDistribution::getDist(unsigned char _val) const
{
	assert(!this->curvePoints.empty());

	int i = 0;
	for(; i < this->curvePoints.size() && this->curvePoints[i].first <= _val; ++i);

	std::pair<int, double> y0 = this->curvePoints[std::max(i-2, 0)];
	std::pair<int, double> y1 = this->curvePoints[std::max(i-1, 0)];
	std::pair<int, double> y2 = this->curvePoints[std::min(i, static_cast<int>(this->curvePoints.size())-1)];
	std::pair<int, double> y3 = this->curvePoints[std::min(i+1, static_cast<int>(this->curvePoints.size())-1)];

	if(_val <= y1.first)
		return { _val, y1.second };

	if(_val >= y2.first)
		return { _val, y2.second };

	double mu = static_cast<double>(_val-y1.first)/(y2.first - y1.first);

	assert(mu >= 0 && mu <= 1);

	return { _val, linearInterpolate(y1.second, y2.second, mu) };
}

void VarDistribution::encodeDist(std::ostream& _out) const
{
	unsigned char nCurvePoints = this->curvePoints.size();
	_out.write(reinterpret_cast<char*>(&nCurvePoints), sizeof(nCurvePoints));

	for(const std::pair<int, double>& curvePoint : this->curvePoints)
	{
		unsigned char x = curvePoint.first;
		unsigned char y = toCompactFp<unsigned char, false, 4, 4>(curvePoint.second * std::exp2(3));

		_out.write(reinterpret_cast<char*>(&x), sizeof(x));
		_out.write(reinterpret_cast<char*>(&y), sizeof(y));
	}
}

void VarDistribution::decodeDist(std::istream& _in)
{
	unsigned char nCurvePoints;
	_in.read(reinterpret_cast<char*>(&nCurvePoints), sizeof(nCurvePoints));

	this->curvePoints.resize(nCurvePoints);
	for(std::pair<int, double>& curvePoint : this->curvePoints)
	{
		unsigned char x;
		unsigned char y;

		_in.read(reinterpret_cast<char*>(&x), sizeof(x));
		_in.read(reinterpret_cast<char*>(&y), sizeof(y));

		curvePoint = {x, fromCompactFp<unsigned char, false, 4, 4>(y) * std::exp2(-3)};
	}
}

void VarDistribution::Calculator::add(int _val, unsigned char _v)
{
	++this->counts[_val][_v];
}

void VarDistribution::Calculator::calculate()
{
	this->dist = VarDistribution(this->counts);
}
