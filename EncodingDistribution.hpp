/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef ENCODINGDISTRIBUTION_HPP
#define ENCODINGDISTRIBUTION_HPP

#include <utility>
#include "ArithmeticEncoder.hpp"
#include "ArithmeticDecoder.hpp"


template <class TEncode>
class EncodingDistribution
{
public:
	typedef ArithmeticEncoder::Range Range;
	typedef ArithmeticEncoder::DoubleRange DoubleRange;

	void encode(ArithmeticEncoder& _encoder, TEncode _v)
	{
		_encoder.encode(this->getRange(_v));
	}

	TEncode decode(ArithmeticDecoder& _decoder)
	{
		std::pair<TEncode, std::pair<Range, Range>> v = this->getValue(_decoder.fraction());

		assert(_decoder.fraction() >= v.second.first &&
			_decoder.fraction()-v.second.first < v.second.second);

		_decoder.decode(v.second);

		return v.first;
	}

	virtual std::pair<Range, Range> getRange(TEncode _v)=0;

	virtual std::pair<
		TEncode,
		std::pair<Range, Range>>    getValue(Range _v)=0;
};

#endif // ENCODINGDISTRIBUTION_HPP
