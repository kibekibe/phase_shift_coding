/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2015, OpenCV Foundation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/


#include "opencv2/opencv.hpp"

#include "PhaseShift.h"

#include "imgUtil.h"

using namespace std;
using namespace cv;


void run_writing_patterns(int argc, char* argv[]) {

	

	const char* keys =
	{
		"{width   | 1024| Projector width}"
		"{height  | 768 | Projector height}"
		"{periods | | Number of periods}"
		"{mode        | output_patterns |}"
		"{divisionNum | 8 |}"
		"{outputPatternPath |.| Path to save patterns}"

	};

	CommandLineParser parser(argc, argv, keys);

	int divisionNum = parser.get<int>("divisionNum");

	SinPatternParam sinPatternParam;
	
	PhaseShiftParam psParam(divisionNum);

	int patternImgWidth = parser.get<int>("width");
	int patternImgHeight = parser.get<int>("height");

	Size patternImgSize(patternImgWidth, patternImgHeight);

	std::vector<cv::Mat1b> patternImg = genSinPatternImg(sinPatternParam, patternImgSize, psParam.angleList);

	for (int i = 0; i < patternImg.size(); ++i) {
		imwrite(format("pat_%03d.png", i), patternImg[i]);
	}

}


void run_decode_patterns(int argc, char* argv[]) {

	const char* keys =
	{
		"{fileList     | imageList.txt   |}"
		"{waveLength   | 1024 |}"
		"{mask         | validMask.png   |}"
		"{unwrapMethod | initialMapBased |}"
		"{divisionNum  | 8               |}"
	};

	CommandLineParser parser(argc, argv, keys);

	const string fileListPath = parser.get<string>("fileList");

	vector<Mat1b> imgList = readImages(fileListPath);

	const cv::Mat1b mask = imread(parser.get<string>("mask"), 0);

	const int divisionNum = parser.get<int>("divisionNum");

	PhaseShiftParam psParam(divisionNum);

	SinPatternParam sinParam;
	sinParam.waveLength = parser.get<int>("waveLength");

	PhaseShiftDecodeParam::UnwrapMethod unwrapMethod;

	const string methodName = parser.get<string>("unwrapMethod");
	if (methodName == "initialMapBased") {
		unwrapMethod = PhaseShiftDecodeParam::InitailMapBased;
	}
	else {
		cout << "[Error] Unexpected method name." << endl;
	}

	Mat1d phaseMap;

	if (unwrapMethod == PhaseShiftDecodeParam::InitailMapBased) {
		CommandLineParser initialMapParser(argc, argv,
			"{initialDecodedMap | initMap.exr|}");

		const string initialDecodeMapName = parser.get<string>("initialDecodedMap");

		const Mat1f initMap = imread(initialDecodeMapName, -1);

		const Mat1d wrappedMap = calcPhaseMap(imgList, psParam);

		const Mat1d unwrappedMap = unwrapFromInitialMap(initMap, wrappedMap, mask, sinParam);

		phaseMap = unwrappedMap;
	}
	else if (unwrapMethod == PhaseShiftDecodeParam::None) {
		phaseMap = calcPhaseMap(imgList, psParam);
	}

	double minVal, maxVal;
	cv::minMaxLoc(phaseMap, &minVal, &maxVal);

	cout << "min : " << minVal << endl;
	cout << "max : " << maxVal << endl;

	
	Mat colorMap;	
	Mat1b scaledPhase1b;

	if (unwrapMethod == PhaseShiftDecodeParam::InitailMapBased) {
		
		convertScaleAbs(phaseMap, scaledPhase1b, 255.0 / 1024 , 0);

		applyColorMap(scaledPhase1b, colorMap, COLORMAP_JET);
	}
	else if (unwrapMethod == PhaseShiftDecodeParam::None) {
		const double PI = 3.14159265358979;
		convertScaleAbs(phaseMap, scaledPhase1b, 255 / 2 / PI, 127);

		applyColorMap(scaledPhase1b, colorMap, COLORMAP_JET);
	}

	imwrite("colorMap.png", colorMap);
}


void test_initial_map_based_unwrapping() {

	Mat1f initMap(20, 100);

	for (int j = 0; j < initMap.rows; ++j) {
		for (int i = 0; i < initMap.cols; ++i) {
			initMap(j, i) = i;
		}
	}

	const int divisionNum = 8;

	

	PhaseShiftParam psParam(divisionNum);
	SinPatternParam sinPatternParam;
	sinPatternParam.waveLength = 25;

	Size patternImgSize(initMap.size());

	std::vector<cv::Mat1b> patternImg = genSinPatternImg(sinPatternParam, patternImgSize, psParam.angleList);

	const Mat1d wrappedMap = calcPhaseMap(patternImg, psParam);
	{
		Mat1b scaledPhase1b;
		const double PI = 3.14159265358979;
		convertScaleAbs(wrappedMap, scaledPhase1b, 255 / 2 / PI, 127);
		Mat colorMap;
		applyColorMap(scaledPhase1b, colorMap, COLORMAP_JET);
		imwrite("wrapped.png", colorMap);
	}
	const Mat1b mask = Mat1b(initMap.size(), 255);
	const Mat1d unwrappedMap = unwrapFromInitialMap(initMap, wrappedMap, mask, sinPatternParam);

	double minVal, maxVal;
	cv::minMaxLoc(unwrappedMap, &minVal, &maxVal);

	cout << "min : " << minVal << endl;
	cout << "max : " << maxVal << endl;

	Mat colorMap;
	Mat1b scaledPhase1b;

	const double PI = 3.14159265358979;
	convertScaleAbs(unwrappedMap, scaledPhase1b, 255.0 / initMap.cols, 0);

	applyColorMap(scaledPhase1b, colorMap, COLORMAP_JET);


	imwrite("unwrapped.png", colorMap);
}

int main(int argc, char* argv[]) {

	using namespace cv;

	const char* keys =
	{
		"{mode        | writing_patterns |}"
	
	};

	CommandLineParser parser(argc, argv, keys);
	string mode = parser.get<string>("mode");

	if (mode == "writing_patterns") {
		run_writing_patterns(argc, argv);
	}
	else if (mode == "decode_patterns") {
		run_decode_patterns(argc, argv);
	}
	else if (mode == "test_initial_map_based_unwrapping") {
		test_initial_map_based_unwrapping();
	}

	return 0;
}