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
	std::pair<TRange, TRange>      range;

	bool                           active;

	TRange                         position;

public:
	ArithmeticDecoder(std::istream& _input, Distribution<TEncode, TRange>& _distribution, std::size_t _size) :
		input(_input),
		size_{_size},
		distribution(_distribution),
		range{0, RANGE_MAX},
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
			this->distribution.getValue(
				(static_cast<long long>(this->position-this->range.first)*RANGE_MAX)/this->range.second);

		TEncode decodedValue = v.first;
		std::pair<TRange, TRange> newRange = v.second;

		this->range =
		{
			this->range.first
			+ (static_cast<long long>(newRange.first) * this->range.second)/RANGE_MAX,
			(static_cast<long long>(newRange.second) * this->range.second)/RANGE_MAX
		};

		if(this->range.second <= RANGE_MAX>>8)
		{
			TRange positionHighByte = this->position & (RANGE_MAX<<LAST_BYTE_SHIFT);

			this->range.first -= positionHighByte;

			this->range.first <<= 8;
			this->range.second <<= 8;

			unsigned char readByte;
			this->input.get(reinterpret_cast<char&>(readByte));

			this->position = (this->position<<8) + readByte;
		}

		return decodedValue;
	}
};

#endif // ARITHMETICDECODER_HPP
