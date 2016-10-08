/*
 * Copyright 2016 Erik Crevel
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ArithmeticDecoder.hpp"

#include <cassert>

#include <iostream>


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

	this->active = true;
}

ArithmeticEncoder::Range ArithmeticDecoder::fraction()
{
	this->scaleRange();

	return ArithmeticEncoder::scaleUp(this->rangeSize, this->position);
}

void ArithmeticDecoder::decode(std::pair<ArithmeticEncoder::Range, ArithmeticEncoder::Range> _range)
{
	assert(this->active);

	ArithmeticEncoder::DoubleRange start = ArithmeticEncoder::scaleDown(this->rangeSize, _range.first);
	this->position -= start;
	this->rangeSize = ArithmeticEncoder::scaleDown(this->rangeSize, _range.second) - start;
}
