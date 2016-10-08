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

#include "ArithmeticEncoder.hpp"
#include "ArithmeticDecoder.hpp"

#include "catch.hpp"

#include <random>
#include <boost/iostreams/stream.hpp>

#include "UniformEncodingDistribution.hpp"
#include "NormalEncodingDistribution.hpp"
#include "LaplaceEncodingDistribution.hpp"


// This tests the boundries between values
void testDistribution(EncodingDistribution& _encodeDist, std::pair<int, int> _encodeRange)
{
	for(int v = _encodeRange.first; v < _encodeRange.second; ++v)
	{
		std::pair<ArithmeticEncoder::Range, ArithmeticEncoder::Range>
			range = _encodeDist.getRange(v);

		std::pair<int, std::pair<ArithmeticEncoder::Range, ArithmeticEncoder::Range>>
			v2_ = _encodeDist.getValue(range.first);

		if(range.second - range.first > 0)
		{
			CHECK( v == (int)v2_.first );
			CHECK( range.first == v2_.second.first );
			CHECK( range.second == v2_.second.second );

			v2_ = _encodeDist.getValue(range.second-1);

			CHECK( v == (int)v2_.first );
			CHECK( range.first == v2_.second.first );
			CHECK( range.second == v2_.second.second );
		}
	}
}

void testEncoding(int _seed)
{
	std::default_random_engine generator;
	generator.seed(_seed);
	std::uniform_int_distribution<int> distribution(0,255);

	NormalEncodingDistribution encodeDist({0, 256}, distribution(generator), distribution(generator)+100);

	testDistribution(encodeDist, {0, 256});

	std::vector<char> encodedData(1000);

	boost::iostreams::stream<boost::iostreams::basic_array_sink<char>>
		outDataStream(encodedData.data(),encodedData.size());
	auto streamStart = outDataStream.tellp();

	ArithmeticEncoder encoder(outDataStream);

	std::vector<unsigned char> values(100);
	for(auto& v : values)
		v = distribution(generator);

	for(auto v : values)
		encodeDist.encode(encoder, v);
	encoder.close();

	std::size_t writeSize = outDataStream.tellp() - streamStart;

	outDataStream.close();

	boost::iostreams::stream<boost::iostreams::basic_array_source<char>>
		inDataStream(encodedData.data(),encodedData.size());
	streamStart = inDataStream.tellg();

	ArithmeticDecoder decoder(inDataStream);
	decoder.open();

	for(auto& v : values)
	{
		unsigned char decodedV = encodeDist.decode(decoder);

		REQUIRE( static_cast<int>(decodedV) == static_cast<int>(v) );
	}

	std::size_t readSize = inDataStream.tellg() - streamStart;
	REQUIRE( readSize == writeSize );
}

TEST_CASE( "Test normal distribution", "[normal_distribution]" )
{
	std::pair<int, int> encodeRange{0, 256};

	NormalEncodingDistribution encodeDist(encodeRange, 100, 100);

	testDistribution(encodeDist, encodeRange);
}

TEST_CASE( "Test laplace distribution", "[laplace_distribution]" )
{
	std::pair<int, int> encodeRange{0, 256};

	LaplaceEncodingDistribution encodeDist(encodeRange, 70.239466369334451, 1.2228440911461289);

	testDistribution(encodeDist, encodeRange);
}

TEST_CASE( "Test arithmetic encoding and decoding", "[arithmetic_encoding_decoding]" )
{
	for(int seed = 0; seed < 1000; ++seed)
		testEncoding(seed);
}
