#pragma once

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class BaseStereoCamera
{
public:
	//构造函数
	BaseStereoCamera();
	//析构函数
	~BaseStereoCamera();
	//左摄像头图像
	cv::Mat left;
	//右摄像头图像
	cv::Mat right;
	//打开摄像头
	virtual void open(int l = -1,int r = -1) = 0;
	//判断摄像头是否正常打开
	virtual bool isOpened() = 0;
	//设置摄像头视频流帧大小
	virtual void setFrame(int width = 0, int height = 0) = 0;
	//取下一帧
	virtual bool next() = 0;
	//没什么用
	virtual void setF() = 0;
};

