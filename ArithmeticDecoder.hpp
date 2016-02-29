/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef ARITHMETICDECODER_HPP
#define ARITHMETICDECODER_HPP

#include <ostream>
#include <utility>
#include <limits>
#include <cassert>
#include "Distribution.hpp"


template <class TEncode, class TRange>
class ArithmeticDecoder
{
	std::istream&                  input;
	std::size_t                    size_;

	Distribution<TEncode, TRange>& distribution;

	static const int               LAST_BYTE_SHIFT = std::numeric_limits<TRange>::digits-8;
	static const TRange            RANGE_MAX = std::numeric_limits<TRange>::max();
	TRange                         rangeSize;

	bool                           active;

	TRange                         position;

public:
	ArithmeticDecoder(std::istream& _input, Distribution<TEncode, TRange>& _distribution, std::size_t _size) :
		input(_input),
		size_{_size},
		distribution(_distribution),
		rangeSize{RANGE_MAX},
		active{true}
	{}

	void startDecoding()
	{
		this->position = 0;

		assert(std::numeric_limits<TRange>::digits%8 == 0);

		for(int i = 0; i < std::numeric_limits<TRange>::digits/8; ++i)
		{
			unsigned char readByte;
			this->input.get(reinterpret_cast<char&>(readByte));

			this->position = (this->position<<8) + readByte;
		}
	}

	TEncode decode()
	{
		std::pair<TEncode, std::pair<TRange, TRange>> v =
			this->distribution.getValue((static_cast<unsigned long long>(this->position)*RANGE_MAX)/this->rangeSize);

		TEncode decodedValue = v.first;
		std::pair<TRange, TRange> newRange = v.second;

		this->position -= (static_cast<unsigned long long>(newRange.first) * this->rangeSize)/RANGE_MAX;
		this->rangeSize = (static_cast<unsigned long long>(newRange.second) * this->rangeSize)/RANGE_MAX;

		while(this->rangeSize <= RANGE_MAX>>8)
		{
			this->rangeSize <<= 8;

			unsigned char readByte;
			this->input.get(reinterpret_cast<char&>(readByte));

			this->position = (this->position<<8) + readByte;
		}

		return decodedValue;
	}
};

#endif // ARITHMETICDECODER_HPP
