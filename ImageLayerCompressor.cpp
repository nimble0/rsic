/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#include "ImageLayerCompressor.hpp"

#include "RgbColour.hpp"
#include "Image.hpp"

#include <iostream>

#include "ArithmeticEncoder.hpp"
#include "ArithmeticEncoder.hpp"
#include "LaplaceEncodingDistribution.hpp"
#include "NormalEncodingDistribution.hpp"


void ImageLayerCompressor::calcDists(MultiVarEncodingDistribution& _dist, std::pair<int, int> _encodeOffset, std::vector<std::pair<int, int>>& _offsets)
{
	MultiVarEncodingDistribution::Calculator distCalculator(_dist);

	std::size_t offsetI = 0;
	for(std::pair<int, int> offset : _offsets)
	{
// 		std::size_t xStart = std::max(0, -offset.first);
// 		std::size_t xEnd = std::min(this->image.width(), this->image.width()-offset.first);
// 		std::size_t yStart = std::max(0, -offset.second);
// 		std::size_t yEnd = std::min(this->image.height(), this->image.height()-offset.second);

		for(std::size_t y = this->start.second; y < this->end.second; y += this->scale)
			for(std::size_t x = this->start.first; x < this->end.first; x += this->scale)
			{
				std::pair<std::size_t, std::size_t> encodePixel(x + _encodeOffset.first, y + _encodeOffset.second);
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

std::vector<std::pair<bool, unsigned char>> ImageLayerCompressor::getVarValues(
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

void ImageLayerCompressor::encodePixel(
	ArithmeticEncoder& _encoder,
	MultiVarEncodingDistribution& _dist,
	const std::vector<std::pair<int, int>>& _vars,
	std::pair<std::size_t, std::size_t> _base,
	std::pair<std::size_t, std::size_t> _offset)
{
	std::pair<std::size_t, std::size_t> encodeI
	{
		_base.first + _offset.first,
		_base.second + _offset.second
	};

	if(this->image.check(encodeI))
		_dist.encode(_encoder, this->getVarValues(_vars, _base), this->image.get(encodeI).r());
}

void ImageLayerCompressor::decodePixel(
	ArithmeticDecoder& _decoder,
	MultiVarEncodingDistribution& _dist,
	const std::vector<std::pair<int, int>>& _vars,
	std::pair<std::size_t, std::size_t> _base,
	std::pair<std::size_t, std::size_t> _offset)
{
	std::pair<std::size_t, std::size_t> decodeI
	{
		_base.first + _offset.first,
		_base.second + _offset.second
	};

	if(this->image.check(decodeI))
		this->image.set(decodeI, RgbColour(_dist.decode(_decoder, this->getVarValues(_vars, _base)),0,0));
}

std::size_t ImageLayerCompressor::pixelCount() const
{
	std::size_t layerWidth = this->end.first - this->start.first;
	std::size_t layerHeight = this->end.second - this->start.second;

	std::size_t aBlocks =
		((layerWidth + this->halfScale - 1)/this->scale)*
		((layerHeight + this->scale - 1)/this->scale);

	std::size_t bBlocks =
		((layerWidth + this->scale - 1)/this->scale)*
		((layerHeight + this->halfScale - 1)/this->scale);

	std::size_t cBlocks =
		((layerWidth + this->halfScale - 1)/this->scale)*
		((layerHeight + this->halfScale - 1)/this->scale);

	return aBlocks + bBlocks + cBlocks;
}

void ImageLayerCompressor::compress(std::ostream& _output)
{
	std::cout<<"encode layer ("<<this->start.first<<", "<<this->start.second<<")->("
		<<this->end.first<<", "<<this->end.second<<")"
		<<" scale="<<this->scale<<std::endl;

	ArithmeticEncoder encoder(_output);

	calcDists(this->aDists, {this->halfScale, 0}, this->aDistVars);
	calcDists(this->bDists, {0, this->halfScale}, this->bDistVars);
	calcDists(this->cDists, {this->halfScale, this->halfScale}, this->cDistVars);

	this->aDists.encodeDist(_output);
	this->bDists.encodeDist(_output);
	this->cDists.encodeDist(_output);

	for(std::size_t y = this->start.second; y < this->end.second; y += this->scale)
		for(std::size_t x = this->start.first; x < this->end.first; x += this->scale)
			this->encodePixel(encoder, this->aDists, this->aDistVars, {x, y}, {this->halfScale, 0});

	for(std::size_t y = this->start.second; y < this->end.second; y += this->scale)
		for(std::size_t x = this->start.first; x < this->end.first; x += this->scale)
			this->encodePixel(encoder, this->bDists, this->bDistVars, {x, y}, {0, this->halfScale});

	for(std::size_t y = this->start.second; y < this->end.second; y += this->scale)
		for(std::size_t x = this->start.first; x < this->end.first; x += this->scale)
			this->encodePixel(encoder, this->cDists, this->cDistVars, {x, y}, {this->halfScale, this->halfScale});

	encoder.close();
}

void ImageLayerCompressor::decompress(std::istream& _input)
{
	std::cout<<"decode layer ("<<this->start.first<<", "<<this->start.second<<")->("
		<<this->end.first<<", "<<this->end.second<<")"
		<<" scale="<<this->scale<<std::endl;

	ArithmeticDecoder decoder(_input, this->pixelCount());
	decoder.open();

	this->aDists.decodeDist(_input);
	this->bDists.decodeDist(_input);
	this->cDists.decodeDist(_input);

	for(std::size_t y = this->start.second; y < this->end.second; y += this->scale)
		for(std::size_t x = this->start.first; x < this->end.first; x += this->scale)
			this->decodePixel(decoder, this->aDists, this->aDistVars, {x, y}, {this->halfScale, 0});

	for(std::size_t y = this->start.second; y < this->end.second; y += this->scale)
		for(std::size_t x = this->start.first; x < this->end.first; x += this->scale)
			this->decodePixel(decoder, this->bDists, this->bDistVars, {x, y}, {0, this->halfScale});

	for(std::size_t y = this->start.second; y < this->end.second; y += this->scale)
		for(std::size_t x = this->start.first; x < this->end.first; x += this->scale)
			this->decodePixel(decoder, this->cDists, this->cDistVars, {x, y}, {this->halfScale, this->halfScale});
}
