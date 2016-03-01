/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef ARITHMETICENCODER_HPP
#define ARITHMETICENCODER_HPP

#include <utility>
#include <cstdint>
#include <limits>
#include <ostream>


class ArithmeticEncoder
{
public:
	typedef std::uint32_t Range;
	typedef std::uint_fast64_t DoubleRange;

private:
	std::ostream&                 output;
	std::size_t                   size_;

	static const int              LAST_BYTE_SHIFT = std::numeric_limits<Range>::digits-8;
	static const Range            RANGE_MAX = std::numeric_limits<Range>::max();
	std::pair<Range, Range>       range;

	bool                          writeBuffered;
	unsigned char                 bufferedEncodeByte;

	bool                          active;

public:
	ArithmeticEncoder(std::ostream& _output) :
		output(_output),
		size_{0},
		range{0, RANGE_MAX},
		writeBuffered{false},
		active{true}
	{}

	std::size_t size() const { return this->size_; }

	void encode(std::pair<Range, Range> _range);

	void close();
};

#endif // ARITHMETICENCODER_HPP
