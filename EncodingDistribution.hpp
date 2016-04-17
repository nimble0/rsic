/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef ENCODINGDISTRIBUTION_HPP
#define ENCODINGDISTRIBUTION_HPP

#include <cassert>
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
		std::pair<Range, DoubleRange> range = this->getRange(_v);

		assert(this->getValue(range.first) == std::make_pair(_v, range));
		assert(this->getValue(range.second - 1) == std::make_pair(_v, range));

		_encoder.encode(range);
	}

	TEncode decode(ArithmeticDecoder& _decoder)
	{
		auto fraction = _decoder.fraction();

		std::pair<TEncode, std::pair<Range, DoubleRange>> v = this->getValue(fraction);

		assert(fraction >= v.second.first &&
			fraction < v.second.second);

		_decoder.decode(v.second);

		return v.first;
	}

	virtual std::pair<Range, DoubleRange> getRange(TEncode _v)=0;

	virtual std::pair<
		TEncode,
		std::pair<Range, DoubleRange>>    getValue(Range _v)=0;
};

#endif // ENCODINGDISTRIBUTION_HPP
