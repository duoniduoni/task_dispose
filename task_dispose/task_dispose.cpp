// task_dispose.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <vector>
#include <list>
#include "Contour.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

#define WNAME "window_for_show"

int _tmain(int argc, char * argv[])
{
	char * path = NULL;
	if(argc == 1)
		path = "c:\\task.bmp";
	else
		path = argv[1];

	Mat src = imread(path, IMREAD_GRAYSCALE);
	if(src.data == NULL)
	{
		cout << "the file which input is invalid !" << endl;
		return 0;
	}
	
	cvNamedWindow(WNAME);
	imshow(WNAME, src);

	cout << "press any key to into next step !" << endl;
	cvWaitKey(0);

	//二值化处理 将大于0的
	Mat dst = src.clone();
	threshold(src, dst, 3, 255, THRESH_BINARY_INV);
	imshow(WNAME, dst);
	imwrite("c:\\task2.bmp", dst);

	cout << "press any key to into next step !" << endl;
	cvWaitKey(0);

	//获取文字黑边
	ContourManager cm;
	cm.analyse(dst);
	cout << "cm.size() = " << cm.getContourCount() << endl;

	Mat show = Mat::zeros(dst.rows, dst.cols, CV_8UC3);
	for(int i = 0; i < cm.getContourCount(); ++i)
		cm.getContour(i).drawContour(show);
	imshow(WNAME, show);
	imwrite("task3.bmp", show);

	cout << "press any key to into next step !" << endl;
	cvWaitKey(0);

	return 0;
}

