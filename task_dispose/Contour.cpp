#include "stdafx.h"
#include "Contour.h"


Contour::Contour(void)
{
}


Contour::~Contour(void)
{
	points.clear();
}

bool Contour::isBelong(CvPoint & point)
{
	//获取到特征矩形中心点
	int cen_x, cen_y;
	cen_x = range.x + range.width / 2;
	cen_y = range.y + range.height / 2;
	if( abs(cen_x - point.x) > (range.width / 2 + 2) || 
		abs(cen_y - point.y) > (range.height / 2 + 2) )
		return false;

	for(int i = points.size() - 1; i >= 0; --i)
	{
		int x = points[i].x;
		int y = points[i].y;

		if(abs(x - point.x) <= 1 && abs(y - point.y) <= 1)
			return true;
	}

	return false;
}

void Contour::addPoint(CvPoint & point)
{
	points.push_back(point);

	getContourRect();
}

void Contour::getContourRect()
{
	int top = points[0].y, left = points[0].x, right = 0, bottom = 0;

	for(int i = 0; i < points.size(); ++i)
	{
		if(points[i].x < left)
			left = points[i].x;
		if(points[i].x > right)
			right = points[i].x;
		if(points[i].y < top)
			top = points[i].y;
		if(points[i].y > bottom)
			bottom = points[i].y;
	}

	range = cvRect(left, top, right - left, bottom - top);
}

void Contour::drawContour(Mat & show)
{
	if(!show.data)
		return ;

	if(show.channels() != 3)
		return ;

	getContourRect();

	if(range.x + range.width > show.cols || range.y + range.height > show.rows)
		return ;

	Vec3b color(0, 0, 0);
	color[rand()%3] = 255;
	Vec3b * p = show.ptr<Vec3b>(0);
	int curline = 0;
	for(int i = 0; i < points.size(); i++)
	{
		if(points[i].x != curline)
		{
			curline = points[i].x;
			p = show.ptr<Vec3b>(curline);
		}

		p[points[i].y][2] = color[2];
		p[points[i].y][1] = color[1];
		p[points[i].y][0] = color[0];
	}
}

////////////////////////////////////////////////////////////////////////////////////
ContourManager::ContourManager()
{
	contours.clear();
}

ContourManager::~ContourManager()
{
	for(int i = 0; i < contours.size(); ++i)
		delete contours[i];

	contours.clear();
}

void ContourManager::analyse(Mat & src)
{
	if(!src.data)
		return ;
	if(src.channels() != 1)
		return ;
	if(contours.size())
		return ;

	//开始计时
	double t = (double)getTickCount();

	//初步分析
	for(int x = 0; x < src.rows; ++x)
	{
		uchar * rdata = src.ptr<uchar>(x);
		for(int y = 0; y < src.cols; ++y)
		{
			if(rdata[y] != 255)
				continue;

			if(contours.size() == 0)
			{
				Contour * con = new Contour;
				con->addPoint(cvPoint(x, y));

				contours.push_back(con);

				continue;
			}

			int i;
			for(i = 0; i < contours.size(); ++i)
			{
				Contour & con = *contours[i];

				if(con.isBelong(cvPoint(x, y)))
				{
					con.addPoint(cvPoint(x, y));
					break;
				}
			}
			if( i == contours.size() )
			{
				Contour * con = new Contour;
				con->addPoint(cvPoint(x, y));

				contours.push_back(con);
			}
		}
	}

	//进一步分析 进行合并

	//显示消耗时间
	t = ((double)getTickCount() - t)/getTickFrequency();
	cout << "Times passed in seconds: " << t << endl;
}