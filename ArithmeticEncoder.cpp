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

#include "ArithmeticEncoder.hpp"

#include <cassert>
#include <algorithm>
#include <boost/range/adaptor/reversed.hpp>


ArithmeticEncoder::Range ArithmeticEncoder::scaleDown(Range _rMax, Range _r)
{
	return (static_cast<DoubleRange>(_r) * _rMax)>>std::numeric_limits<Range>::digits;
}

ArithmeticEncoder::Range ArithmeticEncoder::scaleUp(Range _rMax, Range _r)
{
	return ((static_cast<DoubleRange>(_r)<<std::numeric_limits<Range>::digits) + RANGE_MAX - 1)/_rMax;
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

ArithmeticEncoder::Range ArithmeticEncoder::removeOverflow(DoubleRange _pos)
{
	if(_pos > RANGE_MAX)
	{
		// Add one, accounting for any overflow
		for(unsigned char& c : boost::adaptors::reverse(this->writeBytes))
			if(++c != 0)
				break;

		_pos -= RANGE_MAX+1;
	}

	return _pos;
}

void ArithmeticEncoder::encode(RangePair _range)
{
	assert(this->active);
	assert(_range.second - _range.first >= MIN_RANGE_SIZE);

	this->scaleRange();

	DoubleRange start = scaleDown(this->range.second, _range.first);
	Range size = scaleDown(this->range.second, _range.second) - start;

	assert(scaleUp(this->range.second, start) >= _range.first);
	assert(scaleUp(this->range.second, start+size-1) < _range.second);

	start += this->range.first;

	this->range = {this->removeOverflow(start), size};

	this->writeFixedBytes();

	++this->size_;
}

void ArithmeticEncoder::close()
{
	std::size_t writeBytesPreSize = this->writeBytes.size();

	this->scaleRange();

	// Scale up range so that the range encompassses the max range
	if(this->range.second < RANGE_MAX)
	{
		DoubleRange writePos = static_cast<DoubleRange>(this->range.first) + (1<<LAST_BYTE_SHIFT) - 1;

		this->writeBytes.push_back(this->removeOverflow(writePos)>>LAST_BYTE_SHIFT);
	}

	// Write padding bytes, these are important to make sure
	// encoder write sizes and decoder read sizes match
	this->writeBytes.insert(
			this->writeBytes.end(),
			std::numeric_limits<ArithmeticEncoder::Range>::digits/8
			- (this->writeBytes.size() - writeBytesPreSize),
			0);

	for(unsigned char c : this->writeBytes)
		this->output.put(c);
	this->writeBytes.clear();

	this->active = false;
}
