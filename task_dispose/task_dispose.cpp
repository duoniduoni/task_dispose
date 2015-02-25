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

bool isLineEmpty(uchar * p, int len)
{
	for(int i = 0; i < len; i++)
		if(p[i] != 0)
			return false;

	return true;
}

void getLines(Mat & src, vector<int> & lines)
{
	if(src.channels() != 1)
	{
		lines.clear();
		return ;
	}

	bool find_begin = false, find_end = false;

	for(int i = 0; i < src.rows; i++)
	{
		uchar *p = src.ptr<uchar>(i);
		if(find_begin == false)
		{
			if(!isLineEmpty(p, src.cols))
			{
				find_begin = true;
				lines.push_back(i);
			}
		}
		else
		{
			if(isLineEmpty(p, src.cols))
			{
				find_end = true;
				lines.push_back(i);
			}
		}

		if(find_begin == true && find_end == true)
			find_begin = find_end = false;
	}
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
	threshold(src, dst, 1, 255, THRESH_BINARY_INV);
	imshow(WNAME, dst);

	cout << "press any key to into next step !" << endl;
	cvWaitKey(0);

	//找行
	vector<int> lines;
	getLines(dst, lines);

	cout << "lines.size() = " << lines.size() << endl;
	for(int i = 0; i < (int)lines.size(); i++)
		line(dst, cvPoint(0, lines[i]), cvPoint(dst.cols, lines[i]), cvScalar(255, 255, 255));
	imshow(WNAME, dst);

	cout << "press any key to into next step !" << endl;
	cvWaitKey(0);

	return 0;
}

