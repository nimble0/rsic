/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#include "EncodingDistribution.hpp"


void EncodingDistribution::encode(ArithmeticEncoder& _encoder, int _v)
{
	std::pair<Range, Range> range = this->getRange(_v);

	assert(this->getValue(range.first) == std::make_pair(_v, range));
	assert(this->getValue(range.second - 1) == std::make_pair(_v, range));

	_encoder.encode(range);

	this->update(_v);
}

int EncodingDistribution::decode(ArithmeticDecoder& _decoder)
{
	auto fraction = _decoder.fraction();

	std::pair<int, std::pair<Range, Range>> v = this->getValue(fraction);

	assert(fraction >= v.second.first &&
		fraction < v.second.second);

	_decoder.decode(v.second);

	this->update(v.first);

	return v.first;
}
