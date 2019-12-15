#include "imgUtil.h"

#include <fstream>

std::vector<cv::Mat1b> readImages(const std::string & fileListPath)
{


	using namespace std;
	using namespace cv;

	vector<string> filePathList;

	if (!fileListPath.empty()) {

		ifstream ifs(fileListPath);

		string line;
		while (getline(ifs, line)) {
			filePathList.push_back(line);
		}
	}

	vector<Mat1b> imgs;

	for (string filePath : filePathList) {

		Mat1b im = imread(filePath, 0);

		if (!im.empty()) {
			imgs.push_back(im);
		}
	}

	return imgs;
}