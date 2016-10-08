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
