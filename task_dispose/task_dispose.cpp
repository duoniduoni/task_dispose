// task_dispose.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <vector>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

#define WNAME "window_for_show"


CvRect getContourRect(vector<Point> & contour)
{
	int top = contour[0].y, left = contour[0].x, right = 0, bottom = 0;

	for(int i = 0; i < contour.size(); ++i)
	{
		if(contour[i].x < left)
			left = contour[i].x;

		if(contour[i].x > right)
			right = contour[i].x;

		if(contour[i].y < top)
			top = contour[i].y;

		if(contour[i].y > bottom)
			bottom = contour[i].y;
	}

	return cvRect(left, top, right - left, bottom - top);
}

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

	cout << "press any key to into next step !" << endl;
	cvWaitKey(0);

	//取得二值化图片中所有特征外部轮廓
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours( dst, contours, hierarchy, CV_RETR_EXTERNAL/*CV_RETR_TREE*/, CV_CHAIN_APPROX_NONE );

	//对轮廓进行统计
	vector<int> heights, pos;
	for( int i = 0; i< contours.size(); i++ )
    {
		//计算轮廓面积 舍弃过小的
		double area = contourArea(contours[i]);
		if(area < 20)
			continue;

		//获取轮廓矩形
		CvRect rect = getContourRect(contours[i]);

		//记录高度
		heights.push_back(rect.height);
		//记录行中央位置
		pos.push_back(rect.y + rect.height/2);
    }

	sort(heights.begin(), heights.end());
	sort(pos.begin(), pos.end());

	int i;
	for(i = 0; i < heights.size(); ++i)
		cout << heights[i] << endl;

	for(i = 0; i < pos.size(); ++i)
		cout << pos[i] << endl;

	cout << "press any key to into next step !" << endl;
	cvWaitKey(0);

	return 0;
}

