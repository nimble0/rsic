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

#ifndef COMBINEDENCODINGDISTRIBUTION_HPP
#define COMBINEDENCODINGDISTRIBUTION_HPP

#include "EncodingDistribution.hpp"

#include <memory>


class CombinedEncodingDistribution : public EncodingDistribution
{
public:
	typedef std::uint_fast32_t Proportion;
	typedef std::uint_fast64_t WorkingRange;

private:
	typedef std::pair<
		Proportion,
		std::unique_ptr<EncodingDistribution>> ProportionDist;

	std::vector<ProportionDist> dists;


	CombinedEncodingDistribution() {}

public:
	class Builder
	{
		std::vector<ProportionDist> dists;

	public:
		template <class TDist>
		void add(TDist _dist, Proportion _proportion)
		{
			this->dists.push_back(std::make_pair(
				_proportion,
				std::unique_ptr<EncodingDistribution>(new TDist(_dist))));
		}

		CombinedEncodingDistribution get()
		{
			CombinedEncodingDistribution obj;
			obj.dists.swap(this->dists);

			return obj;
		}
	};

	std::pair<Range, Range> getRange(int _v) const override;
	std::pair<int, std::pair<Range, Range>> getValue(Range _v) const override;
};

#endif // COMBINEDENCODINGDISTRIBUTION_HPP
