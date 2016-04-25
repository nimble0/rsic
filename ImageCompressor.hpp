/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef IMAGECOMPRESSOR_HPP
#define IMAGECOMPRESSOR_HPP

#include <iostream>
#include "ArithmeticEncoder.hpp"
#include "RgbColour.hpp"
#include "Image.hpp"


class ImageCompressor
{
	Image<RgbColour>& image;

	std::vector<std::pair<int, int>> aVars, bVars;

	static std::pair<std::size_t, std::size_t> offsetVector(std::pair<std::size_t, std::size_t> _a, std::pair<int, int> _b)
	{
		return {_a.first + _b.first, _a.second + _b.second};
	}

	template <class TFunc>
	void processLayer(
		std::pair<std::size_t, std::size_t> _startOffset,
		int _layerSize,
		TFunc _func) const;

	void compressLayer(
		std::ostream& _output,
		std::pair<std::size_t, std::size_t> _increment,
		std::pair<std::size_t, std::size_t> _startOffset,
		int _layerSize,
		const std::vector<std::pair<int, int>>& _vars) const;

	void decompressLayer(
		std::istream& _input,
		std::pair<std::size_t, std::size_t> _increment,
		std::pair<std::size_t, std::size_t> _startOffset,
		int _layerSize,
		const std::vector<std::pair<int, int>>& _vars) const;

	static std::vector<std::pair<int, int>> scaleVars(const std::vector<std::pair<int, int>>& _vars, int _scale)
	{
		std::vector<std::pair<int, int>> scaledVars;
		scaledVars.reserve(_vars.size());

		for(std::pair<int, int> v : _vars)
			scaledVars.push_back({v.first * _scale, v.second * _scale});

		return scaledVars;
	}

public:
	ImageCompressor(Image<RgbColour>& _image) :
		image(_image),
		aVars({{-1, 0}, {1, 0}}),
		bVars({{0, -1}, {0, 1}})
	{}

	void compress(std::ostream& _output) const;
	void decompress(std::istream& _input);
};


template <class TFunc>
void ImageCompressor::processLayer(
	std::pair<std::size_t, std::size_t> _startOffset,
	int _layerSize,
	TFunc _func) const
{
	for(std::pair<std::size_t, std::size_t> baseStart = {0,0};
	    baseStart.second < this->image.height();
	    baseStart.second += _layerSize)
		for(baseStart.first = 0;
		    baseStart.first < this->image.width();
		    baseStart.first += _layerSize)
		{
			std::pair<std::size_t, std::size_t> start
			{
				baseStart.first + _startOffset.first,
				baseStart.second + _startOffset.second
			};
			std::pair<std::size_t, std::size_t> end
			{
				start.first + _layerSize,
				start.second + _layerSize
			};

			_func(start, end);
		}
}

#endif // IMAGECOMPRESSOR_HPP
