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
