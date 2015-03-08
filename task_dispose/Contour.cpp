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

	range = cvRect(left, top, right - left + 1, bottom - top + 1);
}

void Contour::drawContour(Mat & show, bool drawRectange)
{
	if(!show.data)
		return ;

	if(show.channels() != 3)
		return ;

	getContourRect();

	if(range.x + range.width > show.cols || range.y + range.height > show.rows)
		return ;

	Vec3b color(0, 0, 0);
	color[0] = 100 + rand() % 155;
	color[1] = 100 + rand() % 155;
	color[2] = 100 + rand() % 155;
	Vec3b * p = show.ptr<Vec3b>(0);
	int curline = 0;

	if(drawRectange)
		rectangle(show, range, cvScalar(color[0], color[1], color[2]));

	for(int i = 0; i < points.size(); i++)
	{
		if(points[i].y != curline)
		{
			curline = points[i].y;
			p = show.ptr<Vec3b>(curline);
		}

		p[points[i].x][2] = color[2];
		p[points[i].x][1] = color[1];
		p[points[i].x][0] = color[0];
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

void ContourManager::merge()
{
	vector<Contour *> tmp = contours;
	contours.clear();

	while(tmp.size() >= 1)
	{
		Contour * record = tmp[0];
		tmp.erase(tmp.begin());

		vector<Contour *>::iterator itVec = tmp.begin();
		
		for ( ; itVec != tmp.end(); )
		{
			bool flag = false;

			Contour & con1 = *record;
			Contour & con2 = *(*itVec);

			CvPoint center1, center2;
			center1.x = con1.getRectange().x + con1.getRectange().width / 2 + con1.getRectange().width % 2;
			center1.y = con1.getRectange().y + con1.getRectange().height / 2 + con1.getRectange().height % 2;
			center2.x = con2.getRectange().x + con2.getRectange().width / 2 + con2.getRectange().width % 2;
			center2.y = con2.getRectange().y + con2.getRectange().height / 2 + con2.getRectange().height % 2;

			int width1 = con1.getRectange().width;
			int width2 = con2.getRectange().width;
			int height1 = con1.getRectange().height;
			int height2 = con2.getRectange().height;
	
			bool flag1 = abs(center1.x - center2.x) <= (width1 + width2)/2;
			bool flag2 = abs(center1.y - center2.y) <= (height1 + height2)/2;
			if( flag1 && flag2 )
			{
				//包含
				flag = true;

				for(int i = 0; i < con2.getPoints().size(); ++i)
					con1.addPoint(con2.getPoints().at(i));

				delete *itVec;
			}
			else
			{
				//不相连
			}

			// 删除
			if (flag) 
				itVec = tmp.erase(itVec);
			else
				++itVec;
		}

		contours.push_back(record);
	}

}

void ContourManager::analyse(Mat & src, int filter)
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
	for(int y = 0; y < src.rows; ++y)
	{
		uchar * rdata = src.ptr<uchar>(y);
		for(int x = 0; x < src.cols; ++x)
		{
			if(rdata[x] != 255)
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
	merge();

	//再进一步分析 省略掉特别小的点
	vector<Contour *>::iterator itVec = contours.begin();	
	for ( ; itVec != contours.end(); )
	{
		bool flag = false;

		if((*itVec)->getPoints().size() < filter)
			flag = true;

		// 删除
		if (flag) 
			itVec = contours.erase(itVec);
		else
			++itVec;
	}

	//再次合并
	merge();

	//显示消耗时间
	t = ((double)getTickCount() - t)/getTickFrequency();
	cout << "Times passed in seconds: " << t << endl;
}