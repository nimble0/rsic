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
