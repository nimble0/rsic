#include <iostream>
#include <utility>
#include <limits>

#include <boost/iostreams/stream.hpp>

#include "Image.hpp"
#include "RgbColour.hpp"
#include "ArithmeticEncoder.hpp"
#include "ArithmeticDecoder.hpp"



// o' = (o0*o1*o2)/(o0*o1 + o0*o2 + o1*o2)
// u' = (u0*o1*o2 + u1*o0*o2 + u2*o0*o1)/(o0*o1 + o0*o2 + o1*o2)
//    = o'*u0/o0 + o'*u1/o1 + o'*u2/o2;
std::pair<double,double> combineNormalDistributions(const std::vector<std::pair<double,double>>& _dists)
{
	std::pair<double,double> combinedDist;

	// a = o0*o1*o2*...*on
	// b = a/o0 + a/o1 + a/o2 + ... + a/on

	double a = 1;
	for(auto dist : _dists)
		a *= dist.second;

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

class FlatDistribution : public Distribution<unsigned char, unsigned int>
{
public:
	std::pair<unsigned int, unsigned int>      getRange(unsigned char _v)
	{
		return {_v*256*256*256, 256*256*256-(_v==255?1:0)};
	}
	std::pair<
		unsigned char,
		std::pair<unsigned int, unsigned int>> getValue(unsigned int _v)
	{
		unsigned char value = _v/(256*256*256);

		return {value, {value*256*256*256, 256*256*256-(_v==255?1:0)}};
	}
};


int main(int argc, char **argv) {
//     Image<RgbColour> image("soft-green.png");
//
// 	std::vector<std::tuple<int,int,int>> totals(256);
//
// 	for(std::size_t y = 0; y < image.height(); ++y)
// 		for(std::size_t x = 1; x < image.width(); ++x)
// 		{
// 			int p = image.get(x,y).r();
// 			double pp = p*p;
// 			++std::get<0>(totals[image.get(x-1,y).r()]);
// 			std::get<1>(totals[image.get(x-1,y).r()]) += p;
// 			std::get<2>(totals[image.get(x-1,y).r()]) += pp;
// 		}
//
// 	for(auto& total : totals)
// 	{
// 		std::cout
// 			<<std::get<0>(total)<<", "
// 			<<std::get<1>(total)<<", "
// 			<<std::get<2>(total)<<", "
// 			<<std::endl;
// 	}

	std::vector<char> encodedData(10);
	boost::iostreams::stream<boost::iostreams::basic_array_sink<char>>
		outDataStream(encodedData.data(),encodedData.size());
	boost::iostreams::stream<boost::iostreams::basic_array_source<char>>
		inDataStream(encodedData.data(),encodedData.size());

	FlatDistribution dist;

	ArithmeticEncoder<unsigned char, unsigned int> encoder(outDataStream, dist);
	encoder.encode(4);
	encoder.encode(45);
	encoder.encode(34);
	encoder.encode(95);
	encoder.encode(25);
	encoder.encode(185);
	encoder.encode(72);
	encoder.encode(32);
	encoder.stopEncoding();

	outDataStream.close();

	for(char c : encodedData)
		std::cout<<static_cast<int>(static_cast<unsigned char>(c))<<std::endl;

	ArithmeticDecoder<unsigned char, unsigned int> decoder(inDataStream, dist, encoder.size());
	decoder.startDecoding();
	for(int i = 0; i < 8; ++i)
		std::cout<<static_cast<int>(decoder.decode())<<std::endl;


    return 0;
}
