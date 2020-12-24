#include "Segment.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef PAI
#define PAI   3.1415926535897932384626433832795
#endif // PAI

#ifndef DB_MAX
#define DB_MAX         1.7976931348623158e+308 /* max value */
#endif // DB_MAX

CSegment::CSegment()
{
}

CSegment::CSegment(POINT32F p1, POINT32F p2)
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

bool CSegment::IntersectionLine(CSegment& os, POINT32F& pInter)
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

	if (fabs(D) < 1e-5)
	{
		//无交点
		return false;
	}

	pInter.x = (b0 * c1 - b1 * c0) / D;
	pInter.y = (c0 * a1 - c1 * a0) / D;

	return true;
}

POINT32F CSegment::GetVerticalPoint(const POINT32F& pt)
{
	if (dblcmp(Slope(), DBL_MAX) == 0)
	{
		//如果是垂直的直线
		double x = x1;
		double y = pt.y;
		return POINT32F(x, y);
	}

	if (dblcmp(Slope(), 0.0) == 0)
	{
		//如果是水平的直线
		double x = pt.x;
		double y = y1;
		return POINT32F(x, y);
	}

	//四舍五入
	double x = (Slope()* x1 + pt.y + 1 / Slope() * pt.x - y1) /(Slope() + 1 / Slope());
	double y = Slope() * x + y1 - Slope() * x1;

	return POINT32F(x, y);
}

MyRect CSegment::GetSquare(const int& nImgWidth, const int& nImgHeight, const int nSideLength)
{
	POINT32F center = GetCenter();

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
	return atan(Slope()) / PAI * 180;
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

std::vector<POINT32F> CSegment::Raster(const double& lfPtGap /*= 1.0*/)
{
	int x1R = static_cast<int>(x1 / lfPtGap + 0.5);
	int	y1R = static_cast<int>(y1 / lfPtGap + 0.5);
	int x2R = static_cast<int>(x2 / lfPtGap + 0.5);
	int y2R = static_cast<int>(y2 / lfPtGap + 0.5);

	std::vector<POINT32F> vPts;
	//增加第一个端点
	POINT32F ptStart(x1R, y1R);
	POINT32F ptEnd(x2R, y2R);

	int dx = abs(x2R - x1R), dy = abs(y2R - y1R), yy = 0;

	if (dx < dy)
	{
		yy = 1;
		std::swap(x1R, y1R);
		std::swap(x2R, y2R);
		std::swap(dx, dy);
	}

	int ix = (x2R - x1R) > 0 ? 1 : -1, iy = (y2R - y1R) > 0 ? 1 : -1, cx = x1R, cy = y1R, n2dy = dy * 2, n2dydx = (dy - dx) * 2, d = dy * 2 - dx;
	if (yy) { // 如果直线与 x 轴的夹角大于 45 度
		while (cx != x2R)
		{
			if (d < 0)
			{
				d += n2dy;
			}
			else
			{
				cy += iy;
				d += n2dydx;
			}
			vPts.push_back(POINT32F(cy, cx));
			cx += ix;
		}
	}
	else
	{
		// 如果直线与 x 轴的夹角小于 45 度
		while (cx != x2R)
		{
			if (d < 0)
			{
				d += n2dy;
			}
			else
			{
				cy += iy;
				d += n2dydx;
			}
			vPts.push_back(POINT32F(cx, cy));
			cx += ix;
		}
	}

	for (size_t i = 0; i < vPts.size(); ++i)
	{
		vPts[i].x *= lfPtGap;
		vPts[i].y *= lfPtGap;
	}

	return vPts;
}

bool CSegment::IsLineInterRect(const double& a, const double& b, const double& c, const MyRect& rect, POINT32F& pt1, POINT32F& pt2)
{
	//如果ab都为零，则返回
	if (dblcmp(a, 0.0) == 0 && dblcmp(b, 0) == 0)
	{
		return false;
	}

	//如果平行于y
	if (dblcmp(a, 0.0) == 0)
	{
		pt1.x = rect.x;
		pt2.x = rect.x + rect.width;
		pt1.y = pt2.y = -c / b;

		return pt1.y >= rect.y && pt1.y <= rect.y + rect.height;
	}

	//如果平行于x
	if (dblcmp(b, 0.0) == 0)
	{
		pt1.x = pt2.x = -c / a;
		pt1.y = rect.y;
		pt1.y = rect.y + rect.height;

		return pt1.x >= rect.x && pt1.x <= rect.x + rect.width;
	}

	//计算交点
	POINT32F ptLD(rect.x, rect.y);
	POINT32F ptLU(rect.x, rect.y + rect.height);
	POINT32F ptRU(rect.x + rect.width, rect.y + rect.height);
	POINT32F ptRD(rect.x + rect.width, rect.y);
	POINT32F ptInter;
	bool bFindPt = false;
	if (IsLineInterSeg(a, b, c, ptLD, ptLU, ptInter) && PointOnSeg(ptInter, ptLD, ptLU) <= 0)
	{
		pt1 = ptInter;
		bFindPt = true;
	}

	if (IsLineInterSeg(a, b, c, ptLU, ptRU, ptInter) && PointOnSeg(ptInter, ptLU, ptRU) <= 0)
	{
		if (bFindPt)
		{
			pt2 = ptInter;
			return true;
		}
		else
		{
			bFindPt = true;
			pt1 = ptInter;
		}
	}

	if (IsLineInterSeg(a, b, c, ptRU, ptRD, ptInter) && PointOnSeg(ptInter, ptRU, ptRD) <= 0)
	{
		if (bFindPt)
		{
			pt2 = ptInter;
			return true;
		}
		else
		{
			bFindPt = true;
			pt1 = ptInter;
		}
	}

	if (IsLineInterSeg(a, b, c, ptRD, ptLD, ptInter) && PointOnSeg(ptInter, ptRD, ptLD) <= 0)
	{
		if (bFindPt)
		{
			pt2 = ptInter;
			return true;
		}
		else
		{
			bFindPt = true;
			pt1 = ptInter;
		}
	}

	return false;
}

double CSegment::GapFromPoint(const POINT32F& pt)
{
	double k = Slope();

	if (dblcmp(k, DBL_MAX) == 0)
	{
		//垂直的时候直接返回差值
		return fabs(x1 - pt.x);
	}

	double b = y1 - k * x1;

	return fabs(k * pt.x - pt.y + b) / sqrt( 1 + k * k);
}

double CSegment::Slope()
{
	if (dblcmp(x1, x2) == 0)
	{
		return DB_MAX;
	}

	return (y1 - y2) / (x1 - x2);
}

int CSegment::IsAbInterCdSeg(POINT32F a,POINT32F b,POINT32F c,POINT32F d, POINT32F& p)
{
	double s1,s2,s3,s4;
	int d1,d2,d3,d4;
	d1=dblcmp(s1=ab_cross_ac(a,b,c),0);
	d2=dblcmp(s2=ab_cross_ac(a,b,d),0);
	d3=dblcmp(s3=ab_cross_ac(c,d,a),0);
	d4=dblcmp(s4=ab_cross_ac(c,d,b),0);

	//如果规范相交则求交点
	if ((d1^d2)==-2 && (d3^d4)==-2)
	{
		p.x=(c.x*s2-d.x*s1)/(s2-s1);
		p.y=(c.y*s2-d.y*s1)/(s2-s1);
		return 1;
	}

	//如果不规范相交
	if (d1==0 && PointOnSeg(c,a,b)<=0)
	{
		p=c;
		return 0;
	}
	if (d2==0 && PointOnSeg(d,a,b)<=0)
	{
		p=d;
		return 0;
	}
	if (d3==0 && PointOnSeg(a,c,d)<=0)
	{
		p=a;
		return 0;
	}
	if (d4==0 && PointOnSeg(b,c,d)<=0)
	{
		p=b;
		return 0;
	}

	//如果不相交

	return -1;
}

int CSegment::IsLineInterLine(const double& a1, const double& b1, const double& c1, const double& a2, const double& b2, const double& c2, POINT32F& ptInter)
{
	if (dblcmp(a1, 0.0) == 0 && dblcmp(b1, 0.0) == 0 || dblcmp(a2, 0.0) == 0 && dblcmp(b2, 0.0) == 0)
	{
		return -1;
	}

	double D = a1 * b2 - a2 * b1;

	if (dblcmp(D, 0.0) == 0)
	{
		return 1;
	}

	ptInter.x = (b1 * c2 - b2 * c1) / D;
	ptInter.y = (c1 * a2 - c2 * a1) / D;

	return 0;
}

int CSegment::IsLineInterSeg(const double& a, const double& b, const double& c, POINT32F pt1, POINT32F pt2, POINT32F& ptInter)
{
	if (dblcmp(a, 0.0) == 0 && dblcmp(b, 0.0) == 0)
	{
		return -1;
	}

	double dXMin = min(pt1.x, pt2.x);
	double dXMax = max(pt1.x, pt2.x);
	double dYMin = min(pt1.y, pt2.y);
	double dYMax = max(pt1.y, pt2.y);

	//如果直线平行于y
	if (dblcmp(a, 0.0) == 0)
	{
		ptInter.y = -c / b;

		if (pt1.y >= dYMin && pt1.y <= dYMax)
		{
			if (CalcLineXByABC(a, b, c, ptInter.x, ptInter.y) == 0)
			{
				return 1;
			}
		}
	}

	//如果直线平行于x
	if (dblcmp(b, 0.0) == 0)
	{
		ptInter.x = -c / a;
		if (pt1.x >= dXMin && pt1.x <= dXMax)
		{
			if (CalcLineYByABC(a, b, c, ptInter.x, ptInter.y) == 0)
			{
				return 1;
			}
		}
	}

	if (dblcmp(pt1.x, pt2.x) == 0 && dblcmp(pt1.y, pt2.y) == 0)
	{
		return -1;
	}

	//线段是垂直于x轴的
	if (dblcmp(pt1.x, pt2.x) == 0)
	{
		ptInter.x = pt1.x;
		CalcLineYByABC(a, b, c, ptInter.x, ptInter.y);

		if (ptInter.y >= dYMin && ptInter.y <= dYMax)
		{
			return 1;
		}
		
	}

	if (dblcmp(pt1.y, pt2.y) == 0)
	{
		ptInter.y = pt1.y;
		CalcLineXByABC(a, b, c, ptInter.x, ptInter.y);

		if (ptInter.x >= dXMin && ptInter.x <= dXMax)
		{
			return 1;
		}
	}

	//求得交点
	double a2 = 0.0, b2 = 0.0, c2 = 0.0;
	CalcLineABC(pt1.x, pt1.y, pt2.x, pt2.y, a2, b2, c2);

	//求得交点
	int nRes = IsLineInterLine(a, b, c, a2, b2, c2, ptInter);

	return nRes == 0 ? 1 : 2;
}

int CSegment::CalcLineABC(double x1, double y1, double x2, double y2, double& a, double& b, double& c)
{
	if (dblcmp(x1, x2) == 0 && dblcmp(y1, y2) == 0)
	{
		return -1;
	}

	if (dblcmp(x1, x2) == 0)
	{
		a = 1;
		b = 0;
		c = -x1;

		return 1;
	}

	if (dblcmp(y1, y2) == 0)
	{
		a = 0;
		b = -1.0;
		c = y1;

		return 2;
	}

	a = (y1 - y2) / (x1 - x2);
	b = -1.0;
	c = y1 - a * x1;

	return 0;
}

int CSegment::CalcLineABC(double dAngle, double x, double y, double& a, double& b, double& c)
{
	if (dblcmp(dAngle, 90.0) == 0 || dblcmp(dAngle, 270.0) == 0)
	{
		a = 1.0;
		b = 0;
		c = -x;

		return 1;
	}


	if (dblcmp(dAngle, 0.0) == 0 || dblcmp(dAngle, 180.0) == 0)
	{
		a = 0.0;
		b = 1.0;
		c = -y;

		return 2;
	}

	a = tan(dAngle / 180.0 * PAI);
	b = -1.0;
	c = y - a * x;

	return 0;
}

int CSegment::AngleSeg(double x1, double y1, double x2, double y2, int flag /*= 0*/)
{
	switch (flag)
	{
	case 0:
		{
			//加上360然后取膜的好处是使得得到的结果为与x轴正向的夹角
			int nDgree = int(atan2(y2 - y1, x2 - x1) * 180 / PAI) + 360;

			return nDgree % 360;

		}
		break;
	case 1:
		{
			int nDgree = atan2(y2 - y1, x2 - x1) * 180 / PAI;

			return nDgree;
		}
		break;
	default:
		break;
	}

	return 0;
}
