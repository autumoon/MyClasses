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

//һ���߶Σ��������յ�
class CSegment
{
public:
	CSegment();
	CSegment(MyPoint p1, MyPoint p2);
	CSegment(const double& x1, const double& y1, const double& x2, const double& y2);
	~CSegment();

public:
	double x1, y1, x2, y2;        //�߶εĶϵ�����

public:
	//�ж���ֱ���Ƿ�ƽ�У�����������Ƕ�
	bool AngleDiffAbs(CSegment& os, const double& dDiff = 1.0);
	//��ֱ�ߵĽ���
	bool IntersectionLine(CSegment& os, MyPoint& pInter);
	//���߶εĽ���
	bool IntersectionSeg(CSegment& os, MyPoint& pInter);
	//�ж��Ƿ���ĳ�����������ཻ
	bool IsPartInRect(const MyRect& rect);
	//���Ƿ���ֱ����
	bool IsPointInLine(const MyPoint& pt, const double& dDiff = 0.01);
	//���Ƿ����߶���
	bool IsPointInSeg(const MyPoint& pt, const double& dDiff = 1.0);
	//�ж���ֱ���Ƿ�ֱ������������Ƕ�
	bool VerAngleDiffAbs(CSegment& os, const double& dDiff = 1.0);

	//��ȡ�߶ε��е�
	MyPoint GetCenter();
	//��ȡ�㵽ֱ�ߵĴ��߶���ֱ�ߵĽ���
	MyPoint GetVerticalPoint(const MyPoint& pt);
	//��ȡ�߶���ĳ��ͼƬ�е�����������
	MyRect GetSquare(const int& nImgWidth, const int& nImgHeight, const int nSideLength);
	//��ȡ�߶����ڵ���С�������򣬶�����С���
	MyRect GetRect(const int& nMinGap = 8);
	//��ȡ�߶���ͼ���ڲ�����С�������򣬶�����С���
	MyRect GetRect(const int& nImgWidth, const int& nImgHeight, const int& nMinGap = 8);

	//��x������ļн�,-90��+90��
	double Angle();
	//�����߶��е�ľ���
	double CenterGap(CSegment& os);
	//����
	double Length();

	double GapFromPoint(const MyPoint& pt);
	//б��
	double Slope();

	int Offset(const int& x, const int& y);

private:
	double TwoPointsGap(const MyPoint& p1, const MyPoint& p2);
	double TwoPointsGap(const double& x1, const double& y1, const double& x2, const double& y2);
};

