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
#include "LaplaceEncodingDistribution.hpp"


// This tests the boundries between values
template <class TEncode>
void testDistribution(EncodingDistribution<TEncode>& _encodeDist)
{
	for(int v = 0; v < 256; ++v)
	{
		std::pair<ArithmeticEncoder::Range, ArithmeticEncoder::DoubleRange>
			range = _encodeDist.getRange(v);

		std::pair<TEncode, std::pair<ArithmeticEncoder::Range, ArithmeticEncoder::DoubleRange>>
			v2_ = _encodeDist.getValue(range.first);

		CHECK( v == (int)v2_.first );
		CHECK( range.first == v2_.second.first );
		CHECK( range.second == v2_.second.second );

		v2_ = _encodeDist.getValue(range.second-1);

		CHECK( v == (int)v2_.first );
		CHECK( range.first == v2_.second.first );
		CHECK( range.second == v2_.second.second );
	}
}

void testEncoding(int _seed)
{
	std::default_random_engine generator;
	generator.seed(_seed);
	std::uniform_int_distribution<int> distribution(0,255);


	NormalEncodingDistribution encodeDist(distribution(generator), distribution(generator)+1);

	testDistribution(encodeDist);

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

TEST_CASE( "Test arithmetic encoding and decoding", "[arithmetic_encoding_decoding]" )
{
	for(int seed = 0; seed < 1000; ++seed)
		testEncoding(seed);
}

TEST_CASE( "Test normal distribution", "[normal_distribution]" )
{
	NormalEncodingDistribution encodeDist(100, 100);

	testDistribution(encodeDist);
}

TEST_CASE( "Test laplace distribution", "[laplace_distribution]" )
{
	LaplaceEncodingDistribution encodeDist(70.239466369334451, 1.2228440911461289);

	testDistribution(encodeDist);
}
