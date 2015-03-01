#pragma once

#include <iostream>
#include <vector>
#include <list>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

class Contour
{
public:
	Contour(void);
	virtual ~Contour(void);

	bool isBelong(CvPoint & point);
	vector<CvPoint> & getPoints() { return points; }
	void addPoint(CvPoint & point);

	void drawContour(Mat & show);

private:
	vector<CvPoint> points;				//�����������ĵ�
	CvRect range;						//����������

	void getContourRect();
};

class ContourManager
{
public:
	ContourManager();
	virtual ~ContourManager();

	void analyse(Mat & src);

	int getContourCount() { return contours.size(); }
	Contour & getContour(int index) { if(index < contours.size()) return * contours[index]; }

private:
	vector<Contour *> contours;
};
