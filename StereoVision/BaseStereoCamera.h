#pragma once

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class BaseStereoCamera
{
public:
	BaseStereoCamera();
	~BaseStereoCamera();
	Mat left;
	Mat right;
	virtual void open(int l = -1,int r = -1) = 0;
	virtual bool isOpened() = 0;
	virtual void setFrame(int width = 0, int height = 0) = 0;
	virtual bool next() = 0;
};

