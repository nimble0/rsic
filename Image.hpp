/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <vector>
#include <string>

#include <Magick++.h>


template <class TColour>
class Image
{
	std::size_t width_, height_;
	std::vector<TColour> pixels;

public:
    Image(std::size_t _width, std::size_t _height) :
		width_{_width},
		height_{_height},
		pixels{width_*height_}
	{}

	Image(const std::string& _path)
	{
		Magick::Image image(_path);

		this->width_ = image.columns();
		this->height_ = image.rows();

		this->pixels.resize(this->width_*this->height_);

		for(std::size_t y = 0; y < this->height_; ++y)
			for(std::size_t x = 0; x < this->width_; ++x)
				this->set(x,y,TColour(image.pixelColor(x,y)));
	}

	std::size_t width() const { return this->width_; }
	std::size_t height() const { return this->height_; }

	TColour getI(std::size_t _i) const
	{
		return this->pixels[_i];
	}
	TColour get(std::size_t _x, std::size_t _y) const
	{
		return this->getI(_y * this->width_ + _x);
	}
	TColour get(std::pair<std::size_t, std::size_t> _i) const
	{
		return this->get(_i.first, _i.second);
	}

	void setI(std::size_t _i, TColour _colour)
	{
		this->pixels[_i] = _colour;
	}
	void set(std::size_t _x, std::size_t _y, TColour _colour)
	{
		this->setI(_y * this->width_ + _x, _colour);
	}
	void set(std::pair<std::size_t, std::size_t> _i, TColour _colour)
	{
		this->set(_i.first, _i.second, _colour);
	}

	bool checkI(std::size_t _i) const
	{
		return _i < this->pixels.size();
	}
	bool check(std::size_t _x, std::size_t _y) const
	{
		return _x < this->width_ && _y < this->height_;
	}
	bool check(std::pair<std::size_t, std::size_t> _i) const
	{
		return this->check(_i.first, _i.second);
	}

	void resize(std::size_t _width, std::size_t _height)
	{
		std::vector<TColour> newPixels(_width*_height);

		std::size_t copyWidth = std::min(this->width_, _width);

		for(std::size_t y = 0; y < _height; ++y)
			std::copy(&this->pixels[y * this->width_], &this->pixels[y * this->width_ + copyWidth], &newPixels[y * _width]);
			
		this->width_ = _width;
		this->height_ = _height;
		this->pixels.swap(newPixels);
	}
};

#endif // IMAGE_HPP
