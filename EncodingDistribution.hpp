/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef ENCODINGDISTRIBUTION_HPP
#define ENCODINGDISTRIBUTION_HPP

#include <utility>
#include "ArithmeticEncoder.hpp"


template <class TEncode>
class EncodingDistribution
{
	ArithmeticEncoder& encoder;

public:
	typedef ArithmeticEncoder::Range Range;
	typedef ArithmeticEncoder::DoubleRange DoubleRange;

	EncodingDistribution(ArithmeticEncoder& _encoder) :
		encoder(_encoder)
	{}

	void encode(TEncode _v)
	{
		this->encoder.encode(this->getRange(_v));
	}

	virtual std::pair<Range, Range> getRange(TEncode _v)=0;
};

#endif // ENCODINGDISTRIBUTION_HPP
