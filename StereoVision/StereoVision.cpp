// StereoVision.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ParamsLoader.h"

#define CUTPIC 1
#define SHOWVISION 2
//��ǰ����
#define MOD 2

//3D��ͼ
Mat pointCloud;

//��3D��ͼ��ȡ������Ϣ
float getD(int x, int y) {

	if (pointCloud.empty())
	{
		return -1;
	}

	// ��ȡ���ͼ��
	vector<Mat> xyzSet;
	split(pointCloud, xyzSet);
	Mat depth;
	xyzSet[2].copyTo(depth);

	return depth.at<float>(y, x);
}

//������ص�����
void on_mouse(int event, int x, int y, int flags, void *ustc)
{

	if (event == CV_EVENT_LBUTTONUP) {
		cout << x << "," << y << ":";
		cout << getD(x, y);
		cout << endl;
	}
}

//��ȡ3D��ͼ, �洢��pointCloud
int getPointClouds(Mat& disparity, Mat& Q)
{
	if (disparity.empty())
	{
		return 0;
	}

	reprojectImageTo3D(disparity, pointCloud, Q, true);

	pointCloud *= 1.6;
	
	for (int y = 0; y < pointCloud.rows; ++y)
	{
		for (int x = 0; x < pointCloud.cols; ++x)
		{
			Point3f point = pointCloud.at<Point3f>(y, x);
			point.y = -point.y;
			pointCloud.at<Point3f>(y, x) = point;
		}
	}

	return 1;
}

// ��ͼ ��MOD1��
int cutPic() {

	//���ز���
	ParamsLoader loader = ParamsLoader();
	BaseStereoCamera* camera = loader.getCamera();

	if (!camera->isOpened())
		return -1;


	Size boardSize = Size(loader.gridW, loader.gridH);
	
	int frameCount = 0;
	while (frameCount < loader.cutFrameNumber) {

		camera->next();

		imshow("LEFT", camera->left);
		imshow("RIGHT", camera->right);

		int t = waitKey(10);

		if (t == 27) {
			break;
		}
		//�����¿ո��ʱ ��ʼ��ͼ
		else if (t == ' ') {

			Mat grayL, grayR;
			cvtColor(camera->left, grayL, CV_BGR2GRAY);
			cvtColor(camera->right, grayR, CV_BGR2GRAY);
			vector<Point2f> temp;
			//��ȡ��Ƭ
			bool findL = findChessboardCorners(grayL, boardSize, temp);
			bool findR = findChessboardCorners(grayR, boardSize, temp);

			if (findL && findR) {
				char lName[64], rName[64];
				sprintf(lName, "../L%d.jpg", frameCount);
				sprintf(rName, "../R%d.jpg", frameCount);
				imwrite(lName, camera->left);
				imwrite(rName, camera->right);
				frameCount++;
				cout << "�� " << frameCount << "/" << loader.cutFrameNumber << " ��ͼƬ�ѱ���" << endl;
			}
			else {
				cout << (findL ? "��" : "��") << "ͼ�޷��ҵ����нǵ㣬�����ԣ�" << endl;
			}

		}
			
	}
	delete camera;
	return 0;
}

// ��� ��MOD2��
int showVision() {

	//�ڲ�
	Mat cameraMatrix1, cameraMatrix2;
	//�������
	Mat distCoeffs1, distCoeffs2;
	//˫�������Բ���
	Mat om, T, R;

	Mat Rl, Rr, Pl, Pr, Q;

	//��Ӧ����ROI��
	Rect vol, vor;

	Size imageSize = Size(640, 480);

	//���ļ��м��ز���
	FileStorage fs;
	fs.open("..\\stereo.yml", FileStorage::READ);

	if (!fs.isOpened()) {
		cout << "�򿪲����ļ�����" << endl;
		return 0;
	}

	fs["cameraMatrix1"] >> cameraMatrix1;
	fs["cameraMatrix2"] >> cameraMatrix2;
	fs["distCoeffs1"] >> distCoeffs1;
	fs["distCoeffs2"] >> distCoeffs2;
	fs["R"] >> R;
	fs["T"] >> T;

	fs.release();

	//����У��
	//���ڴ˴��궨����MATLAB����� ���ֱ�ӽ�������У��
	stereoRectify(cameraMatrix1, distCoeffs1, cameraMatrix2, distCoeffs2, imageSize, R, T, Rl, Rr, Pl, Pr, Q, CALIB_ZERO_DISPARITY, -1, imageSize, &vol, &vor);

	Mat mapLx = Mat(imageSize, CV_32FC1);
	Mat mapLy = Mat(imageSize, CV_32FC1);
	Mat mapRx = Mat(imageSize, CV_32FC1);
	Mat mapRy = Mat(imageSize, CV_32FC1);

	//������ӳ�����
	initUndistortRectifyMap(cameraMatrix1, distCoeffs1, Rl, Pl, imageSize, CV_32FC1, mapLx, mapLy);
	initUndistortRectifyMap(cameraMatrix2, distCoeffs2, Rr, Pr, imageSize, CV_32FC1, mapRx, mapRy);

	//��ʾ
	ParamsLoader loader = ParamsLoader();
	BaseStereoCamera* camera = loader.getCamera();

	if (!camera->isOpened())
		return -1;

	//SBMƥ���㷨��ʼ��
	int	numberOfDisparities = 96;
	int SADWindowSize = 19;
	Ptr<StereoBM> sbm = StereoBM::create(numberOfDisparities, SADWindowSize);
	sbm->setPreFilterCap(31);
	sbm->setPreFilterSize(9);
	sbm->setROI1(vol);
	sbm->setROI2(vor);
	sbm->setMinDisparity(0); 
	sbm->setNumDisparities(numberOfDisparities);
	sbm->setTextureThreshold(10);
	sbm->setDisp12MaxDiff(-1);
	sbm->setUniquenessRatio(25);
	sbm->setSpeckleRange(32);
	sbm->setSpeckleWindowSize(100);
	sbm->setBlockSize(9);

	while (true) {
		camera->next();
		Mat rectifyImageL, rectifyImageR, grayL, grayR, Mask;
		//��ӳ���������ͼƬ
		remap(camera->left, rectifyImageL, mapLx, mapLy, INTER_LINEAR);
		remap(camera->right, rectifyImageR, mapRx, mapRy, INTER_LINEAR);

		//��У�������ʾ����
		Mat canvas;
		double sf;
		int w, h;
		sf = imageSize.width / MAX(imageSize.width, imageSize.height);
		w = cvRound(imageSize.width * sf);
		h = cvRound(imageSize.height * sf);
		canvas.create(h, w * 2, CV_8UC3);

		/*��ͼ�񻭵�������*/
		Mat canvasPartL = canvas(Rect(w * 0, 0, w, h));									//�õ�������һ����
		resize(rectifyImageL, canvasPartL, canvasPartL.size(), 0, 0, INTER_AREA);		//��ͼ�����ŵ���canvasPartһ����С
		Rect vroiL(cvRound(vol.x*sf), cvRound(vol.y*sf),								//��ñ���ȡ������	
			cvRound(vol.width*sf), cvRound(vol.height*sf));
		rectangle(canvasPartL, vroiL, Scalar(0, 0, 255), 3, 8);							//�����Ӧ��

		/*��ͼ�񻭵�������*/
		Mat canvasPartR = canvas(Rect(w, 0, w, h));										//��û�������һ����
		resize(rectifyImageR, canvasPartR, canvasPartR.size(), 0, 0, INTER_LINEAR);
		Rect vroiR(cvRound(vor.x * sf), cvRound(vor.y*sf),
			cvRound(vor.width * sf), cvRound(vor.height * sf));
		rectangle(canvasPartR, vroiR, Scalar(0, 255, 0), 3, 8);							//���Ҹ�Ӧ��

		rectangle(canvasPartL, vroiL & vroiR, Scalar(255, 0, 0), 3, 8);					//�������ҹ�ͬ��Ӧ��

		for (int i = 0; i < canvas.rows; i += 16)
			line(canvas, Point(0, i), Point(canvas.cols, i), Scalar(0, 255, 0), 1, 8);

		imshow("RECTIFIED", canvas);
		
		//��ʼ����ƥ��
		//ת��Ϊ�Ҷ�ͼ
		cvtColor(rectifyImageL, grayL, CV_BGR2GRAY);
		cvtColor(rectifyImageR, grayR, CV_BGR2GRAY);

		Mat disp, vdisp;
		//ȡ���Ӳ�ͼ
		sbm->compute(grayL, grayR, disp);
		normalize(disp, vdisp, 0, 256, NORM_MINMAX, CV_8U);
		//��ʾ�Ӳ�ͼ
		imshow("BM�㷨�Ӳ�ͼ", vdisp);
		//����Ӳ�ͼ
		getPointClouds(disp, pointCloud);
		//���ص�
		setMouseCallback("BM�㷨�Ӳ�ͼ", on_mouse, 0);
		int t = waitKey(10);

		if (t == 27) {
			//��ESC�˳�
			break;
		}
	}

}



int main()
{
	switch (MOD)
	{
	default:
		break;
	case CUTPIC:
		return cutPic();
	case SHOWVISION:
		return showVision();
	}


    return 0;
}



