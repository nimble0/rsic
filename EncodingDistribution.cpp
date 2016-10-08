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
