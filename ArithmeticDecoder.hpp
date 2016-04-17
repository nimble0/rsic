/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef ARITHMETICDECODER_HPP
#define ARITHMETICDECODER_HPP

#include <istream>

#include "ArithmeticEncoder.hpp"


class ArithmeticDecoder
{
	std::istream&                  input;

	ArithmeticEncoder::Range       position;
	ArithmeticEncoder::DoubleRange rangeSize;

	bool                           active;


	void                           scaleRange();

public:
	ArithmeticDecoder(std::istream& _input) :
		input(_input),
		position{0},
		rangeSize{ArithmeticEncoder::RANGE_MAX},
		active{true}
	{}

	void open();

	// Returns numerator where denominator is RANGE_MAX
	// Fraction represents different values according to the distribution used
	ArithmeticEncoder::Range fraction();

	void decode(std::pair<ArithmeticEncoder::Range, ArithmeticEncoder::DoubleRange> _range);
};

#endif // ARITHMETICDECODER_HPP
