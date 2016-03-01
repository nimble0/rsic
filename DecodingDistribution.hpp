/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef DECODINGDISTRIBUTION_HPP
#define DECODINGDISTRIBUTION_HPP

#include <utility>
#include "ArithmeticDecoder.hpp"


template <class TEncode>
class DecodingDistribution
{
	ArithmeticDecoder& decoder;

public:
	typedef ArithmeticDecoder::Range Range;
	typedef ArithmeticDecoder::DoubleRange DoubleRange;

	DecodingDistribution(ArithmeticDecoder& _decoder) :
		decoder(_decoder)
	{}

	TEncode decode()
	{
		std::pair<TEncode, std::pair<Range, Range>> v = this->getValue(this->decoder.fraction());

		this->decoder.decode(v.second);

		return v.first;
	}

	virtual std::pair<
		TEncode,
		std::pair<Range, Range>>    getValue(Range _v)=0;
};

#endif // DECODINGDISTRIBUTION_HPP
