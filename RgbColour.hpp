/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef RGBCOLOUR_HPP
#define RGBCOLOUR_HPP

#include <cstdint>

#include <Magick++.h>


class RgbColour
{
	std::uint_fast8_t r_, g_, b_;

public:
    RgbColour() :
		r_{0},
		g_{0},
		b_{0}
	{}
    RgbColour(const RgbColour& _other) :
		r_{_other.r_},
		g_{_other.g_},
		b_{_other.b_}
	{}
    RgbColour(std::uint_fast8_t _r, std::uint_fast8_t _g, std::uint_fast8_t _b) :
		r_{_r},
		g_{_g},
		b_{_b}
	{}
    RgbColour(const Magick::ColorRGB& _imColour) :
		r_{static_cast<std::uint_fast8_t>(_imColour.red()*255)},
		g_{static_cast<std::uint_fast8_t>(_imColour.red()*255)},
		b_{static_cast<std::uint_fast8_t>(_imColour.red()*255)}
	{}

	std::uint_fast8_t r() { return this->r_; }
	std::uint_fast8_t g() { return this->g_; }
	std::uint_fast8_t b() { return this->b_; }
};

#endif // RGBCOLOUR_HPP
