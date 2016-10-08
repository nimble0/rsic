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

#include "MultiVarEncodingDistribution.hpp"

#include <algorithm>


// o' = (o0*o1*o2)/(o0*o1 + o0*o2 + o1*o2)
// u' = (u0*o1*o2 + u1*o0*o2 + u2*o0*o1)/(o0*o1 + o0*o2 + o1*o2)
//    = o'*u0/o0 + o'*u1/o1 + o'*u2/o2;
std::pair<double,double> MultiVarEncodingDistribution::combineNormalDistributions(const std::vector<std::pair<double,double>>& _dists)
{
	std::pair<double,double> combinedDist;

	// a = o0*o1*o2*...*on
	// b = a/o0 + a/o1 + a/o2 + ... + a/on

	double a = 1;
	for(auto dist : _dists)
	{
		if(dist.second == 0)
			return dist;

		a *= dist.second;
	}

	double b = 0;
	for(auto dist : _dists)
	{
		combinedDist.first += dist.first*a/dist.second;
		b += a/dist.second;
	}

	combinedDist.first /= b;
	combinedDist.second = a/b;

	return combinedDist;
}
