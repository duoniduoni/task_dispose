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

void getCharacterPoints(Mat & src, vector<CvPoint> & points)
{
	for(int x = 0; x < src.rows; x++)
	{
		uchar *p = src.ptr<uchar>(x);
		//行内扫描
		bool flag = false;
		for(int y = 0; y < src.cols; ++y)
		{
			if(flag == false) //未找到开始点
			{
				if(y + 1 < src.cols)
				{
					if( p[y] == 255 && p[y + 1] == 0)
					{
						flag = true;
					}
				}
			}
			else
			{
				points.push_back(cvPoint(x, y));
				if(y + 1 < src.cols)
				{
					if(p[y] == 0 && p[y + 1] == 255)
					{
						flag = false;
						y += 2;
					}
				}
			}
		}
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
	threshold(src, dst, 3, 255, THRESH_BINARY_INV);
	imshow(WNAME, dst);

	cout << "press any key to into next step !" << endl;
	cvWaitKey(0);

	//找行
	/*
	vector<int> lines;
	getLines(dst, lines);

	cout << "lines.size() = " << lines.size() << endl;
	for(int i = 0; i < (int)lines.size(); i++)
		line(dst, cvPoint(0, lines[i]), cvPoint(dst.cols, lines[i]), cvScalar(255, 255, 255));
	imshow(WNAME, dst);

	cout << "press any key to into next step !" << endl;
	cvWaitKey(0);
	*/

	//初步找字符的点
	vector<CvPoint> points;
	getCharacterPoints(dst, points);
	cout << "points.size = " << points.size() << endl;

	//显示字符点查找结果
	Mat show = Mat::zeros(dst.rows, dst.cols, CV_8UC3);

	for(int x = 0; x < show.rows; ++x)
	{
		Vec3b * n = show.ptr<Vec3b>(x);
		uchar * o = dst.ptr<uchar>(x);

		for(int y = 0; y < show.cols; ++y)
		{
			if(o[y] == 255)
				n[y][0] = n[y][1] = n[y][2] = 255;
		}
	}
	/*
	Vec3b * p = show.ptr<Vec3b>(0);
	int curline = 0;
	for(int i = 0; i < points.size(); i++)
	{
		if(points[i].x != curline)
		{
			curline = points[i].x;
			p = show.ptr<Vec3b>(curline);
		}

		p[points[i].y][2] = 0;
		p[points[i].y][1] = 0;
		p[points[i].y][0] = 0;
		p[points[i].y][rand()%3] = 255;
	}*/

	Mat show2 = Mat::zeros(dst.rows, dst.cols, CV_8UC3);
	bitwise_not(show, show2);
	imshow(WNAME, show2);
	imwrite("resutl.bmp", show2);

	cout << "press any key to into next step !" << endl;
	cvWaitKey(0);

	return 0;
}

