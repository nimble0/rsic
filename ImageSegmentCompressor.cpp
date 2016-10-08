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

#include "ImageSegmentCompressor.hpp"

#include "RgbColour.hpp"
#include "Image.hpp"

#include <iostream>

#include "ArithmeticEncoder.hpp"
#include "ArithmeticEncoder.hpp"
#include "LaplaceEncodingDistribution.hpp"
#include "NormalEncodingDistribution.hpp"


void ImageSegmentCompressor::calcDists(MultiVarEncodingDistribution& _dist, std::vector<std::pair<int, int>>& _offsets)
{
	MultiVarEncodingDistribution::Calculator distCalculator(_dist);

	std::size_t offsetI = 0;
	for(std::pair<int, int> offset : _offsets)
	{
		for(std::size_t y = this->start.second; y < this->end.second; y += this->increment.second)
			for(std::size_t x = this->start.first; x < this->end.first; x += this->increment.first)
			{
				std::pair<std::size_t, std::size_t> encodePixel(x, y);
				std::pair<std::size_t, std::size_t> varPixel(x + offset.first, y + offset.second);

				if(this->image.check(varPixel) && this->image.check(encodePixel))
					distCalculator.add(offsetI,
						this->image.get(varPixel).r(),
						this->image.get(encodePixel).r()
					);
			}

		++offsetI;
	}

	distCalculator.calculate();
}

std::vector<std::pair<bool, unsigned char>> ImageSegmentCompressor::getVarValues(
	const std::vector<std::pair<int, int>>& _offsets,
	std::pair<std::size_t, std::size_t> _i)
{
	std::vector<std::pair<bool, unsigned char>> varValues;
	for(std::pair<int, int> offset : _offsets)
	{
		std::pair<std::size_t, std::size_t> offsettedI
		{
			_i.first + offset.first,
			_i.second + offset.second
		};

		std::pair<bool, unsigned char> varValue{this->image.check(offsettedI), 0};

		if(varValue.first)
			varValue.second = this->image.get(offsettedI).r();

		varValues.push_back(varValue);
	}

	return varValues;
}

void ImageSegmentCompressor::encodePixel(
	ArithmeticEncoder& _encoder,
	MultiVarEncodingDistribution& _dist,
	const std::vector<std::pair<int, int>>& _vars,
	std::pair<std::size_t, std::size_t> _offset)
{
	if(this->image.check(_offset))
		_dist.encode(_encoder, this->getVarValues(_vars, _offset), this->image.get(_offset).r());
}

void ImageSegmentCompressor::decodePixel(
	ArithmeticDecoder& _decoder,
	MultiVarEncodingDistribution& _dist,
	const std::vector<std::pair<int, int>>& _vars,
	std::pair<std::size_t, std::size_t> _offset)
{
	if(this->image.check(_offset))
	{
		unsigned char decodedVal = _dist.decode(_decoder, this->getVarValues(_vars, _offset));

		this->image.set(_offset, RgbColour(decodedVal, decodedVal, decodedVal));
	}
}

void ImageSegmentCompressor::compress(std::ostream& _output)
{
	std::cout<<"encode layer ("<<this->start.first<<", "<<this->start.second<<")->("
		<<this->end.first<<", "<<this->end.second<<")"
		<<" scale=("<<this->increment.first<<", "<<this->increment.second<<")"<<std::endl;

	calcDists(this->dist, this->vars);

	this->dist.encodeDist(_output);

	ArithmeticEncoder encoder(_output);

	for(std::size_t y = this->start.second; y < this->end.second; y += this->increment.second)
		for(std::size_t x = this->start.first; x < this->end.first; x += this->increment.first)
			this->encodePixel(encoder, this->dist, this->vars, {x, y});

	encoder.close();
}

void ImageSegmentCompressor::decompress(std::istream& _input)
{
	std::cout<<"decode layer ("<<this->start.first<<", "<<this->start.second<<")->("
		<<this->end.first<<", "<<this->end.second<<")"
		<<" scale=("<<this->increment.first<<", "<<this->increment.second<<")"<<std::endl;

	this->dist.decodeDist(_input);

	ArithmeticDecoder decoder(_input);
	decoder.open();

	for(std::size_t y = this->start.second; y < this->end.second; y += this->increment.second)
		for(std::size_t x = this->start.first; x < this->end.first; x += this->increment.first)
			this->decodePixel(decoder, this->dist, this->vars, {x, y});
}
