#pragma once
#include "BaseStereoCamera.h"


//ÁªºÏÉãÏñ»ú
class UniteStereoCamera :
	public BaseStereoCamera
{
private:
	VideoCapture capture;


public:

	Mat currentFrame;

	UniteStereoCamera();
	~UniteStereoCamera();
	void open(int l = -1, int r = -1);
	bool isOpened();
	void setFrame(int width = 0, int height = 0);
	bool next();
	void setF();
};

