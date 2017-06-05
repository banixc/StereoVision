#include "stdafx.h"
#include "SeparatedStereoCamera.h"


SeparatedStereoCamera::SeparatedStereoCamera()
{
	captureL = VideoCapture();
	captureR = VideoCapture();
}


SeparatedStereoCamera::~SeparatedStereoCamera()
{

}

void SeparatedStereoCamera::open(int l, int r)
{
	captureL.open(l);
	captureR.open(r);
}

bool SeparatedStereoCamera::isOpened()
{
	return captureL.isOpened() && captureR.isOpened();
}

void SeparatedStereoCamera::setFrame(int width, int height)
{
	captureL.set(CV_CAP_PROP_FRAME_WIDTH, width);
	captureL.set(CV_CAP_PROP_FRAME_HEIGHT, height);

	captureR.set(CV_CAP_PROP_FRAME_WIDTH, width);
	captureR.set(CV_CAP_PROP_FRAME_HEIGHT, height);
}

bool SeparatedStereoCamera::next()
{
	captureL >> left;
	captureR >> right;

	return true;
}

void SeparatedStereoCamera::setF()
{
	//captureL.set(CV_CAP_PROP_SETTINGS, 1);
	captureR.set(CV_CAP_PROP_SETTINGS, 1);
}