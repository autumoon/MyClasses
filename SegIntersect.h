#pragma once

typedef struct XPOINT32Ftag
{
	double x;
	double y;

	XPOINT32Ftag(double _x, double _y)
	{
		x = _x;
		y = _y;
	}

}POINT32F;

//返回true 为相交，false为不相交
bool cxLineIntersect32F(POINT32F *p1, POINT32F *p2, POINT32F *p3, POINT32F *p4);