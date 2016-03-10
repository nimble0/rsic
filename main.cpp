#include <iostream>
#include <boost/iostreams/stream.hpp>
#include "Image.hpp"
#include "RgbColour.hpp"
#include "ImageCompressor.hpp"


int main(int argc, char **argv)
{
	Image<RgbColour> image("soft-green2.png");
	std::vector<char> encodedData(20480000);
	boost::iostreams::stream<boost::iostreams::basic_array_sink<char>>
		outputStream(encodedData.data(),encodedData.size());
	std::streampos outputStreamStart = outputStream.tellp();
	ImageCompressor imageCompressor(image, outputStream);
	imageCompressor.compress();


	std::cout<<(outputStream.tellp() - outputStreamStart)<<std::endl;

    return 0;
}
