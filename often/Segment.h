//author:autumoon

#pragma once
#include <vector>

 //��
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

//������
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
	CSegment(POINT32F p1, POINT32F p2);
	CSegment(const double& x1, const double& y1, const double& x2, const double& y2);
	~CSegment();

public:
	double x1, y1, x2, y2;        //�߶εĶϵ�����

public:
	//��ֱ�ߵĽ���
	bool IntersectionLine(CSegment& os, POINT32F& pInter);
	//���߶εĽ���
	bool IntersectionSeg(CSegment& os, POINT32F& pInter){return IntersectionLine(os, pInter) && IsPointOnSeg(pInter) && os.IsPointOnSeg(pInter);}
	//�ж��߶��Ƿ���ĳ�����������ཻ
	bool IsPartInRect(const MyRect& rect)
	{
		return x1 >= rect.x && x1 <= rect.x + rect.width && y1 >= rect.y && y1 <= rect.y + rect.height ||
			x2 >= rect.x && x2 <= rect.x + rect.width && y2 >= rect.y && y2 <= rect.y + rect.height;
	}
	//���Ƿ���ֱ����
	bool IsPointOnLine(const POINT32F& pt, const double& dDiff = 0.01){return GapFromPoint(pt) < dDiff;}
	//���Ƿ����߶���
	bool IsPointOnSeg(const POINT32F& pt, const double& dDiff = 1.0){return Gap(pt, POINT32F(x1, y1)) + Gap(pt, POINT32F(x2, y2)) - Length() < dDiff;}
	//�ж���ֱ���Ƿ�ֱ������������Ƕ�
	bool IsPerpendicular(CSegment& os, const double& dDiff = 1.0){return fabs(Angle() - os.Angle()) > 90.0 - dDiff && fabs(Angle() - os.Angle()) < 90 + dDiff;}

	//��x������ļн�,��ΧΪ[-90.0,90.0]
	double Angle();
	//�����
	double Gap(const POINT32F& p1, const POINT32F& p2){return Gap(p1.x, p1.y, p2.x, p2.y);}
	double Gap(const double& x1, const double& y1, const double& x2, const double& y2){return sqrt((y1 - y2) * (y1 - y2) + (x1 - x2) * (x1 - x2));}

	//����
	double Length(){return sqrt((y1 - y2) * (y1 - y2) + (x1 - x2) * (x1 - x2));}

	double GapFromPoint(const POINT32F& pt);

	//б��
	double Slope();

	//ƫ���߶�
	int Offset(const double& x, const double& y){x1 += x;y1 += y;x2 += x;y2 += y;return 0;}

	//��ȡ�߶ε��е�
	POINT32F GetCenter(){return POINT32F((x1 + x2) / 2, (y1 + y2) / 2);}
	//��ȡ�㵽ֱ�ߵĴ��߶���ֱ�ߵĽ���
	POINT32F GetVerticalPoint(const POINT32F& pt);

	//��ȡ�߶���ĳ��ͼƬ�е�����������
	MyRect GetSquare(const int& nImgWidth, const int& nImgHeight, const int nSideLength);
	//��ȡ�߶����ڵ���С�������򣬶�����С���
	MyRect GetRect(const int& nMinGap = 8);
	//��ȡ�߶���ͼ���ڲ�����С�������򣬶�����С���
	MyRect GetRect(const int& nImgWidth, const int& nImgHeight, const int& nMinGap = 8);

	//�߶�դ��
	std::vector<POINT32F> Raster(const double& lfPtGap = 1.0);
	
	//ֱ���������εĽ���
	static bool IsLineInterRect(const double& a, const double& b, const double& c, const MyRect& rect, POINT32F& pt1, POINT32F& pt2);

	//���߶�ab�Ƿ����߶�cd�ཻ������Ϊp��1�淶�ཻ��0������һ�߶εĶ˵㣬-1���ཻ��
	static int IsAbInterCdSeg(POINT32F a, POINT32F b, POINT32F c, POINT32F d, POINT32F& pInter);
	//�ж�ֱ���Ƿ��ཻ��-1�쳣��0�ཻ��1ƽ��, 2�غ�
	static int IsLineInterLine(const double& a1, const double& b1, const double& c1, const double& a2, const double& b2, const double& c2, POINT32F& ptInter);
	//�ж�ֱ���Ƿ����߶��ཻ��-1�쳣��0������һ�߶εĶ˵�, 1�淶�ཻ��2ƽ��, 3�غ�
	static int IsLineInterSeg(const double& a, const double& b, const double& c, POINT32F pt1, POINT32F pt2, POINT32F& ptInter);
	//�������� ���ax + by + c = 0��ֱ�߷��̵Ĳ�����-1�쳣��0������1��ֱ��x��2��ֱ��y
	static int CalcLineABC(double x1, double y1, double x2, double y2, double& a, double& b, double& c);
	//���ݽǶ�[0.0 - 360.0] ���ax + by + c = 0��ֱ�߷��̵Ĳ���
	static int CalcLineABC(double dAngle, double x, double y, double& a, double& b, double& c);
	//���ֱ���ϵ�ĳ����
	static int CalcLineXByABC(const double& a, const double& b, const double& c, double& x, const double& y){if (dblcmp(a, 0.0) == 0)return -1;x = (-b * y - c) / a;return 0;}
	static int CalcLineYByABC(const double& a, const double& b, const double& c, const double& x, double& y){if (dblcmp(b, 0.0) == 0)return -1;y = (-a * x - c) / b;return 0;}

	//Ĭ�Ϸ�ΧΪ[0��360], flagΪ1ʱ����ΧΪ[-180, 180]
	static int AngleSeg(double x1, double y1, double x2, double y2, int flag = 0);
	//��a���ǲ������߶�bc�ϣ�>0���ڣ�=0��˵��غϣ�<0�ڡ�
	static int PointOnSeg(POINT32F a,POINT32F b,POINT32F c){return dblcmp(dot(b.x-a.x,b.y-a.y,c.x-a.x,c.y-a.y), 0.0);}

private:
	//���ƱȽϴ�С
	static int dblcmp(double a,double b, double lfSnap = 1e-5){if (fabs(a-b) <= lfSnap) return 0;if (a>b) return 1;else return -1;}

	//���
	static double dot(double x1,double y1,double x2,double y2){return x1*x2+y1*y2;}

	static double cross(double x1,double y1,double x2,double y2){return x1*y2-x2*y1;}

	//ab��ac�Ĳ��
	static double ab_cross_ac(POINT32F a,POINT32F b,POINT32F c){return cross(b.x-a.x,b.y-a.y,c.x-a.x,c.y-a.y);}
};

