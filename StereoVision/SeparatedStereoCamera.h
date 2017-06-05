#pragma once
#include "BaseStereoCamera.h"

//·ÖÀëÉãÏñ»ú
class SeparatedStereoCamera :
	public BaseStereoCamera
{
private:
	//×óÓÒÉãÏñÍ·
	VideoCapture captureL,captureR;

public:
	SeparatedStereoCamera();
	~SeparatedStereoCamera();

	void open(int l = -1, int r = -1);
	bool isOpened();
	void setFrame(int width = 0, int height = 0);
	bool next();
	void setF();
};

