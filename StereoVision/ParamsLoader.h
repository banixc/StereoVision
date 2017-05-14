#pragma once
#include "BaseStereoCamera.h"

#define FILENAME "../input.yml" 

class ParamsLoader
{
public:

	ParamsLoader();
	~ParamsLoader();
	BaseStereoCamera* getCamera();
	int unit, cameraNumberL, cameraNumberR, frameWidth, frameHeight, gridW, gridH, cutFrameNumber;

private:
	const char* filename = FILENAME;
};

