#pragma once
#include <vector>

#define PI   3.1415926535897932384626433832795
#define DB_MAX         1.7976931348623158e+308 /* max value */

typedef struct _MyPoint
{
	double x;
	double y;

	_MyPoint(double x, double y)
	{
		this->x = x;
		this->y = y;
	}

}MyPoint;

typedef struct _MyRect
{
	double x;
	double y;
	double width;
	double height;

	_MyRect(double x, double y, double width, double height)
	{
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
	}

}MyRect;

//一条线段，有起点和终点
class CSegment
{
public:
	CSegment();
	CSegment(MyPoint p1, MyPoint p2);
	CSegment(const double& x1, const double& y1, const double& x2, const double& y2);
	~CSegment();

public:
	double x1, y1, x2, y2;        //线段的断点坐标

public:
	//判断两直线是否平行，定义最大误差角度
	bool AngleDiffAbs(CSegment& os, const double& dDiff = 1.0);
	//两直线的交点
	bool IntersectionLine(CSegment& os, MyPoint& pInter);
	//两线段的交点
	bool IntersectionSeg(CSegment& os, MyPoint& pInter);
	//判断是否与某个矩形区域相交
	bool IsPartInRect(const MyRect& rect);
	//点是否在直线上
	bool IsPointInLine(const MyPoint& pt, const double& dDiff = 0.01);
	//点是否在线段上
	bool IsPointInSeg(const MyPoint& pt, const double& dDiff = 1.0);
	//判断两直线是否垂直，定义最大误差角度
	bool VerAngleDiffAbs(CSegment& os, const double& dDiff = 1.0);

	//获取线段的中点
	MyPoint GetCenter();
	//获取点到直线的垂线段与直线的交点
	MyPoint GetVerticalPoint(const MyPoint& pt);
	//获取线段在某张图片中的正方形区域
	MyRect GetSquare(const int& nImgWidth, const int& nImgHeight, const int nSideLength);
	//获取线段所在的最小矩形区域，定义最小宽度
	MyRect GetRect(const int& nMinGap = 8);
	//获取线段在图像内部的最小矩形区域，定义最小宽度
	MyRect GetRect(const int& nImgWidth, const int& nImgHeight, const int& nMinGap = 8);

	//与x轴正向的夹角,-90到+90度
	double Angle();
	//两条线段中点的距离
	double CenterGap(CSegment& os);
	//长度
	double Length();

	double GapFromPoint(const MyPoint& pt);
	//斜率
	double Slope();

	int Offset(const int& x, const int& y);

private:
	double TwoPointsGap(const MyPoint& p1, const MyPoint& p2);
	double TwoPointsGap(const double& x1, const double& y1, const double& x2, const double& y2);
};

