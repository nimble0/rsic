/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#include "ArithmeticEncoder.hpp"

#include <cassert>


void ArithmeticEncoder::encode(std::pair<Range, Range> _range)
{
	assert(this->active);

	DoubleRange start = this->range.first
		+ (this->range.second*static_cast<DoubleRange>(_range.first))/RANGE_MAX;
	Range end = (this->range.second*static_cast<DoubleRange>(_range.second))/RANGE_MAX;

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

		this->range.first -= static_cast<Range>(this->bufferedEncodeByte)<<LAST_BYTE_SHIFT;

		this->range.first <<= 8;
		this->range.second <<= 8;
	}

	++this->size_;
}

void ArithmeticEncoder::close()
{
	DoubleRange encode_ = static_cast<DoubleRange>(this->range.first)+this->range.second-1;

	if(encode_ > RANGE_MAX)
	{
		++this->bufferedEncodeByte;
		encode_ -= RANGE_MAX;
	}

	Range encode = encode_;

	this->output.put(this->bufferedEncodeByte);

	this->active = false;
}
