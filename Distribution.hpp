/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#ifndef DISTRIBUTION_HPP
#define DISTRIBUTION_HPP

#include <utility>


template <class TEncode, class TRange>
class Distribution
{
public:
	virtual std::pair<TRange, TRange> getRange(TEncode _v)=0;
	virtual std::pair<
		TEncode,
		std::pair<TRange, TRange>>    getValue(TRange _v)=0;
};

#endif // DISTRIBUTION_HPP
