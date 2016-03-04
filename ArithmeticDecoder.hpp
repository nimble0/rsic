/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef ARITHMETICDECODER_HPP
#define ARITHMETICDECODER_HPP

#include <istream>
#include <utility>
#include <cstdint>
#include <limits>


class ArithmeticDecoder
{
public:
	typedef std::uint32_t Range;
	typedef std::uint_fast64_t DoubleRange;

	static const Range            RANGE_MAX = std::numeric_limits<Range>::max();

private:
	std::istream&                 input;
	std::size_t                   size_;

	static const int              LAST_BYTE_SHIFT = std::numeric_limits<Range>::digits-8;
	Range                         position;
	Range                         rangeSize;

	bool                          active;


public:
	ArithmeticDecoder(std::istream& _input, std::size_t _size) :
		input(_input),
		size_{_size},
		position{0},
		rangeSize{RANGE_MAX},
		active{true}
	{}

	void open();

	// Returns numerator where denominator is RANGE_MAX
	// Fraction represents different values according to the distribution used
	Range fraction()
	{
		return (static_cast<DoubleRange>(this->position)*RANGE_MAX)/this->rangeSize;
	}

	void decode(std::pair<Range, Range> _range);
};

#endif // ARITHMETICDECODER_HPP
