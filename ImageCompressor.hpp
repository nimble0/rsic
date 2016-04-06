/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef IMAGECOMPRESSOR_HPP
#define IMAGECOMPRESSOR_HPP

#include <iostream>
#include "ArithmeticEncoder.hpp"

class RgbColour;
template <class TColour>
class Image;


class ImageCompressor
{
	Image<RgbColour>& image;

public:
	ImageCompressor(Image<RgbColour>& _image) :
		image(_image)
	{}

	void compress(std::ostream& _output) const;
	void decompress(std::istream& _input);
};

#endif // IMAGECOMPRESSOR_HPP
