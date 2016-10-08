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



class ImageSegmentCompressor
{
	Image<RgbColour>& image;

	std::pair<std::size_t, std::size_t> start;
	std::pair<std::size_t, std::size_t> end;

	std::pair<std::size_t, std::size_t> increment;

	std::vector<std::pair<int, int>> vars;
	MultiVarEncodingDistribution dist;


	std::vector<std::pair<bool, unsigned char>> getVarValues(
		const std::vector<std::pair<int, int>>& _offsets,
		std::pair<std::size_t, std::size_t> _i);

	void encodePixel(
		ArithmeticEncoder& _encoder,
		MultiVarEncodingDistribution& _dist,
		const std::vector<std::pair<int, int>>& _vars,
		std::pair<std::size_t, std::size_t> _offset);

	void decodePixel(
		ArithmeticDecoder& _decoder,
		MultiVarEncodingDistribution& _dist,
		const std::vector<std::pair<int, int>>& _vars,
		std::pair<std::size_t, std::size_t> _offset);

	void calcDists(MultiVarEncodingDistribution& _dist, std::vector<std::pair<int, int>>& _offsets);

public:
	ImageSegmentCompressor(
		Image<RgbColour>& _image,
		std::pair<std::size_t, std::size_t> _start,
		std::pair<std::size_t, std::size_t> _end,
		std::pair<std::size_t, std::size_t> _increment,
		std::vector<std::pair<int, int>> _vars
	) :
		image(_image),
		start{_start},
		end{_end},
		increment{_increment},
		vars(_vars),
		dist(vars.size())
	{}

	void compress(std::ostream& _output);
	void decompress(std::istream& _input);
};

#endif // IMAGELAYERCOMPRESSOR_HPP
