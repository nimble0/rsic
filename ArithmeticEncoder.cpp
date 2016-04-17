/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#include "ArithmeticEncoder.hpp"

#include <cassert>
#include <algorithm>
#include <boost/range/adaptor/reversed.hpp>


ArithmeticEncoder::DoubleRange ArithmeticEncoder::scaleDown(DoubleRange _rMax, DoubleRange _r)
{
	if(_rMax == RANGE_MAX)
		return _r;
	else
		return (_r * _rMax)>>std::numeric_limits<Range>::digits;
}

ArithmeticEncoder::DoubleRange ArithmeticEncoder::scaleUp(DoubleRange _rMax, DoubleRange _r)
{
	if(_rMax == RANGE_MAX)
		return _r;
	else
		return ((_r<<std::numeric_limits<Range>::digits) + RANGE_MAX - 1)/_rMax;
}


void ArithmeticEncoder::scaleRange()
{
	assert(this->range.second > 0);

	// Scale up range if it has become too small
	while(this->range.second <= RANGE_MAX>>8)
	{
		this->writeBytes.push_back(this->range.first>>LAST_BYTE_SHIFT);

		this->range.first &= ~(static_cast<Range>(255)<<LAST_BYTE_SHIFT);

		this->range.first <<= 8;
		this->range.second <<= 8;
	}
}

void ArithmeticEncoder::writeFixedBytes()
{
	if(!this->writeBytes.empty())
	{
		std::deque<unsigned char>::iterator writeIterEnd;
		if(this->writeBytes.back() <= 253)
			writeIterEnd = this->writeBytes.end()-1;
		else
		{
			auto rIterWriteEnd = std::find_if(this->writeBytes.rbegin()+1, this->writeBytes.rend(),
				[](unsigned char _c) { return _c <= 254; });
			writeIterEnd = (rIterWriteEnd != this->writeBytes.rend())
				? (rIterWriteEnd+1).base()
				: this->writeBytes.begin();
		}

		for(auto i = this->writeBytes.begin(); i != writeIterEnd; ++i)
			this->output.put(*i);
		this->writeBytes.erase(this->writeBytes.begin(), writeIterEnd);
	}
}

void ArithmeticEncoder::encode(std::pair<Range, DoubleRange> _range)
{
	assert(this->active);
	assert(_range.second - _range.first >= MIN_RANGE_SIZE);

	this->scaleRange();

	DoubleRange start = scaleDown(this->range.second, _range.first);
	DoubleRange size = scaleDown(this->range.second, _range.second) - start;

	assert(scaleUp(this->range.second, start) >= _range.first);
	assert(scaleUp(this->range.second, start+size-1) < _range.second);

	start += this->range.first;

	if(start >= RANGE_MAX)
	{
		// Add one, accounting for any overflow
		for(unsigned char& c : boost::adaptors::reverse(this->writeBytes))
			if(++c != 0)
				break;

		start -= RANGE_MAX;
	}

	this->range = {start, size};

	this->writeFixedBytes();

	++this->size_;
}

void ArithmeticEncoder::close()
{
	for(unsigned char c : this->writeBytes)
		this->output.put(c);
	this->writeBytes.clear();

	this->scaleRange();
	// Scale up range so that the range encompassses the max range
	if(this->range.second < RANGE_MAX)
		this->writeBytes.push_back((this->range.first + (1<<LAST_BYTE_SHIFT) - 1)>>LAST_BYTE_SHIFT);

	this->writeBytes.insert(this->writeBytes.end(), 4 - this->writeBytes.size(), 0);

	for(unsigned char c : this->writeBytes)
		this->output.put(c);
	this->writeBytes.clear();

	this->active = false;
}
