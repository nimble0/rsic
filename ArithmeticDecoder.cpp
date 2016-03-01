/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#include "ArithmeticDecoder.hpp"

#include <cassert>


void ArithmeticDecoder::open()
{
	assert(std::numeric_limits<Range>::digits%8 == 0);

	for(int i = 0; i < std::numeric_limits<Range>::digits/8; ++i)
	{
		unsigned char readByte;
		this->input.get(reinterpret_cast<char&>(readByte));

		this->position = (this->position<<8) + readByte;
	}
}

void ArithmeticDecoder::decode(std::pair<Range, Range> _range)
{
	this->position -= (static_cast<DoubleRange>(_range.first) * this->rangeSize)/RANGE_MAX;
	this->rangeSize = (static_cast<DoubleRange>(_range.second) * this->rangeSize)/RANGE_MAX;

	while(this->rangeSize <= RANGE_MAX>>8)
	{
		this->rangeSize <<= 8;

		unsigned char readByte;
		this->input.get(reinterpret_cast<char&>(readByte));

		this->position = (this->position<<8) + readByte;
	}
}
