/*
 * Copyright 2016 Erik Crevel <erik.crevel@ntlworld.com>
 */

#include "ArithmeticEncoder.hpp"
#include "ArithmeticDecoder.hpp"

#include "catch.hpp"

#include <random>
#include <boost/iostreams/stream.hpp>

#include "UniformEncodingDistribution.hpp"
#include "UniformDecodingDistribution.hpp"


TEST_CASE( "Test arithmetic encoding and decoding", "[arithmetic_encoding_decoding]" )
{
	std::vector<char> encodedData(2048);
	boost::iostreams::stream<boost::iostreams::basic_array_sink<char>>
		outDataStream(encodedData.data(),encodedData.size());
	boost::iostreams::stream<boost::iostreams::basic_array_source<char>>
		inDataStream(encodedData.data(),encodedData.size());

	ArithmeticEncoder encoder(outDataStream);
	UniformEncodingDistribution encodeDist(encoder);

	std::vector<unsigned char> values(1000);
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0,255);
	for(auto& v : values)
		v = distribution(generator);

	for(auto v : values)
		encodeDist.encode(v);
	encoder.close();

	outDataStream.close();

	ArithmeticDecoder decoder(inDataStream, encoder.size());
	UniformDecodingDistribution decodeDist(decoder);
	decoder.open();

	for(auto& v : values)
	{
		unsigned char decodedV = decodeDist.decode();

		REQUIRE( static_cast<int>(decodedV) == static_cast<int>(v) );
	}
}
