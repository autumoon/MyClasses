#pragma once

typedef struct XPOINT32Ftag
{
	double x;
	double y;

	XPOINT32Ftag(double _x = 0.0, double _y = 0.0)
	{
		x = _x;
		y = _y;
	}

}POINT32F;

//����true Ϊ�ཻ��falseΪ���ཻ
bool cxLineIntersect32F(POINT32F *p1, POINT32F *p2, POINT32F *p3, POINT32F *p4);