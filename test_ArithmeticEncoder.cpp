/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#include "ArithmeticEncoder.hpp"
#include "ArithmeticDecoder.hpp"

#include "catch.hpp"

#include <random>
#include <boost/iostreams/stream.hpp>

#include "UniformEncodingDistribution.hpp"
#include "NormalEncodingDistribution.hpp"


TEST_CASE( "Test arithmetic encoding and decoding", "[arithmetic_encoding_decoding]" )
{
	UniformEncodingDistribution encodeDist;

	std::vector<char> encodedData(2048);
	boost::iostreams::stream<boost::iostreams::basic_array_sink<char>>
		outDataStream(encodedData.data(),encodedData.size());
	boost::iostreams::stream<boost::iostreams::basic_array_source<char>>
		inDataStream(encodedData.data(),encodedData.size());

	ArithmeticEncoder encoder(outDataStream);

	std::vector<unsigned char> values(1000);
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0,255);
	for(auto& v : values)
		v = distribution(generator);

	for(auto v : values)
		encodeDist.encode(encoder, v);
	encoder.close();

	outDataStream.close();

	ArithmeticDecoder decoder(inDataStream, encoder.size());
	decoder.open();

	for(auto& v : values)
	{
		unsigned char decodedV = encodeDist.decode(decoder);

		REQUIRE( static_cast<int>(decodedV) == static_cast<int>(v) );
	}
}


// This tests the boundries between values
template <class TEncode>
void testDistribution(EncodingDistribution<TEncode>& _encodeDist)
{
	for(int v = 0; v < 256; ++v)
	{
		std::pair<ArithmeticEncoder::Range, ArithmeticEncoder::Range>
			range = _encodeDist.getRange(v);

		std::pair<TEncode, std::pair<ArithmeticEncoder::Range, ArithmeticEncoder::Range>>
			v2_ = _encodeDist.getValue(range.first);

		CHECK( v == (int)v2_.first);
		CHECK( range.first == v2_.second.first );
		CHECK( range.second == v2_.second.second );

		v2_ = _encodeDist.getValue(v2_.second.first+range.second-1);

		CHECK( v == (int)v2_.first);
		CHECK( range.first == v2_.second.first );
		CHECK( range.second == v2_.second.second );
	}
}

TEST_CASE( "Test normal distribution", "[normal_distribution]" )
{
	NormalEncodingDistribution encodeDist(100, 100);

	testDistribution(encodeDist);
}
