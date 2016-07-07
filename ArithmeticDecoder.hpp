/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef ARITHMETICDECODER_HPP
#define ARITHMETICDECODER_HPP

#include <istream>

#include "ArithmeticEncoder.hpp"


class ArithmeticDecoder
{
	std::istream&            input;

	ArithmeticEncoder::Range position;
	ArithmeticEncoder::Range rangeSize;

	bool                     active;


	void                     scaleRange();

public:
	ArithmeticDecoder(std::istream& _input) :
		input(_input),
		position{0},
		rangeSize{ArithmeticEncoder::RANGE_MAX},
		active{true}
	{}

	void open();
	bool isOpen() { return this->active; }

	// Returns numerator where denominator is RANGE_MAX
	// Fraction represents different values according to the distribution used
	ArithmeticEncoder::Range fraction();

	void decode(ArithmeticEncoder::RangePair _range);
};

#endif // ARITHMETICDECODER_HPP
