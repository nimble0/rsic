/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#include "ImageLayerCompressor.hpp"

#include "RgbColour.hpp"
#include "Image.hpp"

#include <iostream>


void ImageLayerCompressor::calcDists(MultiVarEncodingDistribution& _dist, std::vector<std::pair<int, int>>& _offsets)
{
	MultiVarEncodingDistribution::Calculator distCalculator(_dist, _offsets.size());

	std::size_t offsetI = 0;
	for(std::pair<int, int> offset : _offsets)
	{
		std::size_t xStart = std::max(0, -offset.first);
		std::size_t xEnd = std::min(this->image.width(), this->image.width()-offset.first);
		std::size_t yStart = std::max(0, -offset.second);
		std::size_t yEnd = std::min(this->image.height(), this->image.height()-offset.second);

		for(std::size_t y = yStart; y < yEnd; ++y)
			for(std::size_t x = xStart; x < xEnd; ++x)
				distCalculator.add(offsetI,
					this->image.get(x + offset.first, y + offset.second).r(),
					this->image.get(x,y).r()
				);

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
	MultiVarEncodingDistribution& _dist,
	const std::vector<std::pair<int, int>>& _vars,
	std::pair<std::size_t, std::size_t> _i)
{
	if(this->image.check(_i))
		_dist.encode(this->getVarValues(_vars, _i), this->image.get(_i).r());
}

void ImageLayerCompressor::encode()
{
	calcDists(this->aDists, this->aDistVars);
	calcDists(this->bDists, this->bDistVars);
	calcDists(this->cDists, this->cDistVars);

	std::cout<<"encode layer ("<<this->start.first<<", "<<this->start.second<<")->("
		<<this->end.first<<", "<<this->end.second<<")"
		<<" scale="<<this->scale<<std::endl;

	for(std::size_t y = this->start.second; y < this->end.second; y += this->scale)
		for(std::size_t x = this->start.first; x < this->end.first; x += this->scale)
		{
			this->encodePixel(this->aDists, this->aDistVars, {x+this->halfScale, y});
			this->encodePixel(this->bDists, this->bDistVars, {x, y+this->halfScale});
			this->encodePixel(this->cDists, this->cDistVars, {x+this->halfScale, y+this->halfScale});
		}
}
