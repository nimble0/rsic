/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#include "ArithmeticDecoder.hpp"

#include <cassert>


void ArithmeticDecoder::scaleRange()
{
	while(this->rangeSize <= ArithmeticEncoder::RANGE_MAX>>8)
	{
		this->rangeSize <<= 8;

		unsigned char readByte;
		this->input.get(reinterpret_cast<char&>(readByte));

		this->position = (this->position<<8) + readByte;
	}
}

void ArithmeticDecoder::open()
{
	assert(std::numeric_limits<ArithmeticEncoder::Range>::digits%8 == 0);

	for(int i = 0; i < std::numeric_limits<ArithmeticEncoder::Range>::digits/8; ++i)
	{
		unsigned char readByte;
		this->input.get(reinterpret_cast<char&>(readByte));

		this->position = (this->position<<8) + readByte;
	}
}

ArithmeticEncoder::Range ArithmeticDecoder::fraction()
{
	this->scaleRange();

	return ArithmeticEncoder::scaleUp(this->rangeSize, this->position);
}

void ArithmeticDecoder::decode(std::pair<ArithmeticEncoder::Range, ArithmeticEncoder::DoubleRange> _range)
{
	ArithmeticEncoder::DoubleRange start = ArithmeticEncoder::scaleDown(this->rangeSize, _range.first);
	this->position -= start;
	this->rangeSize = ArithmeticEncoder::scaleDown(this->rangeSize, _range.second) - start;
}
