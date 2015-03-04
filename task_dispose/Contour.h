#pragma once

#include <iostream>
#include <vector>

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

	void drawContour(Mat & show, bool drawRectange = false);
	CvRect getRectange() { return range; }

private:
	vector<CvPoint> points;				//特征所包含的点
	CvRect range;						//特征最大矩形

	void getContourRect();
};

class ContourManager
{
public:
	ContourManager();
	virtual ~ContourManager();

	void analyse(Mat & src, int filter = 6);

	int getContourCount() { return contours.size(); }
	Contour & getContour(int index) { return * contours[min(index, (int)contours.size() -1)]; }

private:
	vector<Contour *> contours;
};
