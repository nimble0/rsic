/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#include "ImageCompressor.hpp"

#include <utility>
#include <cmath>

#include "ImageLayerCompressor.hpp"
#include "RgbColour.hpp"
#include "Image.hpp"


void ImageCompressor::compress()
{
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
				ImageLayerCompressor layerCompressor(this->image, this->encoder, start, end, scale);
				layerCompressor.encode();
			}
	}

	this->encoder.close();
}
