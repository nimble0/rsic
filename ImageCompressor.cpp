/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#include "ImageCompressor.hpp"

#include <utility>
#include <cmath>

#include "ImageLayerCompressor.hpp"
#include "RgbColour.hpp"
#include "Image.hpp"

#include <iostream>


void ImageCompressor::compress(std::ostream& _output) const
{
	std::size_t width = this->image.width();
	std::size_t height = this->image.height();

	_output.write(reinterpret_cast<char*>(&width), sizeof(width));
	_output.write(reinterpret_cast<char*>(&height), sizeof(height));

	int divisionSize = 64;
	int layers = std::ceil(std::log2(std::max(this->image.width()-1, this->image.height()-1)));

	for(int layer = layers - 3; layer > 0; --layer)
	{
		int scale = 1 << layer;
		int divisions = std::max((layers-layer)-divisionSize, 0);
		int layerSize = divisionSize*scale;

		for(std::pair<std::size_t, std::size_t> start = {0,0};
			start.second < this->image.height(); start.second += layerSize)
			for(start.first = 0; start.first < this->image.width(); start.first += layerSize)
			{
				std::pair<std::size_t, std::size_t> end
				{
					start.first + layerSize,
					start.second + layerSize,
				};
				ImageLayerCompressor layerCompressor(this->image, start, end, scale);


				std::streampos preEncodeStreamPos = _output.tellp();
				layerCompressor.compress(_output);

				std::cout<<"layer size="<<(_output.tellp()-preEncodeStreamPos)<<std::endl;
			}
	}
}

void ImageCompressor::decompress(std::istream& _input)
{
	std::size_t width = this->image.width();
	std::size_t height = this->image.height();

	_input.read(reinterpret_cast<char*>(&width), sizeof(width));
	_input.read(reinterpret_cast<char*>(&height), sizeof(height));

	this->image.resize(width, height);

	int divisionSize = 64;
	int layers = std::ceil(std::log2(std::max(this->image.width()-1, this->image.height()-1)));

	for(int layer = layers - 3; layer > 0; --layer)
	{
		int scale = 1 << layer;
		int divisions = std::max((layers-layer)-divisionSize, 0);
		int layerSize = divisionSize*scale;

		for(std::pair<std::size_t, std::size_t> start = {0,0};
			start.second < this->image.height(); start.second += layerSize)
			for(start.first = 0; start.first < this->image.width(); start.first += layerSize)
			{
				std::pair<std::size_t, std::size_t> end
				{
					start.first + layerSize,
					start.second + layerSize,
				};
				ImageLayerCompressor layerCompressor(this->image, start, end, scale);

				layerCompressor.decompress(_input);
			}
	}
}
