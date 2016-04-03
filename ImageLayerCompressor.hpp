/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef IMAGELAYERCOMPRESSOR_HPP
#define IMAGELAYERCOMPRESSOR_HPP

#include <utility>
#include <vector>
#include <cstdint>
#include "MultiVarEncodingDistribution.hpp"

class RgbColour;
template <class TColour>
class Image;
class ArithmeticEncoder;



class ImageLayerCompressor
{
	const Image<RgbColour>& image;

	ArithmeticEncoder& encoder;

	std::pair<std::size_t, std::size_t> start;
	std::pair<std::size_t, std::size_t> end;

	int scale;
	int halfScale;

	std::vector<std::pair<int, int>>
		aDistVars,
		bDistVars,
		cDistVars;

	MultiVarEncodingDistribution
		aDists,
		bDists,
		cDists;


	std::vector<std::pair<bool, unsigned char>> getVarValues(
		const std::vector<std::pair<int, int>>& _offsets,
		std::pair<std::size_t, std::size_t> _i);

	void encodePixel(
		MultiVarEncodingDistribution& _dist,
		const std::vector<std::pair<int, int>>& _vars,
		std::pair<std::size_t, std::size_t> _base,
		std::pair<std::size_t, std::size_t> _offset);

	void calcDists(MultiVarEncodingDistribution& _dist, std::pair<int, int> _encodeOffset, std::vector<std::pair<int, int>>& _offsets);

public:
	ImageLayerCompressor(
		const Image<RgbColour>& _image,
		ArithmeticEncoder& _encoder,
		std::pair<std::size_t, std::size_t> _start,
		std::pair<std::size_t, std::size_t> _end,
		int _scale
	) :
		image(_image),
		encoder(_encoder),
		start{_start},
		end{_end},
		scale{_scale},
		halfScale{scale/2},
		aDistVars{
			{0, 0},
			{this->scale, 0}},
		bDistVars{
			{0, 0},
			{0, this->scale}},
		cDistVars{
			{this->halfScale, 0},
			{this->halfScale, this->scale}},
		aDists(encoder, aDistVars.size()),
		bDists(encoder, bDistVars.size()),
		cDists(encoder, cDistVars.size())
	{}
	void encode();
};

#endif // IMAGELAYERCOMPRESSOR_HPP
