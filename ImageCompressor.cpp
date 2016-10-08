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

#include "ImageCompressor.hpp"

#include <utility>
#include <cmath>

#include "ImageSegmentCompressor.hpp"

#include <iostream>


void ImageCompressor::compressLayer(
	std::ostream& _output,
	std::pair<std::size_t, std::size_t> _increment,
	std::pair<std::size_t, std::size_t> _startOffset,
	int _layerSize,
	const std::vector<std::pair<int, int>>& _vars) const
{
	this->processLayer(
		_startOffset,
		_layerSize,
		[&](std::pair<std::size_t, std::size_t> _start, std::pair<std::size_t, std::size_t> _end)
		{
			ImageSegmentCompressor layerCompressor(
				this->image,
				_start,
				_end,
				_increment,
				_vars);
			std::streampos preProcessStreamPos = _output.tellp();
			layerCompressor.compress(_output);

			std::cout<<"layer size="<<(_output.tellp()-preProcessStreamPos)<<std::endl;
		}
	);
}

void ImageCompressor::decompressLayer(
	std::istream& _input,
	std::pair<std::size_t, std::size_t> _increment,
	std::pair<std::size_t, std::size_t> _startOffset,
	int _layerSize,
	const std::vector<std::pair<int, int>>& _vars) const
{
	this->processLayer(
		_startOffset,
		_layerSize,
		[&](std::pair<std::size_t, std::size_t> _start, std::pair<std::size_t, std::size_t> _end)
		{
			ImageSegmentCompressor layerCompressor(
				this->image,
				_start,
				_end,
				_increment,
				_vars);
			std::streampos preProcessStreamPos = _input.tellg();
			layerCompressor.decompress(_input);

			std::cout<<"layer size="<<(_input.tellg()-preProcessStreamPos)<<std::endl;
		}
	);
}

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
		int halfScale = scale >> 1;
		int layerSize = divisionSize*scale;

		this->compressLayer(
			_output,
			{scale, scale},
			{halfScale, 0},
			layerSize,
			scaleVars(aVars, halfScale));

		this->compressLayer(
			_output,
			{halfScale, scale},
			{0, halfScale},
			layerSize,
			scaleVars(bVars, halfScale));
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

	for(int layer = layers - skipLayers; layer > 0; --layer)
	{
		int scale = 1 << layer;
		int halfScale = scale >> 1;
		int layerSize = divisionSize*scale;

		this->decompressLayer(
			_input,
			{scale, scale},
			{halfScale, 0},
			layerSize,
			scaleVars(aVars, halfScale));

		this->decompressLayer(
			_input,
			{halfScale, scale},
			{0, halfScale},
			layerSize,
			scaleVars(bVars, halfScale));
	}
}
