#include "stdafx.h"
#include "ParamsLoader.h"
#include "UniteStereoCamera.h"
#include "SeparatedStereoCamera.h"
#include <opencv2\opencv.hpp>

ParamsLoader::ParamsLoader()
{
	FileStorage fs = FileStorage();
	fs.open(filename, FileStorage::READ);
	if (!fs.isOpened())
		return ;
	//������ ������� �궨��ǵ���� ��ȡͼƬ����
	fs["unit"] >> unit;
	fs["cameraNumberL"] >> cameraNumberL;
	fs["cameraNumberR"] >> cameraNumberR;
	fs["frameWidth"] >> frameWidth;
	fs["frameHeight"] >> frameHeight;
	fs["gridW"] >> gridW;
	fs["gridH"] >> gridH;
	fs["cutFrameNumber"] >> cutFrameNumber;
}


ParamsLoader::~ParamsLoader()
{
}

BaseStereoCamera * ParamsLoader::getCamera()
{
	BaseStereoCamera *camera;
	if (unit == 1) {
		camera = new UniteStereoCamera();
	}
	else {
		camera = new SeparatedStereoCamera();
	}

	camera->open(cameraNumberL, cameraNumberR);

	camera->setFrame(frameWidth, frameHeight);

	return camera;
}
