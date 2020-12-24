//author:autumoon

#pragma once
#include <vector>

 //点
typedef struct _POINT32F
{
	double x;
	double y;

	_POINT32F(double x = 0.0, double y = 0.0)
	{
		this->x = x;
		this->y = y;
	}

}POINT32F;

//正矩形
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
	CSegment(POINT32F p1, POINT32F p2);
	CSegment(const double& x1, const double& y1, const double& x2, const double& y2);
	~CSegment();

public:
	double x1, y1, x2, y2;        //线段的断点坐标

public:
	//两直线的交点
	bool IntersectionLine(CSegment& os, POINT32F& pInter);
	//两线段的交点
	bool IntersectionSeg(CSegment& os, POINT32F& pInter){return IntersectionLine(os, pInter) && IsPointOnSeg(pInter) && os.IsPointOnSeg(pInter);}
	//判断线段是否与某个矩形区域相交
	bool IsPartInRect(const MyRect& rect)
	{
		return x1 >= rect.x && x1 <= rect.x + rect.width && y1 >= rect.y && y1 <= rect.y + rect.height ||
			x2 >= rect.x && x2 <= rect.x + rect.width && y2 >= rect.y && y2 <= rect.y + rect.height;
	}
	//点是否在直线上
	bool IsPointOnLine(const POINT32F& pt, const double& dDiff = 0.01){return GapFromPoint(pt) < dDiff;}
	//点是否在线段上
	bool IsPointOnSeg(const POINT32F& pt, const double& dDiff = 1.0){return Gap(pt, POINT32F(x1, y1)) + Gap(pt, POINT32F(x2, y2)) - Length() < dDiff;}
	//判断两直线是否垂直，定义最大误差角度
	bool IsPerpendicular(CSegment& os, const double& dDiff = 1.0){return fabs(Angle() - os.Angle()) > 90.0 - dDiff && fabs(Angle() - os.Angle()) < 90 + dDiff;}

	//与x轴正向的夹角,范围为[-90.0,90.0]
	double Angle();
	//求距离
	double Gap(const POINT32F& p1, const POINT32F& p2){return Gap(p1.x, p1.y, p2.x, p2.y);}
	double Gap(const double& x1, const double& y1, const double& x2, const double& y2){return sqrt((y1 - y2) * (y1 - y2) + (x1 - x2) * (x1 - x2));}

	//长度
	double Length(){return sqrt((y1 - y2) * (y1 - y2) + (x1 - x2) * (x1 - x2));}

	double GapFromPoint(const POINT32F& pt);

	//斜率
	double Slope();

	//偏移线段
	int Offset(const double& x, const double& y){x1 += x;y1 += y;x2 += x;y2 += y;return 0;}

	//获取线段的中点
	POINT32F GetCenter(){return POINT32F((x1 + x2) / 2, (y1 + y2) / 2);}
	//获取点到直线的垂线段与直线的交点
	POINT32F GetVerticalPoint(const POINT32F& pt);

	//获取线段在某张图片中的正方形区域
	MyRect GetSquare(const int& nImgWidth, const int& nImgHeight, const int nSideLength);
	//获取线段所在的最小矩形区域，定义最小宽度
	MyRect GetRect(const int& nMinGap = 8);
	//获取线段在图像内部的最小矩形区域，定义最小宽度
	MyRect GetRect(const int& nImgWidth, const int& nImgHeight, const int& nMinGap = 8);

	//线段栅格化
	std::vector<POINT32F> Raster(const double& lfPtGap = 1.0);
	
	//直线与正矩形的交点
	static bool IsLineInterRect(const double& a, const double& b, const double& c, const MyRect& rect, POINT32F& pt1, POINT32F& pt2);

	//求线段ab是否与线段cd相交，交点为p。1规范相交，0交点是一线段的端点，-1不相交。
	static int IsAbInterCdSeg(POINT32F a, POINT32F b, POINT32F c, POINT32F d, POINT32F& pInter);
	//判断直线是否相交，-1异常，0相交，1平行, 2重合
	static int IsLineInterLine(const double& a1, const double& b1, const double& c1, const double& a2, const double& b2, const double& c2, POINT32F& ptInter);
	//判断直线是否与线段相交，-1异常，0交点是一线段的端点, 1规范相交，2平行, 3重合
	static int IsLineInterSeg(const double& a, const double& b, const double& c, POINT32F pt1, POINT32F pt2, POINT32F& ptInter);
	//根据两点 求解ax + by + c = 0的直线方程的参数，-1异常，0正常，1垂直于x，2垂直于y
	static int CalcLineABC(double x1, double y1, double x2, double y2, double& a, double& b, double& c);
	//根据角度[0.0 - 360.0] 求解ax + by + c = 0的直线方程的参数
	static int CalcLineABC(double dAngle, double x, double y, double& a, double& b, double& c);
	//求解直线上的某个点
	static int CalcLineXByABC(const double& a, const double& b, const double& c, double& x, const double& y){if (dblcmp(a, 0.0) == 0)return -1;x = (-b * y - c) / a;return 0;}
	static int CalcLineYByABC(const double& a, const double& b, const double& c, const double& x, double& y){if (dblcmp(b, 0.0) == 0)return -1;y = (-a * x - c) / b;return 0;}

	//默认范围为[0，360], flag为1时，范围为[-180, 180]
	static int AngleSeg(double x1, double y1, double x2, double y2, int flag = 0);
	//求a点是不是在线段bc上，>0不在，=0与端点重合，<0在。
	static int PointOnSeg(POINT32F a,POINT32F b,POINT32F c){return dblcmp(dot(b.x-a.x,b.y-a.y,c.x-a.x,c.y-a.y), 0.0);}

private:
	//近似比较大小
	static int dblcmp(double a,double b, double lfSnap = 1e-5){if (fabs(a-b) <= lfSnap) return 0;if (a>b) return 1;else return -1;}

	//点积
	static double dot(double x1,double y1,double x2,double y2){return x1*x2+y1*y2;}

	static double cross(double x1,double y1,double x2,double y2){return x1*y2-x2*y1;}

	//ab与ac的叉积
	static double ab_cross_ac(POINT32F a,POINT32F b,POINT32F c){return cross(b.x-a.x,b.y-a.y,c.x-a.x,c.y-a.y);}
};

