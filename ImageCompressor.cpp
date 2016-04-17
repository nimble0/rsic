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

	int skipLayers = 3;
	int divisionSize = 64;

	int layers = std::ceil(std::log2(std::max(this->image.width()-1, this->image.height()-1)));

	int scale = 1 << (layers - skipLayers);
	for(std::size_t y = 0; y < this->image.height(); y += scale)
		for(std::size_t x = 0; x < this->image.width(); x += scale)
		{
			unsigned char rawColour = this->image.get(x,y).r();
			_output.write(reinterpret_cast<char*>(&rawColour), sizeof(rawColour));
		}

	for(int layer = layers - skipLayers; layer > 0; --layer)
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

	int skipLayers = 3;
	int divisionSize = 64;

	int layers = std::ceil(std::log2(std::max(this->image.width()-1, this->image.height()-1)));

	int scale = 1 << (layers - skipLayers);
	for(std::size_t y = 0; y < this->image.height(); y += scale)
		for(std::size_t x = 0; x < this->image.width(); x += scale)
		{
			unsigned char rawColour;
			_input.read(reinterpret_cast<char*>(&rawColour), sizeof(rawColour));

			this->image.set(x, y, RgbColour(rawColour, 0, 0));
		}

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

				std::streampos preEncodeStreamPos = _input.tellg();
				layerCompressor.decompress(_input);

				std::cout<<"layer size="<<(_input.tellg()-preEncodeStreamPos)<<std::endl;
			}
	}
}
