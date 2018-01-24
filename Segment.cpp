#include "Segment.h"

CSegment::CSegment()
{
}


CSegment::CSegment(MyPoint p1, MyPoint p2)
{
	x1 = p1.x;
	y1 = p1.y;
	x2 = p2.x;
	y2 = p2.y;
}

CSegment::CSegment(const double& x1, const double& y1, const double& x2, const double& y2)
{
	this->x1 = x1;
	this->y1 = y1;
	this->x2 = x2;
	this->y2 = y2;
}

CSegment::~CSegment()
{

}

bool CSegment::AngleDiffAbs(CSegment& os, const double& dDiff /*= 1.0*/)
{
	return fabs(Angle() - os.Angle()) <= dDiff;
}

bool CSegment::IntersectionLine(CSegment& os, MyPoint& pInter)
{
	//第一条直线的参数
	double a0 = y1 - y2;
	double b0 = x2 - x1;
	double c0 = x1 * y2 - x2 * y1;

	double x3 = os.x1;
	double y3 = os.y1;
	double x4 = os.x2;
	double y4 = os.y2;

	//第二条直线的参数
	double a1 = y3 - y4;
	double b1 = x4 - x3;
	double c1 = x3 * y4 - x4 * y3;

	//求交点
	double D = a0 * b1 - a1 * b0;

	if (fabs(D) < 0.00001)
	{
		//无交点
		return false;
	}

	pInter.x = (b0 * c1 - b1 * c0) / D;
	pInter.y = (c0 * a1 - c1 * a0) / D;

	return true;
}

bool CSegment::IntersectionSeg( CSegment& os, MyPoint& pInter )
{
	bool bRes = IntersectionLine(os, pInter);
	return bRes && IsPointInSeg(pInter) && os.IsPointInSeg(pInter);
}

bool CSegment::IsPartInRect(const MyRect& rect)
{
	return x1 >= rect.x && x1 <= rect.x + rect.width && y1 >= rect.y && y1 <= rect.y + rect.height ||
		x2 >= rect.x && x2 <= rect.x + rect.width && y2 >= rect.y && y2 <= rect.y + rect.height;
}

bool CSegment::IsPointInLine(const MyPoint& pt, const double& dDiff/* = 0.01*/)
{
	return GapFromPoint(pt) <= dDiff;
}

bool CSegment::IsPointInSeg(const MyPoint& pt, const double& dDiff/* = 1.0*/)
{
	//考虑到图像的特殊性，不需要斜率相同，只需要用距离判断即可
	return TwoPointsGap(pt, MyPoint(x1, y1)) + TwoPointsGap(pt, MyPoint(x2, y2)) - Length() <= dDiff;
}

bool CSegment::VerAngleDiffAbs(CSegment& os, const double& dDiff /*= 1.0*/)
{
	return fabs(Angle() - os.Angle()) >= 90.0 - dDiff && fabs(Angle() - os.Angle()) <= 90 + dDiff;
}

MyPoint CSegment::GetCenter()
{
	return MyPoint((x1 + x2) / 2, (y1 + y2) / 2);
}

MyPoint CSegment::GetVerticalPoint(const MyPoint& pt)
{
	if (Slope() == DBL_MAX)
	{
		//如果是垂直的直线
		double x = x1;
		double y = pt.y;
		return MyPoint(x, y);
	}

	if (fabs(Slope()) <= 0.00001)
	{
		//如果是水平的直线
		double x = pt.x;
		double y = y1;
		return MyPoint(x, y);
	}

	//四舍五入
	double x = (Slope()* x1 + pt.y + 1 / Slope() * pt.x - y1) /(Slope() + 1 / Slope());
	double y = Slope() * x + y1 - Slope() * x1;

	return MyPoint(x, y);
}

MyRect CSegment::GetSquare(const int& nImgWidth, const int& nImgHeight, const int nSideLength)
{
	MyPoint center = GetCenter();

	int x = center.x - nSideLength / 2;
	int y = center.y - nSideLength / 2;
	int width = nSideLength;
	int height = nSideLength;

	if (x < 0)
	{
		x = 0;
		width = center.x + nSideLength / 2;
	}

	if (y < 0)
	{
		y = 0;
		height = center.y + nSideLength / 2;
	}

	if (x + width > nImgWidth)
	{
		width = nImgWidth - x;
	}

	if (y + height > nImgHeight)
	{
		height = nImgHeight - y;
	}

	return MyRect(x, y, width, height);
}

double CSegment::Angle()
{
	return atan(Slope()) / PI * 180;
}

double CSegment::Slope()
{
	if (fabs(x1 - x2) <= 0.0001)
	{
		return DB_MAX;
	}

	return (y1 - y2) / (x1 - x2);
}

double CSegment::Length()
{
	return sqrt((y1 - y2) * (y1 - y2) + (x1 - x2) * (x1 - x2));
}

MyRect CSegment::GetRect(const int& nMinGap /*= 8*/)
{
	//只是得到矩形区域，可能为负
	int x = x1 < x2 ? x1 : x2;
	if (abs(y2 - y1) <= nMinGap)
	{
		int y = (y1 + y2) / 2 - nMinGap / 2;
		int width = abs(x1 - x2);
		if (width <= nMinGap)
		{
			x = (x1 + x2) / 2 - nMinGap / 2;
			width = nMinGap;
		}

		int height = nMinGap;
		return MyRect(x, y, width, height);
	}
	else
	{
		int y = y1 < y2 ? y1 : y2;
		int width = abs(x1 - x2);
		if (width <= nMinGap)
		{
			x = (x1 + x2) / 2 - nMinGap / 2;
			width = nMinGap;
		}
		int height = abs(y1 - y2);
		return MyRect(x, y, width, height);
	}
}

MyRect CSegment::GetRect(const int& nImgWidth, const int& nImgHeight, const int& nMinGap /*= 8*/)
{
	MyRect roi = GetRect(nMinGap);

	if (roi.x < 0)
	{
		roi.x = 0;
	}

	if (roi.x + roi.width > nImgWidth)
	{
		roi.width = nImgWidth - roi.x;
	}

	if (roi.y < 0)
	{
		roi.y = 0;
	}

	if (roi.y + roi.height > nImgHeight)
	{
		roi.height = nImgHeight - roi.y;
	}
	
	return roi;
}

double CSegment::CenterGap(CSegment& os)
{
	MyPoint p1 = GetCenter();
	MyPoint p2 = os.GetCenter();

	return TwoPointsGap(p1, p2);
}

double CSegment::GapFromPoint(const MyPoint& pt)
{
	double k = Slope();

	if (k == DBL_MAX)
	{
		//垂直的时候直接返回差值
		return fabs(x1 - pt.x);
	}

	double b = y1 - k * x1;

	return fabs(k * pt.x - pt.y + b) / sqrt( 1 + k * k);
}

int CSegment::Offset(const int& x, const int& y)
{
	x1 += x;
	y1 += y;
	x2 += x;
	y2 += y;

	return 0;
}

double CSegment::TwoPointsGap(const MyPoint& p1, const MyPoint& p2)
{
	return TwoPointsGap(p1.x, p1.y, p2.x, p2.y);
}

double CSegment::TwoPointsGap(const double& x1, const double& y1, const double& x2, const double& y2)
{
	return sqrt((y1 - y2) * (y1 - y2) + (x1 - x2) * (x1 - x2));
}


