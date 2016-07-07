/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef ENCODINGDISTRIBUTION_HPP
#define ENCODINGDISTRIBUTION_HPP

#include <cassert>
#include <cmath>
#include <algorithm>
#include "ArithmeticEncoder.hpp"
#include "ArithmeticDecoder.hpp"


class EncodingDistribution
{
public:
	typedef ArithmeticEncoder::Range          Range;
	typedef ArithmeticEncoder::DoubleRange    DoubleRange;

	static const Range        RANGE_MAX = ArithmeticEncoder::RANGE_MAX;
	static const DoubleRange  MIN_RANGE_SIZE = ArithmeticEncoder::MIN_RANGE_SIZE;

public:
	~EncodingDistribution() {}

	void encode(ArithmeticEncoder& _encoder, int _v);
	int decode(ArithmeticDecoder& _decoder);

	// Ranges should be ordered.
	// For r = [ getRange(v) ), getValue(r) = v
	virtual std::pair<Range, Range> getRange(int _v) const=0;
	virtual std::pair<
		int,
		std::pair<Range, Range>>    getValue(Range _v) const=0;
	virtual void                    update(int _v) {};


	template <class TContainer>
	double encodeSize(const TContainer& _data, int _offset = 0)
	{
		double size = 0;

		for(int i = 0; i < _data.size(); ++i)
		{
			int count = _data[i];

			auto range = this->getRange(i + _offset);

			double bitsPer = std::log2(ArithmeticEncoder::RANGE_MAX/static_cast<double>(range.second - range.first));

			size += count*bitsPer;
		}

		return size;
	}

	template <class TContainer>
	static double encodeSizeLimit(const TContainer& _data)
	{
		double size = 0;

		int total = std::accumulate(_data.begin(), _data.end(), 0);

		for(int count : _data)
			if(count > 0)
				size += count*std::log2(static_cast<double>(total)/count);

		return size;
	}
};

#endif // ENCODINGDISTRIBUTION_HPP
