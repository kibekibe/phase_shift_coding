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

#include "PhaseShift.h"

using namespace cv;
using namespace std;


constexpr double PI = 3.14159265359;


cv::Mat1d calcPhaseMap(const std::vector<cv::Mat1b>& imgs, const PhaseShiftParam& param)
{
	const size_t ImgNum = imgs.size();

	if (ImgNum == 0) {
		cout << "No images" << endl;
		return Mat1d();
	}



	cv::Mat1d phaseMap(imgs[0].size());
		
	for (int j = 0; j < phaseMap.rows; ++j) {
		for (int i = 0; i < phaseMap.cols; ++i) {

			double denom = 0.0; // •ª•ê
			double numer = 0.0; // •ªŽq

			for (int t = 0; t < param.angleList.size(); ++t) {
				denom += imgs[t](j, i) * param.cosList[t];
				numer += imgs[t](j, i) * param.sinList[t];
			}

			double phase = atan2(numer, denom);

			phaseMap(j, i) = phase;
		}
	}



	return phaseMap;
}

std::vector<cv::Mat1b> genSinPatternImg(const SinPatternParam& param, 
										const cv::Size& imgSize,
										const std::vector<double>& phaseList)
{
	const size_t ImgNum = phaseList.size();

	if (ImgNum == 0) {

		return vector<Mat1b>();
	}

	vector<Mat1b> imgs(ImgNum);

	for (int t = 0; t < ImgNum; ++t) {



		vector<uchar> scanline(imgSize.width);

		for (int x = 0; x < imgSize.width; ++x) {
			double val = param.amplitude * cos((x / param.waveLength + (double)t / ImgNum ) * 2 * PI  ) + param.bias + 0.5;
			scanline[x] = int(val + 0.5);
		}

		imgs[t] = Mat1b::zeros(imgSize);

		for (int y = 0; y < imgSize.height; ++y) {
			uchar* ptr = imgs[t].ptr(y);
			memcpy_s(ptr, scanline.size(), &scanline.at(0), scanline.size());
		}

	}

	return imgs;
	
}

PhaseShiftParam::PhaseShiftParam()
{
}

PhaseShiftParam::PhaseShiftParam(int divisionNum)
{
	this->setAngleList(divisionNum);
}

void PhaseShiftParam::setAngleList(int divisionNum)
{

	

	this->angleList.resize(divisionNum);
	this->cosList.resize(divisionNum);
	this->sinList.resize(divisionNum);

	for (int i = 0; i < divisionNum; ++i) {
		this->angleList[i] = PI * 2 * i / divisionNum;
		this->cosList[i] = cos(this->angleList[i]);
		this->sinList[i] = sin(this->angleList[i]);
	}
}

SinPatternParam::SinPatternParam()
{
	this->amplitude = 100;
	this->bias = 120;
	this->waveLength = 256;
}
