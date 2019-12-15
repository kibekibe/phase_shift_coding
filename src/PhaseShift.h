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

#pragma once



#include "opencv2/opencv.hpp"


struct PhaseShiftParam {

	std::vector<double> angleList;
	std::vector<double> cosList;
	std::vector<double> sinList;

	
	PhaseShiftParam();
	PhaseShiftParam(int divisionNum);

	void setAngleList(int divisionNum);


};

struct PhaseShiftDecodeParam {

	enum UnwrapMethod {
		None,
		InitailMapBased
	};


	UnwrapMethod unwrapMethod;

};

struct SinPatternParam {

	double amplitude;

	double bias;

	int waveLength; // ’PˆÊ‚Ípixel

	SinPatternParam();

};


cv::Mat1d calcPhaseMap(const std::vector<cv::Mat1b>& imgs, const PhaseShiftParam& param);




cv::Mat1d unwrapFromInitialMap(const cv::Mat1f& initMap, const cv::Mat1d & wrappedMap, const cv::Mat1b& mask, const SinPatternParam& sinParam);

std::vector<cv::Mat1b> genSinPatternImg(const SinPatternParam& param,
										const cv::Size& imgSize,
										const std::vector<double>& phaseList);

