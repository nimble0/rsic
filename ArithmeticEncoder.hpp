/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef ARITHMETICENCODER_HPP
#define ARITHMETICENCODER_HPP

#include <iostream>
#include <ostream>
#include <utility>
#include <limits>
#include <cassert>
#include "Distribution.hpp"


template <class TEncode, class TRange>
class ArithmeticEncoder
{
	std::ostream&                  output;
	std::size_t                    size_;

	Distribution<TEncode, TRange>& distribution;

	static const int               LAST_BYTE_SHIFT = std::numeric_limits<TRange>::digits-8;
	static const TRange            RANGE_MAX = std::numeric_limits<TRange>::max();
	std::pair<TRange, TRange>      range;
	TRange                         rangeSize;

	bool                           writeBuffered;
	unsigned char                  bufferedEncodeByte;

	bool                           active;

public:
	ArithmeticEncoder(std::ostream& _output, Distribution<TEncode, TRange>& _distribution) :
		output(_output),
		size_{0},
		distribution(_distribution),
		range{0, RANGE_MAX},
		rangeSize{RANGE_MAX},
		writeBuffered{false},
		active{true}
	{}

	std::size_t size() const { return this->size_; }

	void encode(TEncode _v)
	{
		assert(this->active);

		std::pair<TRange, TRange> newRange = this->distribution.getRange(_v);

		unsigned long long start = this->range.first
			+ (this->range.second*static_cast<unsigned long long>(newRange.first))/RANGE_MAX;
		TRange end = (this->range.second*static_cast<unsigned long long>(newRange.second))/RANGE_MAX;

		if(start > RANGE_MAX)
		{
			++this->bufferedEncodeByte;

			start -= RANGE_MAX+1;
		}

		this->range = {start, end};

		assert(this->range.second > 0);

		// Scale up range if it has become too small
		while(this->range.second <= RANGE_MAX>>8)
		{
			if(this->writeBuffered)
				this->output.put(this->bufferedEncodeByte);
			else
				this->writeBuffered = true;

			this->bufferedEncodeByte = this->range.first>>LAST_BYTE_SHIFT;

			this->range.first -= this->bufferedEncodeByte<<LAST_BYTE_SHIFT;

			this->range.first <<= 8;
			this->range.second <<= 8;
		}

		++this->size_;
	}

	void stopEncoding()
	{
		long long encode_ = static_cast<unsigned long long>(this->range.first)+this->range.second-1;

		if(encode_ > RANGE_MAX)
		{
			++this->bufferedEncodeByte;
			encode_ -= RANGE_MAX;
		}

		TRange encode = encode_;

		this->output.put(this->bufferedEncodeByte);

		this->active = false;
	}
};

#endif // ARITHMETICENCODER_HPP
