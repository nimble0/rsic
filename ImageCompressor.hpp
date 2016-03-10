/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef IMAGECOMPRESSOR_HPP
#define IMAGECOMPRESSOR_HPP

#include <ostream>
#include "ArithmeticEncoder.hpp"

class RgbColour;
template <class TColour>
class Image;


class ImageCompressor
{
	Image<RgbColour>& image;

	ArithmeticEncoder encoder;


	void compressArea(int scale, std::pair<std::size_t, std::size_t> _start, std::pair<std::size_t, std::size_t> _end);

public:
	ImageCompressor(Image<RgbColour>& _image, std::ostream& _output) :
		image(_image),
		encoder(_output)
	{}

	void compress();
};

#endif // IMAGECOMPRESSOR_HPP
