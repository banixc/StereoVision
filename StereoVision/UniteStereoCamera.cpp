#include "stdafx.h"
#include "UniteStereoCamera.h"


UniteStereoCamera::UniteStereoCamera()
{
	cout << "��ʼ�������������" << endl;
	capture = VideoCapture();
}


UniteStereoCamera::~UniteStereoCamera()
{
}

void UniteStereoCamera::open(int l, int r)
{
	if (l == -1) {
		cout << "��������������>";
		cin >> l;
	}
	capture.open(l);
}

bool UniteStereoCamera::isOpened()
{
	return capture.isOpened();
}

void UniteStereoCamera::setFrame(int width, int height)
{
	if (height == 0 || width == 0) {
		cout << "���������õķֱ���(�� ��)>";
		cin >> width >> height;
	}
	width *= 2;
	capture.set(CV_CAP_PROP_FRAME_WIDTH, width);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, height);
}

bool UniteStereoCamera::next()
{
	if (!capture.isOpened())
		return false;
	
	capture >> currentFrame;

	if (currentFrame.empty())
		return false;

	int height = currentFrame.rows;
	int width = currentFrame.cols;

	left.create(Size(width / 2, height), currentFrame.type());
	right.create(Size(width / 2, height), currentFrame.type());
	//������ͷ��ǰ֡
	currentFrame(Rect(width / 2, 0, width / 2, height)).copyTo(left);
	//������ͷ��ǰ֡
	currentFrame(Rect(0, 0, width / 2, height)).copyTo(right);

}

void UniteStereoCamera::setF()
{
	capture.set(CV_CAP_PROP_SETTINGS, 1);
}