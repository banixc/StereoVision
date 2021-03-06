// StereoVision.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ParamsLoader.h"

#define CUTPIC 1
#define SHOWVISION 2
//判断是否检测角点 若不检测则直接跳过
#define Detection_corner true

//深度图
Mat depthMap,Q,disp;

ParamsLoader loader;

//从3D云图获取距离信息
float getDistance(int x, int y) {

	if (depthMap.empty())
	{
		return -1;
	}

	// 提取深度图像
	vector<Mat> xyzSet;
	split(depthMap, xyzSet);
	Mat depth;
	xyzSet[2].copyTo(depth);

	return depth.at<float>(y, x);
}



//获取3D云图, 存储在depthMap
int get3DDepthMap(Mat& disparity)
{
	if (disparity.empty())
	{
		return 0;
	}

	reprojectImageTo3D(disparity, depthMap, Q, true);

	depthMap *= 1.6;
	
	for (int y = 0; y < depthMap.rows; ++y)
	{
		for (int x = 0; x < depthMap.cols; ++x)
		{
			Point3f point = depthMap.at<Point3f>(y, x);
			point.y = -point.y;
			depthMap.at<Point3f>(y, x) = point;
		}
	}

	return 1;
}

//鼠标点击回调函数
void on_mouse(int event, int x, int y, int flags, void *ustc)
{

	if (event == CV_EVENT_LBUTTONUP) {
		get3DDepthMap(disp);
		cout << "坐标(" << x << ", " << y << ")对应的目标点的距离为: " << getDistance(x, y) << endl;
	}
}

// 截图 （MOD1）
int cutPic() {

	BaseStereoCamera* camera = loader.getCamera();

	if (!camera->isOpened())
		return -1;


	Size boardSize = Size(loader.gridW, loader.gridH);
	
	int frameCount = 0;
	while (frameCount < loader.cutFrameNumber) {

		camera->next();
		//camera->setF();

		imshow("LEFT", camera->left);
		imshow("RIGHT", camera->right);

		int t = waitKey(10);

		if (t == 27) {
			break;
		}
		//当按下空格键时 开始截图
		else if (t == ' ') {

			Mat grayL, grayR;
			cvtColor(camera->left, grayL, CV_BGR2GRAY);
			cvtColor(camera->right, grayR, CV_BGR2GRAY);
			vector<Point2f> temp;

			if (Detection_corner) {
				bool findL = findChessboardCorners(grayL, boardSize, temp);
				bool findR = findChessboardCorners(grayR, boardSize, temp);
				if (findL && findR) {
					char lName[64], rName[64];
					sprintf(lName, "../L%d.jpg", frameCount);
					sprintf(rName, "../R%d.jpg", frameCount);
					imwrite(lName, camera->left);
					imwrite(rName, camera->right);
					frameCount++;
					cout << "第 " << frameCount << "/" << loader.cutFrameNumber << " 张图片已保存" << endl;
				}
				else {
					cout << (findL ? "右" : "左") << "图无法找到所有角点，请重试！" << endl;
				}
			}
			else {
				char lName[64], rName[64];
				sprintf(lName, "../L%d.jpg", frameCount);
				sprintf(rName, "../R%d.jpg", frameCount);
				imwrite(lName, camera->left);
				imwrite(rName, camera->right);
				frameCount++;
				cout << "第 " << frameCount << "/" << loader.cutFrameNumber << " 张图片已保存" << endl;
			}
		}
			
	}
	delete camera;
	return 0;
}

// 测距 （MOD2）
int showVision() {

	//内参
	Mat cameraMatrix1, cameraMatrix2;
	//畸变参数
	Mat distCoeffs1, distCoeffs2;
	//双摄像机相对参数
	Mat om, T, R;

	Mat Rl, Rr, Pl, Pr;

	//感应区域（ROI）
	Rect vol, vor;

	BaseStereoCamera* camera = loader.getCamera();

	//从文件中加载参数
	FileStorage fs;
	fs.open("..\\stereo.yml", FileStorage::READ);

	if (!fs.isOpened()) {
		cout << "打开参数文件错误" << endl;
		return 0;
	}

	fs["cameraMatrix1"] >> cameraMatrix1;
	fs["cameraMatrix2"] >> cameraMatrix2;
	fs["distCoeffs1"] >> distCoeffs1;
	fs["distCoeffs2"] >> distCoeffs2;
	fs["R"] >> R;
	fs["T"] >> T;

	fs.release();


	Size imageSize = Size(loader.frameWidth, loader.frameHeight);


	//立体校正
	//由于此处标定已在MATLAB中完成 因此直接进行立体校正
	stereoRectify(cameraMatrix1, distCoeffs1, cameraMatrix2, distCoeffs2, imageSize, R, T, Rl, Rr, Pl, Pr, Q, CALIB_ZERO_DISPARITY, -1, imageSize, &vol, &vor);

	Mat mapLx = Mat(imageSize, CV_32FC1);
	Mat mapLy = Mat(imageSize, CV_32FC1);
	Mat mapRx = Mat(imageSize, CV_32FC1);
	Mat mapRy = Mat(imageSize, CV_32FC1);

	//计算重映射矩阵
	initUndistortRectifyMap(cameraMatrix1, distCoeffs1, Rl, Pl, imageSize, CV_32FC1, mapLx, mapLy);
	initUndistortRectifyMap(cameraMatrix2, distCoeffs2, Rr, Pr, imageSize, CV_32FC1, mapRx, mapRy);

	//显示


	if (!camera->isOpened())
		return -1;

	//SBM匹配算法初始化
	int	numberOfDisparities = 96;
	int SADWindowSize = 19;
	Ptr<StereoBM> sbm = StereoBM::create(numberOfDisparities, SADWindowSize);
	sbm->setPreFilterCap(31);
	sbm->setPreFilterSize(25);
	sbm->setROI1(vol);
	sbm->setROI2(vor);
	sbm->setMinDisparity(0); 
	//sbm->setNumDisparities(numberOfDisparities);
	sbm->setTextureThreshold(10);
	sbm->setDisp12MaxDiff(-1);
	sbm->setUniquenessRatio(25);
	sbm->setSpeckleRange(32);
	sbm->setSpeckleWindowSize(100);
	sbm->setBlockSize(9);

	while (true) {
		camera->next();
		Mat rectifyImageL, rectifyImageR, grayL, grayR, Mask;
		//重映射左右相机图片
		remap(camera->left, rectifyImageL, mapLx, mapLy, INTER_LINEAR);
		remap(camera->right, rectifyImageR, mapRx, mapRy, INTER_LINEAR);

		//把校正结果显示出来
		//主要是负责把两个摄像机显示的内容同时显示在一个窗口中
		Mat canvas;
		double sf;
		int w, h;
		//这个地方的分辨率实际上有BUG，在640*480下无问题，更高的分辨率可能会报错
		sf = imageSize.width / MAX(imageSize.width, imageSize.height);
		w = cvRound(imageSize.width * sf);
		h = cvRound(imageSize.height * sf);
		canvas.create(h, w * 2, CV_8UC3);

		/*左图像画到画布上*/
		Mat canvasPartL = canvas(Rect(w * 0, 0, w, h));									//得到画布的一部分
		resize(rectifyImageL, canvasPartL, canvasPartL.size(), 0, 0, INTER_AREA);		//把图像缩放到跟canvasPart一样大小
		Rect vroiL(cvRound(vol.x*sf), cvRound(vol.y*sf),								//获得被截取的区域	
			cvRound(vol.width*sf), cvRound(vol.height*sf));
		rectangle(canvasPartL, vroiL, Scalar(0, 0, 255), 3, 8);							//画左感应区

		/*右图像画到画布上*/
		Mat canvasPartR = canvas(Rect(w, 0, w, h));										//获得画布的另一部分
		resize(rectifyImageR, canvasPartR, canvasPartR.size(), 0, 0, INTER_LINEAR);
		Rect vroiR(cvRound(vor.x * sf), cvRound(vor.y*sf),
			cvRound(vor.width * sf), cvRound(vor.height * sf));
		rectangle(canvasPartR, vroiR, Scalar(0, 255, 0), 3, 8);							//画右感应区

		rectangle(canvasPartL, vroiL & vroiR, Scalar(255, 0, 0), 3, 8);					//画出左右共同感应区

		for (int i = 0; i < canvas.rows; i += 16)
			line(canvas, Point(0, i), Point(canvas.cols, i), Scalar(0, 255, 0), 1, 8);

		imshow("RECTIFIED", canvas);
		
		//开始立体匹配
		//转换为灰度图
		cvtColor(rectifyImageL, grayL, CV_BGR2GRAY);
		cvtColor(rectifyImageR, grayR, CV_BGR2GRAY);

		Mat vdisp;
		//取得视差图
		sbm->compute(grayL, grayR, disp);

		normalize(disp, vdisp, 0, 256, NORM_MINMAX, CV_8U);
		//显示视差图
		imshow("BM算法视差图", vdisp);
		//获得视差图
		//鼠标回掉
		setMouseCallback("BM算法视差图", on_mouse, 0);
		int t = waitKey(10);

		if (t == 27) {
			//按ESC退出
			break;
		}
	}

}



int main()
{
	//加载参数

	loader = ParamsLoader();

	switch (loader.mod)
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



