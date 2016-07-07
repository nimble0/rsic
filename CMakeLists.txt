cmake_minimum_required(VERSION 2.8)

project(imagecompressor)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

find_package(ImageMagick COMPONENTS Magick++)
find_package(Boost COMPONENTS system REQUIRED)

include_directories(${ImageMagick_INCLUDE_DIRS})

add_executable(image_compress
	ArithmeticEncoder.cpp
	ArithmeticDecoder.cpp
	EncodingDistribution.cpp

	NormalEncodingDistribution.cpp
	LaplaceEncodingDistribution.cpp
	CombinedEncodingDistribution.cpp
	VarDistribution.cpp
	MultiVarEncodingDistribution.cpp

	Image.cpp
	ImageSegmentCompressor.cpp
	ImageCompressor.cpp

	compress_main.cpp
)
target_link_libraries(image_compress ${ImageMagick_LIBRARIES})
target_link_libraries(image_compress ${Boost_LIBRARIES})

add_executable(image_decompress
	ArithmeticEncoder.cpp
	ArithmeticDecoder.cpp
	EncodingDistribution.cpp

	NormalEncodingDistribution.cpp
	LaplaceEncodingDistribution.cpp
	CombinedEncodingDistribution.cpp
	VarDistribution.cpp
	MultiVarEncodingDistribution.cpp

	Image.cpp
	ImageSegmentCompressor.cpp
	ImageCompressor.cpp

	decompress_main.cpp
)
target_link_libraries(image_decompress ${ImageMagick_LIBRARIES})
target_link_libraries(image_decompress ${Boost_LIBRARIES})

add_executable(imagecompressor_test
	ArithmeticEncoder.cpp
	ArithmeticDecoder.cpp
	EncodingDistribution.cpp

	NormalEncodingDistribution.cpp
	LaplaceEncodingDistribution.cpp
	CombinedEncodingDistribution.cpp
	VarDistribution.cpp
	MultiVarEncodingDistribution.cpp

	Image.cpp
	ImageSegmentCompressor.cpp
	ImageCompressor.cpp

	test_ArithmeticEncoder.cpp
	test_main.cpp
)
target_link_libraries(imagecompressor_test ${ImageMagick_LIBRARIES})
target_link_libraries(imagecompressor_test ${Boost_LIBRARIES})

install(TARGETS image_compress image_decompress RUNTIME DESTINATION bin)
