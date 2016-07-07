/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef ARITHMETICENCODER_HPP
#define ARITHMETICENCODER_HPP

#include <utility>
#include <cstdint>
#include <limits>
#include <ostream>
#include <deque>


class ArithmeticEncoder
{
friend class ArithmeticDecoder;

public:
	typedef std::uint32_t Range;
	typedef std::uint_fast64_t DoubleRange;

	typedef std::pair<Range, Range> RangePair;

	static const Range        RANGE_MAX = std::numeric_limits<Range>::max();
	static const DoubleRange  MIN_RANGE_SIZE = 256;

private:
	std::ostream&             output;
	std::size_t               size_;

	static const int          LAST_BYTE_SHIFT = std::numeric_limits<Range>::digits-8;
	RangePair                 range;

	std::deque<unsigned char> writeBytes;
	bool                      writeBuffered;
	unsigned char             bufferedEncodeByte;

	bool                      active;


	static Range scaleDown(Range _rMax, Range _r);
	static Range scaleUp(Range _rMax, Range _r);

	void scaleRange();
	void writeFixedBytes();
	Range removeOverflow(DoubleRange _pos);

public:
	ArithmeticEncoder(std::ostream& _output) :
		output(_output),
		size_{0},
		range{0, RANGE_MAX},
		writeBuffered{false},
		active{true}
	{}

	std::size_t size() const { return this->size_; }

	void encode(RangePair _range);

	bool isOpen() { return this->active; }
	void close();
};

#endif // ARITHMETICENCODER_HPP
