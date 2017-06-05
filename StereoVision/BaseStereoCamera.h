#pragma once

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class BaseStereoCamera
{
public:
	//���캯��
	BaseStereoCamera();
	//��������
	~BaseStereoCamera();
	//������ͷͼ��
	cv::Mat left;
	//������ͷͼ��
	cv::Mat right;
	//������ͷ
	virtual void open(int l = -1,int r = -1) = 0;
	//�ж�����ͷ�Ƿ�������
	virtual bool isOpened() = 0;
	//��������ͷ��Ƶ��֡��С
	virtual void setFrame(int width = 0, int height = 0) = 0;
	//ȡ��һ֡
	virtual bool next() = 0;
	//ûʲô��
	virtual void setF() = 0;
};

