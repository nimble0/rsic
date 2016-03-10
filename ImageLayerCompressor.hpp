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
		std::pair<std::size_t, std::size_t> _i);

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
		aDistVars{{-1,0}, {1,0}},
		bDistVars{{0,-1}, {0,1}},
		cDistVars{{-1,-1}, {-1,1}, {1,-1}, {1,1},},
		aDists(encoder),
		bDists(encoder),
		cDists(encoder)
	{}

	void calcDists(MultiVarEncodingDistribution& _dist, std::vector<std::pair<int, int>>& _offsets);

	void encode();
};

#endif // IMAGELAYERCOMPRESSOR_HPP
