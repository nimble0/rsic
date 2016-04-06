#include <iostream>
#include <fstream>
#include "Image.hpp"
#include "RgbColour.hpp"
#include "ImageCompressor.hpp"


int main(int argc, char **argv)
{
	if(argc == 3)
	{
		std::string inPath(argv[1]);
		std::string outPath(argv[2]);

		Image<RgbColour> image(inPath);
		ImageCompressor imageCompressor(image);

		std::ofstream output;
		output.open(outPath);
		imageCompressor.compress(output);
		output.close();
	}
	else
		std::cout<<"Invalid number of arguments"<<std::endl;

    return 0;
}
